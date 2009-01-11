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
#include <common/assembly_entities.hh>
#include <common/assembly_parser.hh>
#include <common/syntax.hh>
#include <r6xx/assembler.hh>
#include <r6xx/section.hh>
#include <utils/memory.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>
#include <fstream>

using namespace gpu;
using namespace tests;

struct AssemblerSectionsTest :
    public Test
{
    AssemblerSectionsTest() :
        Test("assembler_sections_test")
    {
    }

    void run_one(const std::string & s)
    {
        std::string input_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + s + ".s");
        std::fstream input(input_name.c_str(), std::ios_base::in);

        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities(AssemblyParser::parse(input));

        r6xx::Assembler g(entities);
        TEST_CHECK(g.end_sections() != std::find_if(g.begin_sections(), g.end_sections(), r6xx::SectionNameComparator(".alu")));
        TEST_CHECK(g.end_sections() != std::find_if(g.begin_sections(), g.end_sections(), r6xx::SectionNameComparator(".cf")));
    }

    virtual void run()
    {
        run_one("section");
        run_one("pushpop");
        run_one("shortcut");
    }
} assembler_sections_test;

struct AssemblerFilterTest :
    public Test
{
    AssemblerFilterTest() :
        Test("assembler_filter_test")
    {
    }

    void run_one(const std::string & s)
    {
        std::string input_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + s + ".s");
        std::fstream input(input_name.c_str(), std::ios_base::in);

        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities(AssemblyParser::parse(input));
        r6xx::Assembler g(entities);

        for (r6xx::Assembler::SectionIterator i(g.begin_sections()), i_end(g.end_sections()) ;
                i != i_end ; ++i)
        {
            std::string ref_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + (*i)->name().substr(1) + ".ref");
            std::fstream ref(ref_name.c_str(), std::ios_base::in);

            std::string ref_str, line;
            while (std::getline(ref, line))
            {
                if (! ref_str.empty())
                    ref_str += "\n";

                ref_str += line;
            }

            TEST_CHECK_EQUAL(r6xx::SectionPrinter::print(*i), ref_str);
        }
    }

    virtual void run()
    {
        run_one("section");
        run_one("pushpop");
        run_one("shortcut");
    }
} assembler_filter_test;

struct AssemblerTest :
    public Test
{
    AssemblerTest() :
        Test("assembler_test")
    {
    }

    void run_one(const std::string & s)
    {
        std::string input_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + s + ".s");
        SyntaxContext::File f(input_name);
        std::fstream input(input_name.c_str(), std::ios_base::in);

        std::string reference_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + s + ".ref");
        std::fstream reference(reference_name.c_str(), std::ios_base::in);
        std::string reference_string, line;
        while (std::getline(reference, line))
        {
            if (! reference_string.empty())
                reference_string += "\n";

            reference_string += line;
        }

        std::string symbols_name(std::string(GPU_SRCDIR) + "/r6xx/assembler_TEST_DATA/" + s + ".sym");
        std::fstream symbols(symbols_name.c_str(), std::ios_base::in);
        std::string symbols_string;
        while (std::getline(symbols, line))
        {
            if (! symbols_string.empty())
                symbols_string += "\n";

            symbols_string += line;
        }

        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities(AssemblyParser::parse(input));
        r6xx::Assembler a(entities);

        std::string section_output;
        for (r6xx::Assembler::SectionIterator i(a.begin_sections()), i_end(a.end_sections()) ;
                i != i_end ; ++i)
        {
            if (! section_output.empty())
                section_output += "\n";

            section_output += r6xx::SectionPrinter::print(*i);
        }
        TEST_CHECK_EQUAL(section_output, reference_string);

        std::string symbols_output;
        for (r6xx::Assembler::SymbolIterator i(a.begin_symbols()), i_end(a.end_symbols()) ;
                i != i_end ; ++i)
        {
            if (! symbols_output.empty())
                symbols_output += "\n";

            symbols_output += "Symbol(\n";
            symbols_output += "\tname='" + i->name + "'\n";
            symbols_output += "\toffset=" + stringify(i->offset) + "\n";
            symbols_output += "\tsize=" + stringify(i->size) + "\n";
            symbols_output += "\tsection=" + stringify(i->section) +"\n";
            symbols_output += ")";
        }
        TEST_CHECK_EQUAL(symbols_output, symbols_string);

    }

    virtual void run()
    {
        run_one("minimal");
    }
} assembler_test;
