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

#ifndef GPU_GUARD_TESTS_TESTS_HH
#define GPU_GUARD_TESTS_TESTS_HH 1

#include <utils/stringify.hh>

#include <string>

namespace tests
{
    class Test
    {
        private:
            std::string _name;

        public:
            Test(const std::string & name);

            const std::string & name() const;

            virtual void run() = 0;
    };

    class TestFailed
    {
        private:
            std::string _message;

        public:
            TestFailed(const std::string & message);

            const std::string & message() const;
    };

#define TEST_CHECK(expr) \
    do \
    { \
        if (! (expr)) \
            throw TestFailed("Expected '" + gpu::stringify(#expr) + "' to be true, but was false"); \
    } \
    while (false)

#define TEST_CHECK_EQUAL(a, b) \
    do \
    { \
        if (a != b) \
            throw TestFailed("Expected '" + gpu::stringify(#a) + "' to equal\n--\n" + gpu::stringify(b) + "\n--\nbut got\n--\n" + gpu::stringify(a) + "\n--"); \
    } \
    while (false)

#define TEST_CHECK_THROWS(a, E_) \
    do \
    { \
        try \
        { \
            a; \
        } \
        catch (E_ & e) \
        { \
            break; \
        } \
        throw TestFailed("Expected exception of type '" + gpu::stringify(#E_) + "', but none such exception was thrown"); \
    } \
    while (false);
}

#endif
