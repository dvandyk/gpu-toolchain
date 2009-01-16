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

#ifndef GPU_GUARD_R6XX_RELOCATION_HH
#define GPU_GUARD_R6XX_RELOCATION_HH 1

#include <r6xx/section.hh>
#include <utils/sequence.hh>

#include <string>

namespace gpu
{
    namespace r6xx
    {
        enum CFRelocationType
        {
            cfrel_alu_clause = 1,
            cfrel_alu_kcache0,
            cfrel_alu_kcache1,
            cfrel_branch,
            cfrel_loop_counter,
            cfrel_pic,
            cfrel_tex_clause
        };

        struct Relocation
        {
            const SectionId section;

            const std::string symbol;

            const unsigned type;

            Relocation(SectionId section, const std::string & symbol, unsigned type) :
                section(section),
                symbol(symbol),
                type(type)
            {
            }
        };

        struct RelocationScanner
        {
            static Sequence<Relocation> scan(const Sequence<SectionPtr> & sections);
        };
    }
}

#endif
