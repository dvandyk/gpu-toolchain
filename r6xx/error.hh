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

#ifndef GPU_GUARD_R6XX_ERROR_HH
#define GPU_GUARD_R6XX_ERROR_HH 1

#include <common/syntax.hh>

namespace gpu
{
    namespace r6xx
    {
        class SyntaxError :
            public gpu::SyntaxError
        {
            public:
                SyntaxError(const std::string & message);
        };

        class InvalidSectionNameError :
            public r6xx::SyntaxError
        {
            public:
                InvalidSectionNameError(const std::string & name);
        };

        class UnbalancedSectionStackError :
            public r6xx::SyntaxError
        {
            public:
                UnbalancedSectionStackError();
        };

        class SourceOperandSyntaxError :
            public r6xx::SyntaxError
        {
            public:
                SourceOperandSyntaxError(const std::string & message);
        };

        class SourceGPRSyntaxError :
            public r6xx::SyntaxError
        {
            public:
                SourceGPRSyntaxError(const std::string & message);
        };

        class DestinationGPRSyntaxError :
            public r6xx::SyntaxError
        {
            public:
                DestinationGPRSyntaxError(const std::string & message);
        };

        class DuplicateSymbolError :
            public Exception
        {
            public:
                DuplicateSymbolError(const std::string & symbol);
        };

        class UnresolvedSymbolError :
            public Exception
        {
            public:
                UnresolvedSymbolError(const std::string & symbol);
        };
    }
}

#endif
