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

#include <r6xx/alu_destination_gpr.hh>
#include <r6xx/error.hh>
#include <utils/destringify.hh>
#include <utils/stringify.hh>

namespace
{
    gpu::Enumeration<2> channel_from_suffix(const std::string & suffix)
    {
        try
        {
            switch (suffix[0])
            {
                case 'x':
                case 'y':
                case 'z':
                    return gpu::Enumeration<2>(suffix[0] - 'x');

                case 'w':
                    return gpu::Enumeration<2>(3);
            }
        }
        catch (gpu::EnumerationValueOutOfBoundsError & e)
        {
        }

        throw gpu::r6xx::DestinationGPRSyntaxError("'" + gpu::stringify(suffix[0]) + "' is not a valid channel");
    }

    bool relative_from_suffix(const std::string & suffix)
    {
        bool result(false);

        try
        {
            if (2 == suffix.size())
                result = ('r' == suffix[1]);

            return result;
        }
        catch (gpu::EnumerationValueOutOfBoundsError & e)
        {
        }

        throw gpu::r6xx::DestinationGPRSyntaxError("'" + gpu::stringify(suffix[1]) + "' is not a address mode");
    }
}

namespace gpu
{
    namespace r6xx
    {
        namespace alu
        {
            DestinationGPR::DestinationGPR(const Enumeration<2> & c, const Enumeration<7> & i, bool r) :
                channel(c),
                index(i),
                relative(r)
            {
            }

            DestinationGPR
            DestinationGPRParser::parse(const std::string & input)
            {
                const static std::string digits("0123456789");
                const static std::string modes("ar");

                if (input.empty())
                    throw InternalError("r6xx", "empty input");

                std::string operand(input);
                if (4 > operand.size())
                    throw DestinationGPRSyntaxError("too short for a destination GPR");

                if ('$' != operand[0])
                    throw DestinationGPRSyntaxError("destination GPR needs to start with '$'");

                operand.erase(0, 1);

                std::string::size_type sep(operand.find('.'));
                if (std::string::npos == sep)
                    throw DestinationGPRSyntaxError("no separator");

                std::string suffix(operand.substr(sep + 1));
                operand.erase(sep);

                if (std::string::npos != operand.find_first_not_of(digits))
                    throw DestinationGPRSyntaxError("'" + operand + "' is not a number");

                if ((suffix.empty()) || (2 < suffix.size()))
                    throw DestinationGPRSyntaxError("invalid suffix '" + stringify(suffix) + "'");

                Enumeration<2> channel(::channel_from_suffix(suffix));
                bool relative(::relative_from_suffix(suffix));
                unsigned index(destringify<unsigned>(operand));

                if (127 < index)
                    throw DestinationGPRSyntaxError("register index out of bounds");

                return DestinationGPR(channel, Enumeration<7>(index), relative);
            }

            std::string
            DestinationGPRPrinter::print(const DestinationGPR & input)
            {
                std::string result("DestinationGPR(");

                result += "channel=" + stringify(input.channel);
                result += ", index=" + stringify(input.index);
                result += ", relative=" + stringify(input.relative);

                result += ")";

                return result;
            }
        }
    }
}
