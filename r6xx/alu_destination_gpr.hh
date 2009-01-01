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

#ifndef GPU_GUARD_R6XX_ALU_DESTINATION_GPR_HH
#define GPU_GUARD_R6XX_ALU_DESTINATION_GPR_HH 1

#include <utils/enumeration.hh>

namespace gpu
{
    namespace r6xx
    {
        namespace alu
        {
            struct DestinationGPR
            {
                Enumeration<2> channel;

                Enumeration<7> index;

                bool relative;

                DestinationGPR(const Enumeration<2> & channel, const Enumeration<7> & index, bool relative);
            };

            struct DestinationGPRParser
            {
                static DestinationGPR parse(const std::string & input);
            };

            struct DestinationGPRPrinter
            {
                static std::string print(const DestinationGPR & input);
            };
        }
    }
}

#endif
