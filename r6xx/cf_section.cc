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

#include <common/assembly_entities.hh>
#include <common/expression.hh>
#include <elf/relocation_table.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/relocation.hh>
#include <utils/sequence-impl.hh>
#include <utils/stringify.hh>

#include <algorithm>
#include <vector>

#include <elf.h>

namespace gpu
{
    template <>
    ConstVisits<r6xx::cf::Section>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace cf
        {
            namespace internal
            {
                struct SymbolScanner :
                    public cf::EntityVisitor
                {
                    Sequence<elf::Symbol> symbols;

                    unsigned current_offset;

                    SymbolScanner(const Sequence<cf::EntityPtr> & cf_entities) :
                        current_offset(0)
                    {
                        add_symbol(".cf", 0, STT_SECTION);

                        for (Sequence<cf::EntityPtr>::Iterator i(cf_entities.begin()), i_end(cf_entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        set_symbol_size(".cf", current_offset);
                    }

                    void add_undefined_symbol(const std::string & name, unsigned type)
                    {
                        elf::Symbol symbol(name);
                        symbol.type = type;

                        if (symbols.end() != std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)))
                            throw DuplicateSymbolError(name);

                        symbols.append(symbol);
                    }

                    void add_symbol(const std::string & name, unsigned offset, unsigned type = 0)
                    {
                        elf::Symbol symbol(name);
                        symbol.section = ".cf";
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

                    void visit(const cf::LoopInstruction & l)
                    {
                        current_offset += 8; // size of a cf instruction

                        if (! l.counter.empty())
                        {
                            add_undefined_symbol(l.counter, STT_OBJECT);
                        }
                    }

                    void visit(const cf::NopInstruction &)
                    {
                        current_offset += 8; // size of a cf instruction
                    }

                    void visit(const cf::Size & s)
                    {
                        ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolScanner::symbol_lookup), *this, std::tr1::placeholders::_1));
                        set_symbol_size(s.symbol, e.evaluate(s.expression));
                    }

                    void visit(const cf::Type & t)
                    {
                        set_symbol_type(t.symbol, t.type);
                    }

                    void visit(const cf::TextureFetchClause &)
                    {
                        current_offset += 8; // size of a cf instruction
                    }
                };

                typedef uint64_t InstructionData;

                enum InstructionType
                {
                    it_none,
                    it_default,
                    it_alu
                };

                struct DefaultData
                {
                    /* CF_DWORD0 */
                    unsigned address:32;

                    /* CF_DWORD1 */
                    unsigned pop_count:3;
                    unsigned cf_const:5;
                    unsigned cond:2;
                    unsigned count:3;
                    unsigned call_count:6;
                    unsigned reserved:2;
                    unsigned end_of_program:1;
                    unsigned valid_pixel_mode:1;
                    unsigned opcode:7;
                    unsigned whole_quad_mode:1;
                    unsigned barrier:1;
                } __attribute__((packed));

                struct ALUClauseData
                {
                    /* CF_ALU_DWORD0 */
                    unsigned address:22;
                    unsigned kcache_bank0:4;
                    unsigned kcache_bank1:4;
                    unsigned kcache_mode0:2;

                    /* CF_ALU_DWORD1 */
                    unsigned kcache_mode1:2;
                    unsigned kcache_address0:8;
                    unsigned kcache_address1:8;
                    unsigned count:7;
                    unsigned use_waterfall:1;
                    unsigned opcode:4;
                    unsigned whole_quad_mode:1;
                    unsigned barrier:1;
                } __attribute__((packed));

                struct Generator :
                    public cf::EntityVisitor
                {
                    elf::Section cf_rel;

                    elf::Section cf_text;

                    std::vector<InstructionData> instructions;

                    InstructionType last_type;

                    elf::RelocationTable reltab;

                    Sequence<elf::Symbol> symbols;

                    Generator(const Sequence<cf::EntityPtr> & cf_entities, const elf::SymbolTable & symtab, const Sequence<elf::Symbol> & symbols) :
                        cf_rel(elf::Section::Parameters()
                                .alignment(0x8)
                                .name(".cf.rel")
                                .type(SHT_RELA)),
                        cf_text(elf::Section::Parameters()
                                .alignment(0x8)
                                .flags(SHF_ALLOC | SHF_EXECINSTR)
                                .name(".cf")
                                .type(SHT_PROGBITS)),
                        last_type(it_none),
                        reltab(symtab),
                        symbols(symbols)
                    {
                        for (Sequence<cf::EntityPtr>::Iterator i(cf_entities.begin()), i_end(cf_entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        unsigned size(sizeof(InstructionData) * instructions.size());
                        cf_text.data().resize(size);
                        cf_text.data().write(0, reinterpret_cast<const char *>(&instructions[0]), size);

                        reltab.write(cf_rel.data());
                    }

                    unsigned offset_of(const std::string & local_symbol, const std::string & section)
                    {
                        Sequence<elf::Symbol>::Iterator i(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(local_symbol)));
                        if (symbols.end() == i)
                            throw UnresolvedSymbolError(local_symbol);

                        if (section != i->section)
                            throw InternalError("r6xx", "local symbol '" + local_symbol + "' not in section '" + section + "' as expected");

                        return i->value;
                    }

                    std::string find_symbol_before(const std::string & symbol, const std::string & section)
                    {
                        Sequence<elf::Symbol>::Iterator result(symbols.begin());

                        for (Sequence<elf::Symbol>::Iterator s(symbols.begin()), s_end(symbols.end()) ;
                                s != s_end ; ++s)
                        {
                            if (section != s->section)
                                continue;

                            if (".L" != s->name.substr(0, 2))
                                result = s;

                            if (symbol == s->name)
                                return result->name;
                        }

                        throw InternalError("r6xx", "Did not find symbol before '" + symbol + "'");
                    }

                    // cf::EntityVisitor
                    void visit(const cf::Label &) { }
                    void visit(const cf::Size &) { }
                    void visit(const cf::Type &) { }

                    void visit(const cf::ALUClause & a)
                    {
                        // Relocations
                        // TODO KCache relocations
                        unsigned offset(instructions.size() * sizeof(InstructionData));
                        reltab.append(elf::Relocation(offset, a.clause, cfrel_alu_clause, 0));

                        // Microcode
                        InstructionData instruction(~0LL);
                        ALUClauseData * ad(reinterpret_cast<ALUClauseData *>(&instruction));
                        ad->use_waterfall = 0;
                        ad->opcode = a.opcode;
                        ad->whole_quad_mode = 0;
                        ad->barrier = 0;

                        instructions.push_back(instruction);
                        last_type = it_alu;
                    }

                    void visit(const cf::BranchInstruction & b)
                    {
                        bool local_branch(".L" == b.target.substr(0, 2));

                        // Relocations
                        unsigned offset(instructions.size() * sizeof(InstructionData));
                        if (local_branch)
                        {
                            std::string symbol(find_symbol_before(b.target, ".cf"));
                            unsigned addend(offset_of(b.target, ".cf") - offset_of(symbol, ".cf"));

                            reltab.append(elf::Relocation(offset, symbol, cfrel_pic, addend));
                        }
                        else
                        {
                            reltab.append(elf::Relocation(offset, b.target, cfrel_branch, 0));
                        }

                        // Microcode
                        InstructionData instruction(0);
                        DefaultData * dd(reinterpret_cast<DefaultData *>(&instruction));
                        dd->address = (1LL << 32) - 1;
                        dd->opcode = b.opcode;
                        do
                        {
                            switch (b.opcode)
                            {
                                case 0x0d: /* call */
                                    dd->call_count = b.count;
                                    continue;

                                case 0x0a: /* push */
                                case 0x0c: /* pop */
                                case 0x10: /* jump */
                                case 0x11: /* else */
                                    dd->pop_count = b.count;
                                    continue;

                                case 0x0b: /* push_else */
                                case 0x0e: /* return */
                                    continue;
                            }

                            throw InternalError("r6xx", "unhandled opcode '" + stringify(b.opcode) + "' for BranchInstruction");
                        }
                        while (false);

                        instructions.push_back(instruction);
                        last_type = it_default;
                    }

                    void visit(const cf::LoopInstruction & i)
                    {
                        bool local_branch(".L" == i.target.substr(0, 2));
                        bool needs_cf_const(false);
                        if ((0x4 == i.opcode) || (0x7 == i.opcode))
                            needs_cf_const = true;

                        // Relocations
                        // TODO loop counter relocation if needs_cf_const
                        unsigned offset(instructions.size() * sizeof(InstructionData));
                        if (local_branch)
                        {
                            std::string symbol(find_symbol_before(i.target, ".cf"));
                            unsigned addend(offset_of(i.target, ".cf") - offset_of(symbol, ".cf"));

                            reltab.append(elf::Relocation(offset, symbol, cfrel_pic, addend));
                        }
                        else
                        {
                            reltab.append(elf::Relocation(offset, i.target, cfrel_branch, 0));
                        }
                        if (needs_cf_const)
                        {
                        }

                        // Microcode
                        InstructionData instruction(0);
                        DefaultData * dd(reinterpret_cast<DefaultData *>(&instruction));
                        dd->address = (1LL << 32) - 1;
                        dd->cf_const = needs_cf_const ? (1 << 5) - 1 : 0;
                        dd->opcode = i.opcode;

                        instructions.push_back(instruction);
                        last_type = it_default;
                    }

                    void visit(const cf::NopInstruction &)
                    {
                        instructions.push_back(InstructionData(0));
                        last_type = it_default;
                    }

                    void visit(const cf::ProgramEnd &)
                    {
                        // ALU clause instructions cannot be the last instruction in a program
                        // Push a nop instruction in that case
                        if (it_default != last_type)                        {
                            instructions.push_back(InstructionData(0));
                            last_type = it_default;
                        }

                        instructions.back() |= (1 << 21);
                    }

                    void visit(const cf::TextureFetchClause &)
                    {
                        // Relocations
                        // TODO Count + Address relocation
                        InstructionData instruction(0);

                        instructions.push_back(instruction);
                        last_type = it_default;
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
                return ".cf";
            }

            void
            Section::accept(SectionVisitor & v) const
            {
                static_cast<ConstVisits<r6xx::cf::Section> *>(&v)->visit(*this);
            }

            Sequence<elf::Symbol>
            Section::symbols() const
            {
                internal::SymbolScanner ss(entities);

                return ss.symbols;
            }

            Sequence<elf::Section>
            Section::sections(const elf::SymbolTable & symtab, const Sequence<elf::Symbol> & symbols) const
            {
                Sequence<elf::Section> result;
                internal::Generator g(entities, symtab, symbols);

                if (g.cf_text.data().size() > 0)
                    result.append(g.cf_text);

                if (g.cf_rel.data().size() > 0)
                    result.append(g.cf_rel);

                return result;
            }

        }
    }
}
