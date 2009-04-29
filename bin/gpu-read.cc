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

#include <elf/file.hh>
#include <elf/relocation_table.hh>
#include <utils/exception.hh>
#include <utils/hexify.hh>
#include <utils/stringify.hh>

#include <iostream>
#include <iomanip>

using namespace gpu;

void
print_sections(const elf::File & file)
{
    unsigned index(0);
    unsigned count(std::distance(file.begin(), file.end()));
    const unsigned index_width(stringify(count).size() + 1);
    const unsigned name_width(20);
    const unsigned type_width(7);

    std::cout << "Sections" << std::endl;
    std::cout
        << std::setw(index_width)
        << std::left
        << "#";

    std::cout << "  ";

    std::cout
        << std::setw(name_width)
        << std::left
        << "Name";

    std::cout << "  ";

    std::cout
        << std::setw(type_width)
        << std::left
        << "Type";

    std::cout << std::endl;

    for (elf::File::Iterator s(file.begin()), s_end(file.end()) ;
            s != s_end ; ++s, ++index)
    {
        std::cout
            << std::setw(index_width)
            << std::right
            << index;

        std::cout << "  ";

        std::cout
            << std::setw(name_width)
            << std::left
            << s->name();

        if (name_width < s->name().size())
            std::cout
                << std::endl
                << std::setw(index_width + name_width + 2)
                << std::right
                << "";

        std::cout << std::endl;
    }
}

void
print_symbols(const elf::File & file)
{
    unsigned index(0);
    elf::SymbolTable symtab(file.symbol_table());
    unsigned count(std::distance(file.begin(), file.end()));
    const unsigned index_width(stringify(count).size() + 1);
    const unsigned value_width(10);
    const unsigned size_width(6);
    const unsigned name_width(25);
    const unsigned section_width(10);

    std::cout << "Symbols" << std::endl;
    std::cout
        << std::setw(index_width)
        << std::right
        << "#";

    std::cout << "  ";

    std::cout
        << std::setw(value_width)
        << std::left
        << "Value";

    std::cout << "  ";

    std::cout
        << std::setw(size_width)
        << std::left
        << "Size";

    std::cout << "  ";

    std::cout
        << std::setw(name_width)
        << std::left
        << "Name";

    std::cout << "  ";

    std::cout
        << std::setw(name_width)
        << std::left
        << "Section";

    std::cout << std::endl;

    for (elf::SymbolTable::Iterator s(symtab.begin()), s_end(symtab.end()) ;
            s != s_end ; ++s, ++index)
    {
        std::cout
            << std::setw(index_width)
            << std::right
            << index;

        std::cout << "  ";

        std::cout
            << std::setw(value_width)
            << std::setfill('0')
            << std::right
            << hexify(s->value)
            << std::setfill(' ');

        std::cout << "  ";

        std::cout
            << std::setw(size_width)
            << std::right
            << s->size;

        std::cout << "  ";

        std::cout
            << std::setw(name_width)
            << std::left
            << s->name;

        std::cout << "  ";

        std::cout
            << std::setw(section_width)
            << std::left
            << s->section;

        std::cout << std::endl;
    }
}

void
print_relocations(const elf::File & file)
{
    elf::File::Iterator i(file.find(".cf.rel"));

    if (file.end() == i)
    {
        std::cout << "This file does not contain relocations." << std::endl;
        return;
    }

    std::cout << "Relocations" << std::endl;

    elf::RelocationTable reltab(file.symbol_table());
    reltab.read(i->data());

    unsigned index(0);
    const unsigned count(std::distance(file.begin(), file.end()));
    const unsigned index_width(stringify(count).size() + 1);
    const unsigned offset_width(10);

    std::cout
        << std::setw(index_width)
        << std::right
        << "#";

    std::cout << "  ";

    std::cout
        << std::setw(offset_width)
        << std::left
        << "Offset";

    std::cout << "  ";

    std::cout
        << std::left
        << "Symbol + Addend";

    std::cout << std::endl;

    for (elf::RelocationTable::Iterator r(reltab.begin()), r_end(reltab.end()) ;
            r != r_end ; ++r, ++index)
    {
        std::cout
            << std::setw(index_width)
            << std::right
            << index;

        std::cout << "  ";

        std::cout
            << std::setw(offset_width)
            << std::left
            << hexify(r->offset);

        std::cout << "  ";

        std::cout
            << std::left
            << r->symbol
            << " + "
            << r->addend;

        std::cout << std::endl;
    }
}

int
main(int argc, char ** argv)
{
    try
    {
        for (int i(1) ; i < argc ; ++i)
        {
            std::string filename(argv[i]);
            elf::File file(elf::File::open(filename));

            print_sections(file);

            std::cout << std::endl;

            print_symbols(file);

            std::cout << std::endl;

            print_relocations(file);
        }
    }
    catch (Exception & e)
    {
        std::cout << e.message() << std::endl;
    }
}
