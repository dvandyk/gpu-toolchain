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
            "\tfmuladd $0.w, $0.x, $0.y, $0.z\n"
            ".groupend\n");
        const static std::string reference(
            "Label(text='foo')\n"
            "IndexMode(mode=4)\n"
            "Form2Instruction(\n"
            "\topcode=0,\n"
            "\tdestination=DestinationGPR(channel=0, index=127, relative=false),\n"
            "\tslots=1,\n"
            "\tsource[0]=SourceGPR(3, 33, false, false),\n"
            "\tsource[1]=SourceGPR(2, 17, false, false),\n"
            ")\n"
            "Form3Instruction(\n"
            "\topcode=16,\n"
            "\tdestination=DestinationGPR(channel=3, index=0, relative=false),\n"
            "\tslots=1,\n"
            "\tsource[0]=SourceGPR(0, 0, false, false),\n"
            "\tsource[1]=SourceGPR(1, 0, false, false),\n"
            "\tsource[2]=SourceGPR(2, 0, false, false),\n"
            ")\n"
            "GroupEnd()");
        std::stringstream input(text);

        Sequence<AssemblyEntityPtr> entities(AssemblyParser::parse(input));
        Sequence<r6xx::alu::EntityPtr> alu_entities(r6xx::alu::EntityConverter::convert(entities));

        TEST_CHECK_EQUAL(r6xx::alu::EntityPrinter::print(alu_entities), reference);
    }
} alu_destination_gpr_parser_test;
