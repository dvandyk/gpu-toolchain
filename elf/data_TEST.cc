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
#include <elf/data.hh>
#include <utils/stringify.hh>

#include <string>

using namespace gpu;
using namespace tests;

struct ElfDataTest :
    public Test
{
    ElfDataTest() :
        Test("elf_data_test")
    {
    }

    void run()
    {
        elf::Data data(20);

        char input[] = "foobar";

        char output_a[] = "xxxxxx";
        char output_b[] = "xxx";

        char ref_a[] = "bar";
        char ref_b[] = "barfoo";

        data.write(0, input, 6);
        data.read(0, output_a, 6);
        TEST_CHECK_EQUAL(stringify(input), stringify(output_a));

        data.read(3, output_b, 3);
        TEST_CHECK_EQUAL(stringify(ref_a), stringify(output_b));

        data.read(3, output_a, 3);
        data.read(0, output_a + 3, 3);
        TEST_CHECK_EQUAL(stringify(ref_b), stringify(output_a));
    }
} elf_data_test;
