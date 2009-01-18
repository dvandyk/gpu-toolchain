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

#include <elf/string_table.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/stringify.hh>

#include <algorithm>
#include <vector>

namespace gpu
{
    template <>
    struct Implementation<elf::StringTable>
    {
        unsigned offset;

        std::vector<std::pair<std::string, unsigned> > entries;

        Implementation() :
            offset(1) // let the first char in a string table always be '\0'
        {
        }
    };

    namespace elf
    {
        namespace internal
        {
            struct StringComparator
            {
                const std::string reference;

                StringComparator(const std::string & s) :
                    reference(s)
                {
                }

                bool operator() (const std::pair<const std::string, unsigned> & entry)
                {
                    return entry.first == reference;
                }
            };

            struct OffsetComparator
            {
                const unsigned reference;

                OffsetComparator(unsigned o) :
                    reference(o)
                {
                }

                bool operator() (const std::pair<const std::string, unsigned> & entry)
                {
                    return entry.second == reference;
                }
            };
        }

        StringTable::StringTable() :
            PrivateImplementationPattern<elf::StringTable>(new Implementation<elf::StringTable>)
        {
        }

        StringTable::~StringTable()
        {
        }

        unsigned
        StringTable::operator[] (const std::string & s)
        {
            std::vector<std::pair<std::string, unsigned> >::const_iterator e(std::find_if(
                        _imp->entries.begin(), _imp->entries.end(), internal::StringComparator(s)));
            if (_imp->entries.end() != e)
                return e->second;

            unsigned result(_imp->offset);

            _imp->entries.push_back(std::pair<std::string, unsigned>(s, result));
            _imp->offset += s.size() + 1;

            return result;
        }

        std::string
        StringTable::operator[] (unsigned o)
        {
            std::vector<std::pair<std::string, unsigned> >::const_iterator e(std::find_if(
                        _imp->entries.begin(), _imp->entries.end(), internal::OffsetComparator(o)));
            if (_imp->entries.end() != e)
                return e->first;

            throw InternalError("elf", "There is no string at offset '" + stringify(o) + "'");
        }

        void
        StringTable::read(const Data & data)
        {
            std::vector<char> raw_data(data.size());
            data.read(0, &raw_data[0], data.size());

            unsigned current_offset(1);
            while (current_offset < raw_data.size())
            {
                std::string s(&raw_data[current_offset]);
                _imp->entries.push_back(std::pair<std::string, unsigned>(s, current_offset));
                current_offset += s.size() + 1;
            }

            _imp->offset = current_offset;
        }

        void
        StringTable::write(Data data)
        {
            data.resize(_imp->offset);

            unsigned current_offset(1);
            for (std::vector<std::pair<std::string, unsigned> >::const_iterator e(_imp->entries.begin()), e_end(_imp->entries.end()) ;
                e != e_end ; ++e)
            {
                data.write(current_offset, e->first.c_str(), e->first.size());
                current_offset += e->first.size() + 1;
            }
        }
    }
}
