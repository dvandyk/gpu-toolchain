CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = gpu-read gpu-link

gpu_read_SOURCES = gpu-read.cc
gpu_read_LDADD = ../elf/libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la

gpu_link_SOURCES = gpu-link.cc
gpu_link_LDADD = ../r6xx/libgpur6xx.la ../elf/libgpuelf.la ../tests/libgputests.a ../utils/libgpuutils.la

