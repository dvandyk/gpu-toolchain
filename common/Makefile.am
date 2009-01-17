CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
	-DGPU_BUILDDIR=\"$(top_builddir)\" \
	-DGPU_SRCDIR=\"$(top_srcdir)\" \
	-DGPU_VERSION=\"$(GPU_VERSION)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpucommon.la

libgpucommon_la_SOURCES = \
	assembly_entities.cc assembly_entities-fwd.hh assembly_entities.hh \
	assembly_parser.cc assembly_parser.hh \
	directives.cc directives.hh \
	expression.cc expression-fwd.hh expression.hh \
	gpgpu_notes_section.cc gpgpu_notes_section.hh \
	section.cc section.hh \
	syntax.cc syntax.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	assembly_entities_TEST \
	assembly_parser_TEST \
	expression_TEST

check_PROGRAMS = $(TESTS)

assembly_entities_TEST_SOURCES = assembly_entities_TEST.cc
assembly_entities_TEST_LDADD = libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

assembly_parser_TEST_SOURCES = assembly_parser_TEST.cc
assembly_parser_TEST_LDADD = libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
EXTRA_DIST += \
	assembly_parser_TEST_DATA/sum.s \
	assembly_parser_TEST_DATA/sum.ref

expression_TEST_SOURCES = expression_TEST.cc
expression_TEST_LDADD = libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
