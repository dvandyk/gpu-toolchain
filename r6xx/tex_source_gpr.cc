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

#include <r6xx/tex_source_gpr.hh>
#include <r6xx/error.hh>
#include <utils/destringify.hh>
#include <utils/stringify.hh>

namespace gpu
{
    namespace r6xx
    {
        namespace tex
        {
            SourceGPR::SourceGPR(const Enumeration<7> & i, bool r) :
                index(i),
                relative(r)
            {
            }

            SourceGPR
            SourceGPRParser::parse(const std::string & input)
            {
                const static std::string digits("0123456789");
                const static std::string modes("ar");

                if (input.empty())
                    throw InternalError("r6xx", "empty input");

                std::string operand(input);
                if (2 > operand.size())
                    throw SourceGPRSyntaxError("too short for a source GPR");

                if ('$' != operand[0])
                    throw SourceGPRSyntaxError("destination GPR needs to start with '$'");

                operand.erase(0, 1);

                bool relative(false);
                std::string::size_type suffix_begin(operand.find_last_not_of(digits));
                if ((operand.size() - 1) == suffix_begin)
                {
                    char suffix(operand[suffix_begin]);
                    operand.erase(suffix_begin);

                    if ('r' == suffix)
                    {
                        relative = true;
                    }
                    else if ('a' != suffix)
                    {
                        throw SourceGPRSyntaxError("invalid address mode '" + stringify(operand[suffix_begin]) + "'");
                    }
                }

                if (std::string::npos != operand.find_first_not_of(digits))
                    throw SourceGPRSyntaxError("'" + operand + "' is not a number");

                unsigned index(destringify<unsigned>(operand));

                if (127 < index)
                    throw SourceGPRSyntaxError("register index out of bounds");

                return SourceGPR(Enumeration<7>(index), relative);
            }

            std::string
            SourceGPRPrinter::print(const SourceGPR & input)
            {
                std::string result("SourceGPR(");

                result += "index=" + stringify(input.index);
                result += ", relative=" + stringify(input.relative);

                result += ")";

                return result;
            }
        }
    }
}
