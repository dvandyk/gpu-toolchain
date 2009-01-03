CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -I$(top_srcdir)
DEFS = \
       -DGPU_BUILDDIR=\"$(top_builddir)\" \
       -DGPU_SRCDIR=\"$(top_srcdir)\"
EXTRA_DIST =

noinst_LTLIBRARIES = libgpur6xx.la

libgpur6xx_la_SOURCES = \
	alu_destination_gpr.cc alu_destination_gpr.hh \
	alu_entities.cc alu_entities-fwd.hh alu_entities.hh \
	alu_section.cc alu_section.hh \
	alu_source_operand.cc alu_source_operand-fwd.hh alu_source_operand.hh \
	assembler.cc assembler.hh \
	cf_entities.cc cf_entities-fwd.hh cf_entities.hh \
	cf_section.cc cf_section.hh \
	error.cc error.hh \
	section.cc section-fwd.hh section.hh \
	tex_destination_gpr.cc tex_destination_gpr.hh \
	tex_entities.cc tex_entities-fwd.hh tex_entities.hh \
	tex_section.cc tex_section.hh \
	tex_source_gpr.cc tex_source_gpr.hh
libgpuutils_la_CXXFLAGS = -I$(top_srcdir)

TESTS = \
	alu_entities_TEST \
	alu_destination_gpr_TEST \
	alu_source_operand_TEST \
	assembler_TEST \
	section_TEST

check_PROGRAMS = $(TESTS)

alu_entities_TEST_SOURCES = alu_entities_TEST.cc
alu_entities_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

alu_destination_gpr_TEST_SOURCES = alu_destination_gpr_TEST.cc
alu_destination_gpr_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

alu_source_operand_TEST_SOURCES = alu_source_operand_TEST.cc
alu_source_operand_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

assembler_TEST_SOURCES = assembler_TEST.cc
assembler_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la
EXTRA_DIST += \
	assembler_TEST_DATA/section.s \
	assembler_TEST_DATA/pushpop.s \
	assembler_TEST_DATA/shortcut.s \
	assembler_TEST_DATA/alu.ref \
	assembler_TEST_DATA/cf.ref \
	assembler_TEST_DATA/minimal.s \
	assembler_TEST_DATA/minimal.ref

section_TEST_SOURCES = section_TEST.cc
section_TEST_LDADD = libgpur6xx.la ../common/libgpucommon.la ../tests/libgputests.a ../utils/libgpuutils.la

