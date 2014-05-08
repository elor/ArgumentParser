AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -Iinclude -DRELEASE

lib_LTLIBRARIES = libArgumentParser.la
libArgumentParser_la_SOURCES = src/ArgumentParser.cpp src/ArgumentParserInternals.cpp src/convert.cpp src/Argument.cpp

libArgumentParser_la_LDFLAGS = -version-info 0:0:0
include_HEADERS = include/ArgumentParser.h

