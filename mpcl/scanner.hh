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

#ifndef MPCL_GUARD_SCANNER_HH
#define MPCL_GUARD_SCANNER_HH 1

#include <mpcl/token.hh>
#include <utils/memory.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/sequence.hh>

#include <istream>
#include <tr1/functional>

namespace gpu
{
    class Scanner :
        public PrivateImplementationPattern<Scanner>
    {
        public:
            Scanner(std::istream & stream);

            ~Scanner();

            void scan();

            Sequence<std::tr1::shared_ptr<Token> > tokens() const;

            /* stateful scanning functions */
            void scan_comment();

            void scan_default();

            void scan_float_exp();

            void scan_float();

            void scan_identifier();

            void scan_integer();

            void scan_maybe_comment();

            void scan_maybe_comment_end();

            void scan_maybe_trailing_equal();

            void scan_maybe_trailing_asterisk();
    };
}

#endif
