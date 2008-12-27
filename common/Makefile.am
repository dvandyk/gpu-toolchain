CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libgpucommon.la

libgpucommon_la_SOURCES = \
	assembly_entities.cc assembly_entities-fwd.hh assembly_entities.hh \
	expression.cc expression-fwd.hh expression.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	assembly_entities_TEST \
	expression_TEST

check_PROGRAMS = $(TESTS)

assembly_entities_TEST_SOURCES = assembly_entities_TEST.cc
assembly_entities_TEST_LDADD = libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la libgpucommon.la

expression_TEST_SOURCES = expression_TEST.cc
expression_TEST_LDADD = libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
