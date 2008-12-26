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
#include <utils/exception.hh>

#include <algorithm>
#include <iostream>
#include <list>

namespace tests
{
    static std::list<Test *> tests;

    Test::Test(const std::string & name) :
        _name(name)
    {
        tests.push_back(this);
    }

    const std::string &
    Test::name() const
    {
        return _name;
    }

    TestFailed::TestFailed(const std::string & message) :
        _message(message)
    {
    }

    const std::string &
    TestFailed::message() const
    {
        return _message;
    }
}

int main(int, char **)
{
    tests::tests.sort();

    try
    {
        unsigned index(1);

        for (std::list<tests::Test *>::iterator t(tests::tests.begin()), t_end(tests::tests.end()) ;
                t != t_end ; ++t, ++index)
        {
            std::cout << "(" << index << "/" << tests::tests.size() << ") " << (*t)->name() << std::endl;
            (*t)->run();
            std::cout << "PASSED" << std::endl;
        }
    }
    catch (gpu::Exception & e)
    {
        std::cout << "FAILED: " << e.message() << std::endl;
        return EXIT_FAILURE;
    }
    catch (tests::TestFailed & e)
    {
        std::cout << "FAILED: " << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

