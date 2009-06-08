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

#include <mpcl/parser.hh>
#include <mpcl/scanner.hh>
#include <tests/tests.hh>

using namespace gpu;
using namespace tests;

struct SimpleParserTest :
    public Test
{
    SimpleParserTest() :
        Test("simple_parser_test")
    {
    }

    virtual void run()
    {
        static const std::string input =
            "float f(float x)\n"
            "{\n"
            "\tfloat denom = x * x + 1;\n"
            "\treturn 1 / denom;\n"
            "}\n";

        std::stringstream ss(input);
        Scanner s(ss);
        s.scan();

        Parser p(s.tokens());
        p.parse();
    }
} simple_parser_test;
