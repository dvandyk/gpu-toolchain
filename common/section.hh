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

#ifndef GPU_GUARD_COMMON_SECTION_HH
#define GPU_GUARD_COMMON_SECTION_HH 1

#include <common/assembly_entities-fwd.hh>
#include <elf/section.hh>
#include <elf/symbol.hh>
#include <elf/symbol_table.hh>
#include <utils/memory.hh>
#include <utils/sequence.hh>

#include <string>

namespace gpu
{
    class Section
    {
        public:
            virtual ~Section() = 0;

            virtual void append(const AssemblyEntityPtr &) = 0;

            virtual std::string name() const = 0;

            virtual Sequence<elf::Section> sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const = 0;

            virtual Sequence<elf::Symbol> symbols() const = 0;
    };

    typedef std::tr1::shared_ptr<Section> SectionPtr;

    struct SectionFactory
    {
        static bool valid(const std::string & name);

        static SectionPtr make(const std::string & name);
    };
}

#endif
