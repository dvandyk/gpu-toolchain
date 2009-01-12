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

#include <common/expression.hh>
#include <r6xx/alu_entities.hh>
#include <r6xx/alu_section.hh>
#include <r6xx/cf_entities.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/symbol.hh>
#include <r6xx/tex_entities.hh>
#include <r6xx/tex_section.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>

#include <elf.h>

namespace gpu
{
    namespace r6xx
    {
        namespace internal
        {
            struct SymbolStage :
                public SectionVisitor,
                public alu::EntityVisitor,
                public cf::EntityVisitor,
                public tex::EntityVisitor
            {
                Sequence<Symbol> symbols;

                unsigned current_offset;

                std::string current_section;

                void add_symbol(const std::string & name, unsigned offset, unsigned type = 0)
                {
                    Symbol symbol(name, offset, current_section);
                    symbol.type = type;

                    if (symbols.end() != std::find_if(symbols.begin(), symbols.end(), SymbolComparator(symbol)))
                        throw DuplicateSymbolError(name);

                    symbols.append(symbol);
                }

                void set_symbol_size(const std::string & name, unsigned size)
                {
                    Sequence<Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), SymbolComparator(Symbol(name, 0, current_section))));
                    if (symbols.end() == s)
                        throw UnresolvedSymbolError(name);

                    s->size = size;
                }

                unsigned symbol_lookup(const std::string & name)
                {
                    if ("." == name)
                        return current_offset;

                    Sequence<Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), SymbolComparator(Symbol(name, 0, current_section))));
                    if (symbols.end() == s)
                        throw UnresolvedSymbolError(name);

                    return s->offset;
                }

                // SectionVisitor
                void visit(const alu::Section & a)
                {
                    current_offset = 0;
                    current_section = ".alu";

                    add_symbol(".alu", 0, STT_SECTION);

                    for (Sequence<alu::EntityPtr>::Iterator i(a.entities.begin()), i_end(a.entities.end()) ;
                            i != i_end ; ++i)
                    {
                        (*i)->accept(*this);
                    }

                    set_symbol_size(".alu", current_offset);
                }

                void visit(const cf::Section & c)
                {
                    current_offset = 0;
                    current_section = ".cf";

                    add_symbol(".cf", 0, STT_SECTION);

                    for (Sequence<cf::EntityPtr>::Iterator i(c.entities.begin()), i_end(c.entities.end()) ;
                            i != i_end ; ++i)
                    {
                        (*i)->accept(*this);
                    }

                    set_symbol_size(".cf", current_offset);
                }

                void visit(const tex::Section & t)
                {
                    current_offset = 0;
                    current_section = ".tex";

                    add_symbol(".tex", 0, STT_SECTION);

                    for (Sequence<tex::EntityPtr>::Iterator i(t.entities.begin()), i_end(t.entities.end()) ;
                            i != i_end ; ++i)
                    {
                        (*i)->accept(*this);
                    }

                    set_symbol_size(".tex", current_offset);
                }

                // alu::EntityVisitor
                void visit(const alu::GroupEnd &) { }
                void visit(const alu::IndexMode &) { }

                void visit(const alu::Form2Instruction &)
                {
                    current_offset += 8; // size of an alu instruction
                }

                void visit(const alu::Form3Instruction &)
                {
                    current_offset += 8; // size of an alu instruction
                }

                void visit(const alu::Label & l)
                {
                    add_symbol(l.text, current_offset);
                }

                void visit(const alu::Size & s)
                {
                    ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolStage::symbol_lookup), *this, std::tr1::placeholders::_1));
                    set_symbol_size(s.symbol, e.evaluate(s.expression));
                }

                // cf::EntityVisitor
                void visit(const cf::ProgramEnd &) { }

                void visit(const cf::ALUClause &)
                {
                    current_offset += 8; // size of a cf instruction
                }

                void visit(const cf::BranchInstruction &)
                {
                    current_offset += 8; // size of a cf instruction
                }

                void visit(const cf::Label & l)
                {
                    add_symbol(l.text, current_offset);
                }

                void visit(const cf::LoopInstruction &)
                {
                    current_offset += 8; // size of a cf instruction
                }

                void visit(const cf::NopInstruction &)
                {
                    current_offset += 8; // size of a cf instruction
                }

                void visit(const cf::Size & s)
                {
                    ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolStage::symbol_lookup), *this, std::tr1::placeholders::_1));
                    set_symbol_size(s.symbol, e.evaluate(s.expression));
                }

                void visit(const cf::TextureFetchClause &)
                {
                    current_offset += 8; // size of a cf instruction
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
                    ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolStage::symbol_lookup), *this, std::tr1::placeholders::_1));
                    set_symbol_size(s.symbol, e.evaluate(s.expression));
                }
            };
        }

        Sequence<Symbol>
        SymbolScanner::scan(const Sequence<SectionPtr> & sections)
        {
            internal::SymbolStage ss;

            for (Sequence<SectionPtr>::Iterator s(sections.begin()), s_end(sections.end()) ;
                    s != s_end ; ++s)
            {
                (*s)->accept(ss);
            }

            return ss.symbols;
        }
    }
}
