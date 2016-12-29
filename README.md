# C++ Kafka Broker Stub
[![Build Status](https://travis-ci.org/Wramberg/kafkabrokerstub.svg?branch=master)](https://travis-ci.org/Wramberg/kafkabrokerstub)
[![Coverage Status](https://coveralls.io/repos/github/Wramberg/kafkabrokerstub/badge.svg?branch=master)](https://coveralls.io/github/Wramberg/kafkabrokerstub?branch=master)

A simple, header-only C++ Kafka broker stub for isolated unittests. Note that the repo is still under construction.

## Support
The broker stub currently supports
- Metadata requests [API version 0]
- Produce requests [API version 0]

## Future Development
The stub was developed to support exactly what was needed and nothing more. The basis for future development has been laid however. The stub is structured in a hierarchical fashion (think composite design pattern) where all primitive types have been implemented. It should thus be straightforward to add support for more requests/versions.

## Usage
TBD

