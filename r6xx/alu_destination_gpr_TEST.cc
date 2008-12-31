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
#include <r6xx/alu_destination_gpr.hh>
#include <r6xx/error.hh>

using namespace gpu;
using namespace tests;

struct AluDestinationGPRParserTest :
    public Test
{
    AluDestinationGPRParserTest() :
        Test("alu_destination_gpr_test")
    {
    }

#define DATA std::pair<std::string, r6xx::alu::DestinationGPR>
#define DATA2 r6xx::alu::DestinationGPR
#define CHANNEL Enumeration<2>
#define INDEX Enumeration<7>
    virtual void run()
    {
        const static DATA valid_data[] = 
        {
            DATA("$0.x",    DATA2(CHANNEL(0), INDEX(0),   false)),
            DATA("$0.yr",   DATA2(CHANNEL(1), INDEX(0),   true)),
            DATA("$32.z",   DATA2(CHANNEL(2), INDEX(32),  false)),
            DATA("$127.wr", DATA2(CHANNEL(3), INDEX(127), true))
        };
        const static DATA * valid_data_begin(valid_data);
        const static DATA * valid_data_end(valid_data_begin + sizeof(valid_data) / sizeof(DATA));
        const static DATA invalid_data[] =
        {
            DATA("$-0.x",   DATA2(CHANNEL(0), INDEX(0), false)),
            DATA("$128.x",  DATA2(CHANNEL(0), INDEX(0), false)),
            DATA("$0",      DATA2(CHANNEL(0), INDEX(0), false)),
            DATA("$0.rx",   DATA2(CHANNEL(0), INDEX(0), false))
        };
        const static DATA * invalid_data_begin(invalid_data);
        const static DATA * invalid_data_end(invalid_data_begin + sizeof(invalid_data) / sizeof(DATA));

        for (const DATA * i(valid_data_begin), * i_end(valid_data_end) ; i != i_end ; ++i)
        {
            r6xx::alu::DestinationGPR gpr(r6xx::alu::DestinationGPRParser::parse(i->first));
            TEST_CHECK_EQUAL(gpr.channel, i->second.channel);
            TEST_CHECK_EQUAL(gpr.index, i->second.index);
            TEST_CHECK_EQUAL(gpr.relative, i->second.relative);
        }

        for (const DATA * i(invalid_data_begin), * i_end(invalid_data_end) ; i != i_end ; ++i)
        {
            TEST_CHECK_THROWS(r6xx::alu::DestinationGPRParser::parse(i->first), r6xx::DestinationGPRSyntaxError);
        }
    }
#undef DATA
#undef DATA2
#undef CHANNEL
#undef INDEX
} alu_destination_gpr_parser_test;
