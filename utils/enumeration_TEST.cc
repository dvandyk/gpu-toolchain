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
#include <utils/enumeration.hh>

using namespace gpu;
using namespace tests;

struct EnumerationTest :
    public Test
{
    EnumerationTest() :
        Test("enumeration_test")
    {
    }

    virtual void run()
    {
        Enumeration<9> x(0), y(100);

        TEST_CHECK_EQUAL(x, 0);
        TEST_CHECK_EQUAL(y, 100);

        x = y;
        TEST_CHECK_EQUAL(x, 100);

        x = Enumeration<9>(77);
        TEST_CHECK_EQUAL(x, 77);

        TEST_CHECK_THROWS(x = Enumeration<9>(512), EnumerationValueOutOfBoundsError);
    }
} enumeration_test;
