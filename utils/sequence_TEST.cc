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
#include <utils/sequence-impl.hh>

#include <string>

using namespace gpu;
using namespace tests;

struct SequenceTest :
    public Test
{
    SequenceTest() :
        Test("sequence_test")
    {
    }

    virtual void run()
    {
        Sequence<char> seq;

        seq.append('a');
        seq.append('b');
        seq.append('c');

        TEST_CHECK_EQUAL(3, std::distance(seq.begin(), seq.end()));

        std::string str(seq.begin(), seq.end());

        TEST_CHECK_EQUAL("abc", str);
    }
} sequence_test;

