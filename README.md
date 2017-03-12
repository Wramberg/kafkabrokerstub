# C++ Kafka Broker Stub
[![Build Status](https://travis-ci.org/Wramberg/kafkabrokerstub.svg?branch=master)](https://travis-ci.org/Wramberg/kafkabrokerstub)
[![Coverage Status](https://coveralls.io/repos/github/Wramberg/kafkabrokerstub/badge.svg?branch=master)](https://coveralls.io/github/Wramberg/kafkabrokerstub?branch=master)

A simple, header-only C++ Kafka broker stub for isolated unittests. Note that the repo is still under construction.

## Support
The broker stub currently supports
- Metadata requests [API version 0]
- Produce requests [API version 0]

## Future Development
The stub was developed due to lack of any other C++ broker stubs. The authors requirements are very limited, however, so the stub only supports a small number of requests. The basis for future development has been laid though. The stub is structured in a hierarchical fashion (think composite design pattern) where all primitive types have been implemented. It should thus be straightforward to add support for more requests/versions.

## Usage
To use the broker stub a TCP server must first be implemented. This is not included in the broker stub to avoid enforcing a particular library or architecture-specific approach on the developer.

