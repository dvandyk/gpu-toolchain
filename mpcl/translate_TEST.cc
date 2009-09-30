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
#include <mpcl/translate.hh>
#include <tests/tests.hh>

#include <iostream>

using namespace gpu;
using namespace tests;

struct SimpleTranslateTest :
    public Test
{
    SimpleTranslateTest() :
        Test("simple_translate_test")
    {
    }

    virtual void run()
    {
        static const std::string input =
            "float f(float x)\n"
            "{\n"
            "\tfloat denom;\n"
            "\tdenom = x * x + 1.0;\n"
            "\treturn 1.0 / denom;\n"
            "}\n";

        static const PILOperationPtr reference[] =
        {
            PILOperation::Unary(
                    PILOperand::Temporary(1),
                    pop_get_param,
                    PILOperand::Constant(Number(1u))),
            PILOperation::Unary(
                    PILOperand::Temporary(2),
                    pop_assign,
                    PILOperand::Undefined()),
            PILOperation::Unary(
                    PILOperand::Temporary(3),
                    pop_assign,
                    PILOperand::Temporary(1)),
            PILOperation::Unary(
                    PILOperand::Temporary(4),
                    pop_assign,
                    PILOperand::Temporary(1)),
            PILOperation::Binary(
                    PILOperand::Temporary(5),
                    pop_mul,
                    PILOperand::Temporary(3),
                    PILOperand::Temporary(4)),
            PILOperation::Unary(
                    PILOperand::Temporary(6),
                    pop_iload,
                    PILOperand::Constant(Number(1.0))),
            PILOperation::Binary(
                    PILOperand::Temporary(7),
                    pop_add,
                    PILOperand::Temporary(5),
                    PILOperand::Temporary(6)),
            PILOperation::Unary(
                    PILOperand::Temporary(8),
                    pop_assign,
                    PILOperand::Temporary(7)),
            PILOperation::Unary(
                    PILOperand::Temporary(9),
                    pop_iload,
                    PILOperand::Constant(Number(1.0))),
            PILOperation::Unary(
                    PILOperand::Temporary(10),
                    pop_assign,
                    PILOperand::Temporary(8)),
            PILOperation::Binary(
                    PILOperand::Temporary(11),
                    pop_div,
                    PILOperand::Temporary(9),
                    PILOperand::Temporary(10)),
            PILOperation::Unary(
                    PILOperand::Temporary(12),
                    pop_return,
                    PILOperand::Temporary(11))
        };

        std::stringstream ss(input);
        Parser p(ss);
        Sequence<FunctionPtr> functions(p.result());

        Translator t;
        Sequence<PILOperationPtr> operations(t.translate(functions.first()));
        TEST_CHECK_EQUAL(operations.size(), sizeof(reference) / sizeof(*reference));

        const PILOperationPtr * i(reference);
        for (Sequence<PILOperationPtr>::Iterator o(operations.begin()),
                o_end(operations.end()) ; o != o_end ; ++o, ++i)
        {
            TEST_CHECK_EQUAL(*(*o), *(*i));
        }
    }
} simple_translate_test;

struct MultipleFunctionsTranslateTest :
    public Test
{
    MultipleFunctionsTranslateTest() :
        Test("multiple_functions_translate_test")
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
            "}\n"
            "\n"
            "int32u c(int32u x)\n"
            "{\n"
            "\treturn a(x) * b(x);\n"
            "}\n";

        std::stringstream ss(input);
        Parser p(ss);
    }
} multiple_functions_translate_test;
