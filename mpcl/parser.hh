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

#ifndef MPCL_GUARD_PARSER_HH
#define MPCL_GUARD_PARSER_HH 1

#include <mpcl/function.hh>
#include <mpcl/token.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/sequence.hh>

#include <tr1/memory>

namespace gpu
{
    enum ParserErrorType
    {
        pet_expected_semicolon,
        pet_expected_closing_brace,
        pet_expected_closing_chevron,
        pet_expected_closing_parenthesis
    };

    struct ParserErrorMessage
    {
        ParserErrorType type;

        unsigned line;

        std::string message;

        ParserErrorMessage(ParserErrorType type, unsigned line, const std::string & message);

        bool operator!= (const ParserErrorMessage & other);
    };

    std::ostream & operator<< (std::ostream &, const ParserErrorMessage &);

    extern template class Sequence<ParserErrorMessage>;

    extern template class WrappedForwardIterator<Sequence<ParserErrorMessage>::IteratorTag, ParserErrorMessage>;

    class Parser :
        public PrivateImplementationPattern<Parser>
    {
        public:
            Parser(const Sequence<std::tr1::shared_ptr<Token> > & tokens);

            ~Parser();

            Sequence<FunctionPtr> parse();

            Sequence<ParserErrorMessage> errors();
    };
}

#endif
