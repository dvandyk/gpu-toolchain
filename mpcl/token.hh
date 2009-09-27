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

#ifndef MPCL_GUARD_TOKEN_HH
#define MPCL_GUARD_TOKEN_HH 1

#include <utils/memory.hh>
#include <utils/sequence.hh>

#include <string>

namespace gpu
{
    enum TokenType
    {
        tt_invalid = 0,
        tt_comment,
        tt_delimiter,
        tt_float,
        tt_identifier,
        tt_integer,
        tt_operator,
        tt_linebreak
    };

    struct Token
    {
        std::string text;

        TokenType type;

        Token(const std::string text, TokenType type);

        ~Token();
    };

    extern template class Sequence<std::tr1::shared_ptr<Token> >;
}

#endif
