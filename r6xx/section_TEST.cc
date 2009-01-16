/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008, 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
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
#include <r6xx/error.hh>
#include <r6xx/section.hh>

using namespace gpu;
using namespace tests;

struct SectionTest :
    public Test
{
    SectionTest() :
        Test("section_test")
    {
    }

    void run_one(const std::string & n, bool v)
    {
        try
        {
            gpu::SectionPtr test(r6xx::Section::make(n));
        }
        catch (r6xx::InvalidSectionNameError & e)
        {
            TEST_CHECK_EQUAL(false, v);
            return;
        }

        TEST_CHECK_EQUAL(true, v);
    }

    virtual void run()
    {
        typedef std::pair<std::string, bool> TestData;
        const static TestData data[] = 
        {
            TestData(".alu", true),
            TestData(".cf", true),
            TestData(".text", false)
        };
        const static TestData * const data_begin(&data[0]);
        const static TestData * const data_end(data_begin + sizeof(data) / sizeof(TestData));

        for (const TestData * d(data_begin), * d_end(data_end) ; d != d_end ; ++d)
        {
            run_one(d->first, d->second);
        }
    }
} section_test;

