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
#include <r6xx/generator.hh>
#include <utils/memory.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>
#include <fstream>

using namespace gpu;
using namespace tests;

struct GeneratorSectionsTest :
    public Test
{
    GeneratorSectionsTest() :
        Test("generator_sections_test")
    {
    }

    void run_one(const std::string & s)
    {
        std::string input_name(std::string(GPU_SRCDIR) + "/r6xx/generator_TEST_DATA/" + s + ".s");
        std::fstream input(input_name.c_str(), std::ios_base::in);

        AssemblyEntityPrinter p;
        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities(AssemblyParser::parse(input));

        r6xx::Generator g(entities);
        TEST_CHECK(g.end_sections() != std::find_if(g.begin_sections(), g.end_sections(), r6xx::SectionNameComparator(".alu")));
        TEST_CHECK(g.end_sections() != std::find_if(g.begin_sections(), g.end_sections(), r6xx::SectionNameComparator(".cf")));
    }

    virtual void run()
    {
        run_one("section");
        run_one("pushpop");
        run_one("shortcut");
    }
} assembly_parser_test;

