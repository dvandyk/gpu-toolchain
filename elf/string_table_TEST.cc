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
#include <elf/string_table.hh>
#include <utils/stringify.hh>

#include <string>

using namespace gpu;
using namespace tests;

struct ElfStringTableTest :
    public Test
{
    ElfStringTableTest() :
        Test("elf_string_table_test")
    {
    }

    void run()
    {
        elf::StringTable string_table;

        TEST_CHECK_EQUAL(string_table["Everybody"], 1);

        TEST_CHECK_EQUAL(string_table["lies"], 11);

        TEST_CHECK_EQUAL(string_table["!"], 16);

        TEST_CHECK_EQUAL(string_table["Everybody"], 1);

        TEST_CHECK_EQUAL(string_table["!"], 16);
    }
} elf_string_table_test;
