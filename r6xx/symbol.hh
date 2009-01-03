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

#ifndef GPU_GUARD_R6XX_SYMBOL_HH
#define GPU_GUARD_R6XX_SYMBOL_HH 1

#include <r6xx/section.hh>
#include <utils/sequence.hh>

#include <string>

namespace gpu
{
    namespace r6xx
    {
        enum SectionId
        {
            sid_cf = 0,
            sid_alu = 1,
            sid_tex = 2
        };

        struct Symbol
        {
            const std::string name;

            unsigned offset;

            unsigned size;

            const SectionId section;

            Symbol(const std::string & name, unsigned offset, SectionId section) :
                name(name),
                offset(offset),
                size(0),
                section(section)
            {
            }
        };

        struct SymbolComparator
        {
            Symbol reference;

            SymbolComparator(const Symbol & s) :
                reference(s)
            {
            }

            bool operator() (const Symbol & s)
            {
                return (reference.name == s.name) && (reference.section == s.section);
            }
        };

        struct SymbolScanner
        {
            static Sequence<Symbol> scan(const Sequence<SectionPtr> & sections);
        };
    }
}

#endif
