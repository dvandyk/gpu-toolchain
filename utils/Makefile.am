CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)

noinst_LTLIBRARIES = libgpuutils.la

libgpuutils_la_SOURCES = \
	destringify.hh destringify.cc \
	enumeration.cc enumeration.hh \
	exception.cc exception.hh \
	hexify.cc hexify.hh \
	memory.hh \
	private_implementation_pattern.hh private_implementation_pattern-impl.hh \
	sequence.hh sequence-impl.hh \
	stringify.hh \
	text_manipulation.hh \
	visitor.hh visitor-impl.hh \
	wrapped_forward_iterator-fwd.hh wrapped_forward_iterator.hh wrapped_forward_iterator-impl.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	enumeration_TEST \
	hexify_TEST \
	sequence_TEST

check_PROGRAMS = $(TESTS)

enumeration_TEST_SOURCES = enumeration_TEST.cc
enumeration_TEST_LDADD = ../tests/libgputests.a libgpuutils.la

hexify_TEST_SOURCES = hexify_TEST.cc
hexify_TEST_LDADD = ../tests/libgputests.a libgpuutils.la

sequence_TEST_SOURCES = sequence_TEST.cc
sequence_TEST_LDADD = ../tests/libgputests.a libgpuutils.la
