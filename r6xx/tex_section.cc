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

#include <common/assembly_entities.hh>
#include <common/expression.hh>
#include <r6xx/error.hh>
#include <r6xx/tex_section.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>

#include <elf.h>

namespace gpu
{
    template <>
    ConstVisits<r6xx::tex::Section>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace tex
        {
            namespace internal
            {
                struct SymbolScanner :
                    public tex::EntityVisitor
                {
                    Sequence<elf::Symbol> symbols;

                    unsigned current_offset;

                    SymbolScanner(const Sequence<tex::EntityPtr> & tex_entities) :
                        current_offset(0)
                    {
                        add_symbol(".tex", 0, STT_SECTION);

                        for (Sequence<tex::EntityPtr>::Iterator i(tex_entities.begin()), i_end(tex_entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        set_symbol_size(".tex", current_offset);
                    }

                    void add_symbol(const std::string & name, unsigned offset, unsigned type = 0)
                    {
                        elf::Symbol symbol(name);
                        symbol.section = ".tex";
                        symbol.type = type;
                        symbol.value = offset;

                        if (symbols.end() != std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)))
                            throw DuplicateSymbolError(name);

                        symbols.append(symbol);
                    }

                    void set_symbol_size(const std::string & name, unsigned size)
                    {
                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        s->size = size;
                    }

                    void set_symbol_type(const std::string & name, unsigned type)
                    {
                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        s->type = type;
                    }

                    unsigned symbol_lookup(const std::string & name)
                    {
                        if ("." == name)
                            return current_offset;

                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        return s->value;
                    }

                    // tex::EntityVisitor
                    void visit(const tex::LoadInstruction &)
                    {
                        current_offset += 16; // size of a tex instruction
                    }

                    void visit(const tex::Label & l)
                    {
                        add_symbol(l.text, current_offset);
                    }

                    void visit(const tex::Size & s)
                    {
                        ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolScanner::symbol_lookup), *this, std::tr1::placeholders::_1));
                        set_symbol_size(s.symbol, e.evaluate(s.expression));
                    }

                    void visit(const tex::Type & t)
                    {
                        set_symbol_type(t.symbol, t.type);
                    }
                };
            }

            Section::Section()
            {
            }

            Section::~Section()
            {
            }

            void
            Section::append(const AssemblyEntityPtr & ptr)
            {
                EntityPtr converted(EntityConverter::convert(ptr));

                if (0 != converted.get())
                {
                    entities.append(converted);
                }
            }

            std::string
            Section::name() const
            {
                return ".tex";
            }

            void
            Section::accept(SectionVisitor & v) const
            {
                static_cast<ConstVisits<r6xx::tex::Section> *>(&v)->visit(*this);
            }

            Sequence<elf::Section>
            Section::sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const
            {
                elf::Section tex_section(elf::Section::Parameters()
                        .alignment(0x10)
                        .flags(SHF_ALLOC | SHF_EXECINSTR)
                        .name(".tex")
                        .type(SHT_PROGBITS));
                Sequence<elf::Section> result;

                result.append(tex_section);

                return result;
            }

            Sequence<elf::Symbol>
            Section::symbols() const
            {
                internal::SymbolScanner ss(entities);

                return ss.symbols;
            }
        }
    }
}
