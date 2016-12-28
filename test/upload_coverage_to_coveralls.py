import argparse
import hashlib
import io
import os
import re
import subprocess
import sys
import requests
import json


def create_args(params):
    parser = argparse.ArgumentParser('coveralls')
    parser.add_argument('--gcov-options', metavar='GCOV_OPTS', default='',
                        help='set the options given to gcov')
    parser.add_argument('-i', '--include', metavar='DIR|FILE', action='append',
                        help='set include file or directory', default=[])

    return parser.parse_args(params)


def collect_gcov_report(args):
    for root, dirs, files in os.walk('.', followlinks=False):
        gcov_file_reports = {}
        for filepath in files:
            basename, ext = os.path.splitext(filepath)
            if ext == '.gcno':
                print "\n-----------------------------------------------"
                print "Checking code coverage in", filepath
                print "-----------------------------------------------"

                gcov_files = []
                path = os.path.abspath(os.path.join(root, basename))
                subprocess.call(
                    'cd "%s" && gcov %s "%s" > /dev/null' % (root, args.gcov_options, filepath),
                    shell=True)
                analyze_gcov_result(args, gcov_file_reports)

    return gcov_file_reports


def analyze_gcov_result(args, file_reports={}):
    report = {}
    discovered_files = set()
    abs_root = os.path.abspath('.')
    for root, dirs, files in os.walk('.', followlinks=False):
        dirs[:] = filter_dirs(root, dirs)
        for filepath in files:
            if os.path.splitext(filepath)[1] == '.gcov':
                gcov_path = os.path.join(os.path.join(root, filepath))
                print "Analyzing", gcov_path
                with open(gcov_path, mode='rb') as fobj:
                    source_file_line = fobj.readline().decode('utf-8', 'replace')
                    source_file_path = source_file_line.split(':')[-1].strip()
                    if not os.path.isabs(source_file_path):
                        if not source_file_path.startswith(os.path.pardir + os.path.sep) and \
                                os.path.dirname(source_file_path):
                            the_root = abs_root
                        else:
                            the_root = root
                        source_file_path = os.path.abspath(
                            os.path.join(the_root, source_file_path))

                    src_path = os.path.relpath(source_file_path, abs_root)
                    if src_path.startswith(os.path.pardir + os.path.sep):
                        continue
                    if is_not_included_path(args, source_file_path):
                        continue

                    src_report = {}
                    src_report['name'] = src_path.replace(os.path.sep, '/') # turn into posix path
                    discovered_files.add(src_path)
                    with io.open(source_file_path, mode='rb') as src_file:
                        src_report['source_digest'] = hashlib.md5(src_file.read()).hexdigest()

                    src_report['coverage'] = parse_gcov_file(fobj, gcov_path)
                    if src_path in file_reports:
                        file_reports[src_path] = combine_reports(file_reports[src_path], src_report)
                    else:
                        file_reports[src_path] = src_report
    return


def is_not_included_path(args, filepath):
    abspath = os.path.abspath(filepath)

    for incl_path in args.include:
        absolute_include_path = os.path.abspath(os.path.join('.', incl_path))
        if is_child_dir(absolute_include_path, abspath):
            return False
    return True


def is_child_dir(parent, child):
    rel = os.path.relpath(child, parent)
    return (not rel.startswith(os.pardir))


def filter_dirs(root, dirs):
    filtered_dirs = []
    for dirpath in dirs:
        abspath = os.path.abspath(os.path.join(root, dirpath))
        if os.path.basename(abspath) != ".git":
            filtered_dirs.append(dirpath)
    return filtered_dirs


def parse_gcov_file(fobj, filename):
    coverage = []
    ignoring = False
    for line in fobj:
        report_fields = line.decode('utf-8', 'replace').split(':', 2)
        if len(report_fields) == 1:
            continue

        cov_num = report_fields[0].strip()
        line_num = int(report_fields[1].strip())
        text = report_fields[2]
        if line_num == 0:
            continue
        if re.search(r'\bLCOV_EXCL_START\b', text):
            if ignoring:
                sys.stderr.write("Warning: %s:%d: nested LCOV_EXCL_START, "
                                 "please fix\n" % (filename, line_num))
            ignoring = True
        elif re.search(r'\bLCOV_EXCL_(STOP|END)\b', text):
            if not ignoring:
                sys.stderr.write("Warning: %s:%d: LCOV_EXCL_STOP outside of "
                                 "exclusion zone, please fix\n" % (filename,
                                                                   line_num))
            if 'LCOV_EXCL_END' in text:
                sys.stderr.write("Warning: %s:%d: LCOV_EXCL_STOP is the "
                                 "correct keyword\n" % (filename, line_num))
            ignoring = False
        if cov_num == '-':
            coverage.append(None)
        elif cov_num == '#####':
            # Avoid false positives.
            if (
                ignoring or
                text.lstrip().startswith(('inline', 'static')) or
                text.strip() == '}' or
                re.search(r'\bLCOV_EXCL_LINE\b', text)
            ):
                coverage.append(None)
            else:
                coverage.append(0)
        elif cov_num == '=====':
            # This is indicitive of a gcov output parse
            # error.
            coverage.append(0)
        else:
            coverage.append(int(cov_num))
    return coverage

def combine_reports(original, new):
    print "Combining with previous results for", original['name']
    report = {}
    report['name'] = original['name']
    report['source_digest'] = original['source_digest']
    coverage = []
    for original_num, new_num in zip(original['coverage'], new['coverage']):
        if original_num is None:
            coverage.append(new_num)
        elif new_num is None:
            coverage.append(original_num)
        else:
            coverage.append(original_num + new_num)

    report['coverage'] = coverage
    return report


def post_report(report):
    response = requests.post("https://coveralls.io/api/v1/jobs", files={'json_file': json.dumps(report)})
    try:
        result = response.json()
    except ValueError:
        result = {'error': 'Failure to submit data. '
                  'Response [%(status)s]: %(text)s' % {
                      'status': response.status_code,
                      'text': response.text}}
    print(result)
    if 'error' in result:
        return False
    return True


if __name__ == "__main__":
    args = create_args(sys.argv[1:])
    cov_report = collect_gcov_report(args)
    coveralls_report = {'source_files': list(cov_report.values())}

    print "\n-----------------------------------------------"
    print "Final Coverage Report"
    print "-----------------------------------------------"
    print coveralls_report

    coveralls_report['repo_token'] = os.environ.get('COVERALLS_REPO_TOKEN')
    coveralls_report['service_name'] = 'travis-ci'
    coveralls_report['service_job_id'] = os.environ.get('TRAVIS_JOB_ID', '')

    if post_report(coveralls_report) == False:
        sys.exit(1)
    sys.exit(0)

