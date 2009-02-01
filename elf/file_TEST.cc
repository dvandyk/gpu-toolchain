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

#include <algorithm>
#include <vector>

#include <elf.h>

using namespace gpu;
using namespace tests;

struct ElfFileWriteTest :
    public Test
{
    ElfFileWriteTest() :
        Test("elf_file_write_test")
    {
    }

    void run()
    {
        std::string filename(stringify(GPU_BUILDDIR) + "/elf/file_TEST.output");
        elf::File file(elf::File::create(elf::File::Parameters()
                    .data(ELFDATA2LSB)
                    .machine(EM_PPC)
                    .type(ET_REL)));


        file.write(filename);
    }
} elf_file_write_test;

struct ElfFileReadTest :
    public Test
{
    ElfFileReadTest() :
        Test("elf_file_read_test")
    {
    }

    void run()
    {
        std::string filename(stringify(GPU_SRCDIR) + "/elf/file_TEST_DATA/minimal");
        elf::File file(elf::File::open(filename));

        std::vector<std::string> section_names;
        for (elf::File::Iterator i(file.begin()), i_end(file.end()) ; i != i_end ; ++i)
        {
            section_names.push_back(i->name());
        }

        TEST_CHECK(section_names.end() != find(section_names.begin(), section_names.end(), ".alu"));

        elf::SymbolTable symtab(file.symbol_table());

        TEST_CHECK_EQUAL(1, symtab[".cf"]);
        TEST_CHECK_EQUAL(2, symtab["main"]);
        TEST_CHECK_EQUAL(6, symtab[".tex"]);
    }
} elf_file_read_test;
