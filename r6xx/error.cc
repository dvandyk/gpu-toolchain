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

#include <r6xx/error.hh>

namespace gpu
{
    namespace r6xx
    {
        SyntaxError::SyntaxError(const std::string & message) :
            gpu::SyntaxError("r6xx", message)
        {
        }

        InvalidSectionNameError::InvalidSectionNameError(const std::string & name) :
            SyntaxError("'" + name + "' is not a valid section name in the r6xx ISA")
        {
        }

        UnbalancedSectionStackError::UnbalancedSectionStackError() :
            SyntaxError("the section stack was not balanced (.pushsection/.popsection)")
        {
        }

        SourceOperandSyntaxError::SourceOperandSyntaxError(const std::string & message) :
            SyntaxError(message)
        {
        }

        SourceGPRSyntaxError::SourceGPRSyntaxError(const std::string & message) :
            SyntaxError(message)
        {
        }

        DestinationGPRSyntaxError::DestinationGPRSyntaxError(const std::string & message) :
            SyntaxError(message)
        {
        }
    }
}
