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

#include <r6xx/tex_destination_gpr.hh>
#include <r6xx/error.hh>
#include <utils/destringify.hh>
#include <utils/stringify.hh>

namespace gpu
{
    namespace r6xx
    {
        namespace tex
        {
            static bool relative_from_char(char c)
            {
                if ('r' == c)
                    return true;
                else if ('a' == c)
                    return false;
                else
                    throw DestinationGPRSyntaxError("invalid address mode '" + stringify(c) + "'");
            }

            static Enumeration<3> selector_element_from_char(char c)
            {
                switch (c)
                {
                    case 'x':
                    case 'y':
                    case 'z':
                        return Enumeration<3>(c - 'x');

                    case 'w':
                        return Enumeration<3>(3);

                    case '0':
                        return Enumeration<3>(4);

                    case '1':
                        return Enumeration<3>(5);

                    case 'm':
                        return Enumeration<3>(7);

                    default:
                        throw DestinationGPRSyntaxError("invalid selector element '" + stringify(c) + "'");
                }
            }

            static DestinationGPR::Selector selector_from_string(const std::string & input)
            {
                DestinationGPR::Selector result(Enumeration<3>(0), Enumeration<3>(1), Enumeration<3>(2), Enumeration<3>(3));
                std::string str(input);

                std::string::size_type size(str.size());

                if (size > 6)
                    throw DestinationGPRSyntaxError("'" + str + "' is too long for a valid selector");

                str.erase(size - 1);
                str.erase(0, 1);
                size -= 2;

                switch (size)
                {
                    case 4:
                        result.fourth = selector_element_from_char(str[3]);
                    case 3:
                        result.third = selector_element_from_char(str[2]);
                    case 2:
                        result.second = selector_element_from_char(str[1]);
                    case 1:
                        result.first = selector_element_from_char(str[0]);
                    case 0:
                        return result;
                }

                throw InternalError("r6xx", "selector_from_string went haywire");
            }

            DestinationGPR::DestinationGPR(const Enumeration<7> & i, bool r, const Selector & s) :
                index(i),
                relative(r),
                selector(s)
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
                if (2 > operand.size())
                    throw DestinationGPRSyntaxError("too short for a destination GPR");

                if ('$' != operand[0])
                    throw DestinationGPRSyntaxError("destination GPR needs to start with '$'");

                operand.erase(0, 1);

                std::string::size_type sel_begin(operand.find('['));
                std::string::size_type sel_end(operand.find(']'));

                if ((std::string::npos != sel_begin) && (std::string::npos == sel_end))
                    throw DestinationGPRSyntaxError("unterminated selector");

                if ((std::string::npos == sel_begin) && (std::string::npos != sel_end))
                    throw DestinationGPRSyntaxError("unexpected ']'");

                DestinationGPR::Selector selector(selector_from_string(operand.substr(sel_begin, sel_end)));
                std::string suffix(operand.substr(sel_end + 1));
                operand.erase(sel_begin);

                bool relative(false);
                if (! suffix.empty())
                    relative = relative_from_char(suffix[0]);

                if (std::string::npos != operand.find_first_not_of(digits))
                    throw DestinationGPRSyntaxError("register index '" + operand + "' is not a number");

                unsigned index(destringify<unsigned>(operand));

                if (127 < index)
                    throw DestinationGPRSyntaxError("register index out of bounds");

                return DestinationGPR(Enumeration<7>(index), relative, selector);
            }

            std::string
            DestinationGPRPrinter::print(const DestinationGPR & input)
            {
                std::string result("DestinationGPR(");

                result += "index=" + stringify(input.index);
                result += ", relative=" + stringify(input.relative);
                result += ", selector=<" + stringify(input.selector.first) + ", " + stringify(input.selector.second)
                        + ", " + stringify(input.selector.third) + ", " + stringify(input.selector.fourth) + ">";

                result += ")";

                return result;
            }
        }
    }
}
