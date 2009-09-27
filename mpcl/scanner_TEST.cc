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

#include <mpcl/scanner.hh>
#include <tests/tests.hh>

using namespace gpu;
using namespace tests;

struct IdentifierTest :
    public Test
{
    IdentifierTest() :
        Test("identifier_test")
    {
    }

    virtual void run()
    {
        {
            static std::string id_input =
                "id1 id2\tid3\t \tfoo_bar_baz67";

            std::stringstream ss(id_input);
            Scanner s(ss);
            s.scan();

            for (Sequence<std::tr1::shared_ptr<Token> >::Iterator i(s.tokens().begin()),
                    i_end(s.tokens().end()) ; i != i_end ; ++i)
            {
                TEST_CHECK_EQUAL((*i)->type, tt_identifier);
            }

            TEST_CHECK_EQUAL(s.tokens().size(), 4);
        }

        {
            static std::string nonid_input =
                "( ) < > * ** /* foo */";

            std::stringstream ss(nonid_input);
            Scanner s(ss);
            s.scan();

            for (Sequence<std::tr1::shared_ptr<Token> >::Iterator i(s.tokens().begin()),
                    i_end(s.tokens().end()) ; i != i_end ; ++i)
            {
                TEST_CHECK((*i)->type != tt_identifier);
            }

            TEST_CHECK_EQUAL(s.tokens().size(), 7);
        }
    }
} identifier_test;
