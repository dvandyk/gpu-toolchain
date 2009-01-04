CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpuelf.la

libgpuelf_la_SOURCES = \
	data.cc data.hh \
	section.cc section.hh \
	string_table.cc string_table.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	data_TEST \
	string_table_TEST

check_PROGRAMS = $(TESTS)

data_TEST_SOURCES = data_TEST.cc
data_TEST_LDADD = libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la

string_table_TEST_SOURCES = string_table_TEST.cc
string_table_TEST_LDADD = libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la
