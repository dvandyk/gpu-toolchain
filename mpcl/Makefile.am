CLEANFILES = *~ *.output
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpumpcl.la

libgpumpcl_la_SOURCES = \
	function.cc function.hh \
	parameter.cc parameter.hh \
	parser.cc parser.hh \
	scanner.cc scanner.hh \
	statement.cc statement-fwd.hh statement.hh \
	token.cc token.hh \
	translate.cc translate.hh

libgpumpcl_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	scanner_TEST \
	parser_TEST \
	translate_TEST

check_PROGRAMS = $(TESTS)

scanner_TEST_SOURCES = scanner_TEST.cc
scanner_TEST_LDADD = libgpumpcl.la ../tests/libgputests.a ../common/libgpucommon.la ../utils/libgpuutils.la

parser_TEST_SOURCES = parser_TEST.cc
parser_TEST_LDADD = libgpumpcl.la ../tests/libgputests.a ../common/libgpucommon.la ../utils/libgpuutils.la

translate_TEST_SOURCES = translate_TEST.cc
translate_TEST_LDADD = libgpumpcl.la ../tests/libgputests.a ../common/libgpucommon.la ../utils/libgpuutils.la

