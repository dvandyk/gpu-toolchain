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
#include <utils/exception.hh>
#include <utils/number.hh>

#include <iostream>

using namespace gpu;
using namespace tests;

struct NumberTest :
    public Test
{
    NumberTest() :
        Test("number_test")
    {
    }

    virtual void run()
    {
        Number n1(1.1f);
        Number n2(100u);
        Number n3(100);

        TEST_CHECK(1.1f == interpret_number_as<float>(n1));
        TEST_CHECK(100u == interpret_number_as<unsigned>(n2));
        TEST_CHECK(100 == interpret_number_as<signed>(n3));

        Number sum12(n1 + n2);
        Number sum13(n1 + n3);
        Number sum23(n2 + n3);

        TEST_CHECK(101.1f == interpret_number_as<float>(sum12));
        TEST_CHECK(101.1f == interpret_number_as<float>(sum13));
        TEST_CHECK(200 == interpret_number_as<signed>(sum23));

        TEST_CHECK_THROWS(interpret_number_as<signed>(sum12), InternalError);
        TEST_CHECK_THROWS(interpret_number_as<unsigned>(sum13), InternalError);
        TEST_CHECK_THROWS(interpret_number_as<unsigned>(sum23), InternalError);
    }
} number_test;
