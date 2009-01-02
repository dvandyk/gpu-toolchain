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

#include <common/assembly_entities.hh>
#include <common/assembly_parser.hh>
#include <common/expression.hh>
#include <utils/sequence-impl.hh>
#include <utils/text_manipulation.hh>

#include <string>
#include <map>

namespace
{
    std::string split_comment(std::string & line)
    {
        std::string::size_type pos(line.find('#'));
        std::string result;

        if (std::string::npos != pos)
        {
            result = line.substr(pos);
            line.erase(pos);
        }

        return result;
    }

    std::string split_label(std::string & line)
    {
        std::string::size_type pos(line.find_first_of(": \t"));
        std::string result;

        if ((std::string::npos != pos) && (':' == line[pos]))
        {
            result = line.substr(0, pos);
            line.erase(0, pos + 1);
        }

        return result;
    }

    bool is_directive(const std::string & s)
    {
        return '.' == s[0];
    }
}

namespace gpu
{
    static std::tr1::shared_ptr<AssemblyEntity> make_directive(const std::string & name, const std::string & params)
    {
#define RAW_DATA(n, x) std::pair<const std::string, unsigned>(n, x)
        const static std::pair<const std::string, unsigned> raw_data[] = {
            RAW_DATA("byte", 1),
            RAW_DATA("word", 4),
            RAW_DATA("long", 8)
        };
#undef RAW_DATA
        const static std::map<std::string, unsigned> data(raw_data, raw_data + sizeof(raw_data) / sizeof(raw_data[0]));

        std::tr1::shared_ptr<AssemblyEntity> result;

        std::map<std::string, unsigned>::const_iterator d(data.find(name));
        if (data.end() != d)
        {
            result = make_shared_ptr(new Data(d->second, ExpressionParser::parse(params)));
        }
        else
        {
            result = make_shared_ptr(new Directive(name, params));
        }

        return result;
    }

    Sequence<std::tr1::shared_ptr<AssemblyEntity> >
    AssemblyParser::parse(std::istream & input)
    {
        Sequence<std::tr1::shared_ptr<AssemblyEntity> > result;
        std::string line;
        unsigned number(0);

        while (std::getline(input, line))
        {
            ++number;
            result.append(make_shared_ptr(new Line(number)));

            line = strip_whitespaces(line);

            std::string comment(strip_whitespaces(split_comment(line)));
            std::string label(strip_whitespaces(split_label(line)));
            std::string rest(strip_whitespaces(line));

            if (! label.empty())
                result.append(make_shared_ptr(new Label(label)));

            if (! rest.empty())
            {
                if (is_directive(rest))
                {
                    std::string name(split_first_word("\t ", rest).substr(1));
                    std::string params(strip_whitespaces(rest));

                    result.append(make_directive(name, params));
                }
                else
                {
                    std::string mnemonic(split_first_word("\t ", rest));
                    std::string operands(strip_whitespaces(rest));

                    Instruction * i(new Instruction(mnemonic));
                    while (! operands.empty())
                    {
                        i->operands.append(split_first_word(",", operands));
                        operands = strip_whitespaces(operands);
                    }

                    result.append(make_shared_ptr(i));
                }
            }

            if (! comment.empty())
                result.append(make_shared_ptr(new Comment(comment)));
        }

        return result;
    }
}
