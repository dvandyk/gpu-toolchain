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
#include <r6xx/alu_source_operand.hh>
#include <r6xx/error.hh>

using namespace gpu;
using namespace tests;

struct AluSourceOperandParserTest :
    public Test
{
    AluSourceOperandParserTest() :
        Test("alu_source_operand_parser_test")
    {
    }

#define DATA std::pair<std::string, std::string>
    virtual void run()
    {
        const static DATA valid_data[] = 
        {
            DATA("$0.x",     "SourceGPR(0, 0, false, false)"),
            DATA("$0.y",     "SourceGPR(1, 0, false, false)"),
            DATA("-$127.wr", "SourceGPR(3, 127, true, true)"),
            DATA("K0.z",     "SourceKCache(2, 0, false, false)"),
            DATA("-K63.ya",  "SourceKCache(1, 63, true, false)"),
            DATA("C0.xr",    "SourceCFile(0, 0, false, true)"),
            DATA("-C255.w",  "SourceCFile(3, 255, true, false)"),
            DATA("1.1",      "SourceLiteral(0x3f8ccccd)"),
            DATA("-1.1",     "SourceLiteral(0xbf8ccccd)"),
            DATA("10u",      "SourceLiteral(0x0000000a)"),
            DATA("-10",      "SourceLiteral(0xfffffff6)")
        };
        const static DATA * valid_data_begin(valid_data);
        const static DATA * valid_data_end(valid_data_begin + sizeof(valid_data) / sizeof(DATA));
        const static DATA invalid_data[] =
        {
            DATA("$-0.x",    ""),
            DATA("$128.x",   ""),
            DATA("$0",       ""),
            DATA("$0.rx",    ""),
            DATA("K64.x",    ""),
            DATA("K32.rz",   ""),
            DATA("C256.w",   "")
        };
        const static DATA * invalid_data_begin(invalid_data);
        const static DATA * invalid_data_end(invalid_data_begin + sizeof(invalid_data) / sizeof(DATA));

        r6xx::alu::SourceOperandPrinter p;

        for (const DATA * i(valid_data_begin), * i_end(valid_data_end) ; i != i_end ; ++i)
        {
            TEST_CHECK_EQUAL(p.print(r6xx::alu::SourceOperandParser::parse(i->first)), i->second);
        }

        for (const DATA * i(invalid_data_begin), * i_end(invalid_data_end) ; i != i_end ; ++i)
        {
            TEST_CHECK_THROWS(r6xx::alu::SourceOperandParser::parse(i->first), r6xx::SourceOperandSyntaxError);
        }
    }
#undef DATA
} alu_source_operand_parser_test;
