/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Danny van Dyk <danny.dyk@tu-dortmund.de>
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
#include <utils/hexify.hh>

using namespace gpu;
using namespace tests;

struct HexifyTest :
    public Test
{
    HexifyTest() :
        Test("hexify_test")
    {
    }

    virtual void run()
    {
        TEST_CHECK_EQUAL(hexify(1.5f), "0x3fc00000");
        TEST_CHECK_EQUAL(hexify(0x1234ABCD), "0x1234abcd");
        TEST_CHECK_EQUAL(hexify(10), "0x0000000a");
        TEST_CHECK_EQUAL(hexify(-1), "0xffffffff");
    }
} hexify_test;
