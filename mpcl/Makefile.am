CLEANFILES = *~ *.output
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpumpcl.la

libgpumpcl_la_SOURCES = \
	scanner.cc scanner.hh \
	token.cc token.hh

libgpumpcl_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	scanner_TEST

check_PROGRAMS = $(TESTS)

scanner_TEST_SOURCES = scanner_TEST.cc
scanner_TEST_LDADD = libgpumpcl.la ../tests/libgputests.a ../utils/libgpuutils.la
