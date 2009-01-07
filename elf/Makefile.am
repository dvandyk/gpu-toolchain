CLEANFILES = *~ *_output
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir) -lelf
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpuelf.la

libgpuelf_la_SOURCES = \
	data.cc data.hh \
	file.cc file.hh \
	error.cc error.hh \
	section.cc section.hh \
	string_table.cc string_table.hh \
	symbol_table.cc symbol_table.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	data_TEST \
	file_TEST \
	string_table_TEST

check_PROGRAMS = $(TESTS)

data_TEST_SOURCES = data_TEST.cc
data_TEST_LDADD = libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la

file_TEST_SOURCES = file_TEST.cc
file_TEST_LDADD = libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la

string_table_TEST_SOURCES = string_table_TEST.cc
string_table_TEST_LDADD = libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la
