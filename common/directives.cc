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

#include <common/directives.hh>
#include <common/expression.hh>
#include <common/syntax.hh>
#include <utils/text_manipulation.hh>

#include <algorithm>

#include <elf.h>

namespace gpu
{
    Tuple<std::string, ExpressionPtr>
    SizeParser::parse(const std::string & input)
    {
        std::string::size_type sep(input.find(','));

        if ((std::string::npos == sep) || (0 == sep) || ((input.size() - 1) == sep))
            throw CommonSyntaxError("Expected 2 operands, got 1 operand");

        std::string symbol(strip_whitespaces(input.substr(0, sep)));
        std::string expression(input.substr(sep + 1));

        return Tuple<std::string, ExpressionPtr>(symbol, ExpressionParser::parse(expression));
    }

    Tuple<std::string, unsigned>
    TypeParser::parse(const std::string & input)
    {
        typedef Tuple<std::string, unsigned> Type;
        typedef TupleComparator<Type, std::string, 1> TypeComparator;
        static const Type types[] =
        {
            Type("\"func\"",        STT_FUNC),
            Type("\"object\"",      STT_OBJECT),
            Type("\"tls_object\"",  STT_TLS),
            Type("\"common\"",      STT_COMMON)
        };
        static const Type * types_begin(types);
        static const Type * types_end(types + sizeof(types) / sizeof(Type));

        std::string::size_type sep(input.find(','));

        if ((std::string::npos == sep) || (0 == sep) || ((input.size() - 1) == sep))
            throw CommonSyntaxError("Expected 2 operands, got 1 operand");

        std::string symbol(strip_whitespaces(input.substr(0, sep)));
        std::string type(strip_whitespaces(input.substr(sep + 1)));
        const Type * t(std::find_if(types_begin, types_end, TypeComparator(type)));
        if (types_end == t)
            throw CommonSyntaxError("Not a valid symbol type: '" + strip_whitespaces(input.substr(sep + 1)) + "'");

        return Tuple<std::string, unsigned>(symbol, t->second);
    }
}
