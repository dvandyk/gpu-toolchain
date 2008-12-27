CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libgpucommon.la

libgpucommon_la_SOURCES = \
	assembly_entities.cc assembly_entities-fwd.hh assembly_entities.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	assembly_entities_TEST

check_PROGRAMS = $(TESTS)

assembly_entities_TEST_SOURCES = assembly_entities_TEST.cc
assembly_entities_TEST_LDADD = ../tests/libgputests.a ../utils/libgpuutils.la  libgpucommon.la
