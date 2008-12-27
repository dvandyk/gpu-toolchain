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

#include <common/expression.hh>
#include <tests/tests.hh>
#include <utils/exception.hh>
#include <utils/visitor-impl.hh>

#include <algorithm>
#include <map>
#include <vector>

using namespace gpu;
using namespace tests;

class ExpressionPrinter :
    public ExpressionVisitor
{
    public:
        std::string output;

        ExpressionPrinter(const ExpressionPtr & ptr)
        {
            ptr->accept(*this);
        }

        virtual ~ExpressionPrinter()
        {
        }

        virtual void visit(Difference & d)
        {
            output += "-(";

            d.left_hand_side()->accept(*this);

            output += ",";

            d.right_hand_side()->accept(*this);

            output += ")";
        }

        virtual void visit(Sum & s)
        {
            output += "+(";

            s.left_hand_side()->accept(*this);

            output += ",";

            s.right_hand_side()->accept(*this);

            output += ")";
        }

        virtual void visit(Value & v)
        {
            output += stringify(v.value());
        }

        virtual void visit(Variable & v)
        {
            output += v.variable();
        }
};

class ExpressionParserTest :
    public Test
{
    private:
        typedef std::pair<std::string, std::string> DataPair;

        std::vector<DataPair> data;

    public:
        ExpressionParserTest() :
            Test("expression_parser_test")
        {
            data.push_back(DataPair(". - main", "-(.,main)"));
        }

        void run_one(const DataPair & x)
        {
            ExpressionPtr expression(ExpressionParser::parse(x.first));
            TEST_CHECK(0 != expression.get());

            ExpressionPrinter printer(expression);
            TEST_CHECK_EQUAL(printer.output, x.second);
        }

        virtual void run()
        {
            std::for_each(data.begin(), data.end(),
                    std::tr1::bind(std::tr1::mem_fn(&ExpressionParserTest::run_one), this, std::tr1::placeholders::_1));
        }
} expression_parser_test;

class ExpressionEvaluatorTest :
    public Test
{
    private:
        typedef std::pair<std::string, unsigned> DataPair;

        std::vector<DataPair> data;

        typedef std::pair<const std::string, unsigned> SymbolPair;

        std::map<std::string, unsigned> symbols;

    public:
        ExpressionEvaluatorTest() :
            Test("expression_evaluator_test")
        {
            data.push_back(DataPair(".-main", 722));
            data.push_back(DataPair("0x1234", 0x1234));

            symbols.insert(SymbolPair(".", 745));
            symbols.insert(SymbolPair("main", 23));
        }

        unsigned operator() (const std::string & name)
        {
            return symbols[name];
        }

        void run_one(const DataPair & x)
        {
            ExpressionPtr expression(ExpressionParser::parse(x.first));
            TEST_CHECK(0 != expression.get());

            ExpressionEvaluator evaluator(*this);
            TEST_CHECK_EQUAL(evaluator.evaluate(expression), x.second);
        }

        virtual void run()
        {
            std::for_each(data.begin(), data.end(),
                    std::tr1::bind(std::tr1::mem_fn(&ExpressionEvaluatorTest::run_one), this, std::tr1::placeholders::_1));
        }
} expression_evaluator_test;

