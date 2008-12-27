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
#include <common/assembly_entities.hh>
#include <common/assembly_parser.hh>
#include <utils/memory.hh>
#include <utils/sequence-impl.hh>

#include <fstream>
#include <sstream>

using namespace gpu;
using namespace tests;

struct AssemblyParserTest :
    public Test
{
    AssemblyParserTest() :
        Test("assembly_parser_test")
    {
    }

    void run_one(const std::string & s)
    {
        std::string input_name(std::string(GPU_SRCDIR) + "/common/assembly_parser_TEST_DATA/" + s + ".s");
        std::string reference_name(std::string(GPU_SRCDIR) + "/common/assembly_parser_TEST_DATA/" + s + ".ref");

        std::fstream input(input_name.c_str(), std::ios_base::in);
        std::fstream reference(reference_name.c_str(), std::ios_base::in);

        AssemblyEntityPrinter p;
        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities(AssemblyParser::parse(input));
        for (Sequence<std::tr1::shared_ptr<AssemblyEntity> >::Iterator i(entities.begin()), i_end(entities.end()) ;
                i != i_end ; ++i)
        {
            (*i)->accept(p);
        }

        std::string ref_str(""), line;
        while (std::getline(reference, line))
        {
            ref_str += line + "\n";
        }

        TEST_CHECK_EQUAL(p.output(), ref_str);
    }

    virtual void run()
    {
        run_one("sum");
    }
} assembly_parser_test;

