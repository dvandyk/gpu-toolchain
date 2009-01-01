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
#include <common/assembly_parser.hh>
#include <r6xx/alu_entities.hh>
#include <r6xx/error.hh>

#include <string>

#include <iostream>

using namespace gpu;
using namespace tests;

struct AluEntitiesConverterTest :
    public Test
{
    AluEntitiesConverterTest() :
        Test("alu_entities_converter_test")
    {
    }

    virtual void run()
    {
        const static std::string text(
            "foo:\n"
            ".indexmode loop\n"
            "\tfadd $127.x, $33.w, $17.z\n"
            "\tfmuladd $0.w, $0.x, $0.y, $0.z\n");
        std::stringstream input(text);

        Sequence<AssemblyEntityPtr> entities(AssemblyParser::parse(input));
        Sequence<r6xx::alu::EntityPtr> alu_entities(r6xx::alu::EntityConverter::convert(entities));

        std::cout << text << std::endl;
        std::cout << "--" << std::endl;
        for (Sequence<r6xx::alu::EntityPtr>::Iterator i(alu_entities.begin()), i_end(alu_entities.end()) ;
                i != i_end ; ++i)
        {
            std::cout << r6xx::alu::EntityPrinter::print(*i) << std::endl;
        }
    }
} alu_destination_gpr_parser_test;
