CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpur6xx.la

libgpur6xx_la_SOURCES = \
	error.cc error.hh \
	generator.cc generator.hh \
	section.cc section.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	generator_TEST \
	section_TEST

check_PROGRAMS = $(TESTS)

generator_TEST_SOURCES = generator_TEST.cc
generator_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
EXTRA_DIST += \
	generator_TEST_DATA/section.s \
	generator_TEST_DATA/pushpop.s \
	generator_TEST_DATA/shortcut.s

section_TEST_SOURCES = section_TEST.cc
section_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

