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

#include <elf/symbol_table.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/tuple.hh>

#include <algorithm>
#include <cstring>
#include <map>
#include <vector>

#include <elf.h>

namespace gpu
{
    template <>
    struct Implementation<elf::SymbolTable>
    {
        std::map<std::string, unsigned> map;

        std::vector<elf::Symbol> entries;

        elf::StringTable strtab;

        Implementation(const elf::StringTable & strtab) :
            strtab(strtab)
        {
        }
    };

    namespace elf
    {
        Symbol::Symbol(const std::string & name, const std::string & section) :
            bind(0),
            name(name),
            section(section),
            size(0),
            type(0),
            value(0)
        {
        }

        bool
        Symbol::operator== (const Symbol & other) const
        {
            if (other.name != name)
                return false;

            if (other.section != section)
                return false;

            if (other.bind != bind)
                return false;

            if (other.type != type)
                return false;

            if (other.value != value)
                return false;

            if (other.size != size)
                return false;

            return true;
        }

        SymbolTable::SymbolTable(const StringTable & strtab) :
            PrivateImplementationPattern<elf::SymbolTable>(new Implementation<elf::SymbolTable>(strtab))
        {
        }

        SymbolTable::~SymbolTable()
        {
        }

        unsigned
        SymbolTable::operator[] (const std::string & name)
        {
            std::map<std::string, unsigned>::const_iterator e(_imp->map.find(name)), e_end(_imp->map.end());
            if (e == e_end)
                return 0;

            return e->second;
        }

        void
        SymbolTable::append(const Symbol & symbol)
        {
            std::map<std::string, unsigned>::const_iterator e(_imp->map.find(symbol.name)), e_end(_imp->map.end());
            if (e != e_end)
                throw InternalError("elf", "duplicate symbol");

            _imp->entries.push_back(symbol);
            _imp->map.insert(std::pair<const std::string, unsigned>(symbol.name, _imp->entries.size()));
        }

        void
        SymbolTable::write(const SectionTable & section_table, Data data)
        {
            std::vector<Elf32_Sym> symbols;

            Elf32_Sym dummy;
            memset(&dummy, 0, sizeof(dummy));
            symbols.push_back(dummy);

            for (std::vector<Symbol>::const_iterator s(_imp->entries.begin()), s_end(_imp->entries.end()) ;
                    s != s_end ; ++s)
            {
                Elf32_Sym symbol;
                symbol.st_name = _imp->strtab[s->name];
                symbol.st_info = ELF32_ST_INFO(s->bind, s->type);
                symbol.st_size = s->size;
                symbol.st_value = s->value;
                symbol.st_other = 0;
                symbol.st_shndx = section_table[s->section];

                symbols.push_back(symbol);
            }

            unsigned size(symbols.size() * sizeof(Elf32_Sym));
            data.resize(size);
            data.write(0, reinterpret_cast<char *>(&symbols[0]), size);
        }
    }
}
