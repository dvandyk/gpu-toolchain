CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libgputests.a

libgputests_a_SOURCES = \
	tests.cc \
	tests.hh
libgputests_a_CXXFLAGS = -I$(top_srcdir)

XFAIL_TESTS = test_fail_TEST
TESTS = test_pass_TEST

check_LIBRARIES = libgputests.a
check_PROGRAMS = $(TESTS) $(XFAIL_TESTS)

test_pass_TEST_SOURCES = test_pass_TEST.cc
test_pass_TEST_LDADD = libgputests.a ../utils/libgpuutils.la

test_fail_TEST_SOURCES = test_fail_TEST.cc
test_fail_TEST_LDADD = libgputests.a ../utils/libgpuutils.la
