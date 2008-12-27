CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libgpuutils.la

libgpuutils_la_SOURCES = \
	exception.cc exception.hh \
	memory.hh \
	private_implementation_pattern.hh private_implementation_pattern-impl.hh \
	sequence.hh sequence-impl.hh \
	stringify.hh \
	visitor.hh visitor-impl.hh \
	wrapped_forward_iterator-fwd.hh wrapped_forward_iterator.hh wrapped_forward_iterator-impl.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	sequence_TEST

check_PROGRAMS = $(TESTS)

sequence_TEST_SOURCES = sequence_TEST.cc
sequence_TEST_LDADD = ../tests/libgputests.a libgpuutils.la
