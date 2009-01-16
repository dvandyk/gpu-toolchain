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

#ifndef GPU_GUARD_R6XX_TEX_SECTION_HH
#define GPU_GUARD_R6XX_TEX_SECTION_HH 1

#include <r6xx/tex_entities.hh>
#include <r6xx/section.hh>
#include <utils/sequence.hh>

namespace gpu
{
    namespace r6xx
    {
        namespace tex
        {
            struct Section :
                public r6xx::Section
            {
                Sequence<EntityPtr> entities;

                Section();

                virtual ~Section();

                virtual void append(const AssemblyEntityPtr &);

                virtual std::string name() const;

                virtual Sequence<elf::Section> sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const;

                virtual Sequence<elf::Symbol> symbols() const;

                virtual void accept(SectionVisitor &) const;
            };
        }
    }
}

#endif
