CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpur6xx.la

libgpur6xx_la_SOURCES = \
	alu_source_operand.cc alu_source_operand-fwd.hh alu_source_operand.hh \
	error.cc error.hh \
	assembler.cc assembler.hh \
	section.cc section.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	alu_source_operand_TEST \
	assembler_TEST \
	section_TEST

check_PROGRAMS = $(TESTS)

alu_source_operand_TEST_SOURCES = alu_source_operand_TEST.cc
alu_source_operand_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

assembler_TEST_SOURCES = assembler_TEST.cc
assembler_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
EXTRA_DIST += \
	assembler_TEST_DATA/section.s \
	assembler_TEST_DATA/pushpop.s \
	assembler_TEST_DATA/shortcut.s \
	assembler_TEST_DATA/alu.ref \
	assembler_TEST_DATA/cf.ref

section_TEST_SOURCES = section_TEST.cc
section_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

