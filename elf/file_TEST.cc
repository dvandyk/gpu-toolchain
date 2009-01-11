/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
 *
 * This file is part of the GPU Toolchain. GPU Toolchain is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * GPU Toolchain is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <tests/tests.hh>
#include <elf/file.hh>
#include <utils/stringify.hh>

#include <elf.h>

using namespace gpu;
using namespace tests;

struct ElfFileTest :
    public Test
{
    ElfFileTest() :
        Test("elf_file_test")
    {
    }

    void run()
    {
        std::string filename(stringify(GPU_BUILDDIR) + "/elf/file_TEST.output");
        elf::File file(elf::File::Parameters()
                .data(ELFDATA2LSB)
                .machine(EM_PPC)
                .type(ET_REL));


        file.write(filename);
    }
} elf_file_test;
