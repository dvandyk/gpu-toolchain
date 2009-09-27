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

#include <iostream>

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
            "\tfloat denom;\n"
            "\tdenom = x * x + 1;\n"
            "\treturn 1 / denom;\n"
            "}\n";

        std::stringstream ss(input);
        Scanner s(ss);
        s.scan();

        Parser p(s.tokens());
        p.parse();
    }
} simple_parser_test;

struct BogusParserTest :
    public Test
{
    BogusParserTest() :
        Test("bogus_parser_test")
    {
    }

    virtual void run()
    {
        static const std::string input =
            "void invert(vector<float input\n"
            "{\n"
            "\tforeach (x in input\n"
            "\t{\n"
            "\t\tx = 1 / x\n"
            "\t}\n"
            "}\n";

        static const ParserErrorMessage reference[] =
        {
            ParserErrorMessage(pet_expected_closing_chevron, 1, "input"),
            ParserErrorMessage(pet_expected_closing_parenthesis, 2, "{"),
            ParserErrorMessage(pet_expected_closing_parenthesis, 4, "{"),
            ParserErrorMessage(pet_expected_semicolon, 6, "}")
        };

        std::stringstream ss(input);
        Scanner s(ss);
        s.scan();

        Parser p(s.tokens());
        TEST_CHECK_EQUAL(1, p.parse().size());

        Sequence<ParserErrorMessage> errors(p.errors());
        TEST_CHECK_EQUAL(sizeof(reference) / sizeof(*reference), errors.size());

        const ParserErrorMessage * j(reference);
        for (Sequence<ParserErrorMessage>::Iterator i(errors.begin()), i_end(errors.end()) ;
                i != i_end ; ++i, ++j)
        {
            TEST_CHECK_EQUAL(*i, *j);
        }
    }
} bogus_parser_test;

struct MultipleFunctionsParserTest :
    public Test
{
    MultipleFunctionsParserTest() :
        Test("multiple_functions_parser_test")
    {
    }

    virtual void run()
    {
        static const std::string input =
            "float a(float x)\n"
            "{\n"
            "\treturn 1 / x;\n"
            "}\n"
            "\n"
            "int32u b(int32u x)\n"
            "{\n"
            "\treturn x*x;\n"
            "}\n";

        std::stringstream ss(input);
        Scanner s(ss);
        s.scan();

        Parser p(s.tokens());
        p.parse();
    }
} multiple_functions_parser_test;
