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

#include <elf/relocation_table.hh>
#include <elf/section.hh>
#include <r6xx/cf_entities.hh>
#include <r6xx/cf_generate.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>
#include <cstring>
#include <vector>

#include <elf.h>

#include <iostream>

namespace gpu
{
    namespace r6xx
    {
        namespace cf
        {
            namespace internal
            {
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
                    public r6xx::SectionVisitor,
                    public cf::EntityVisitor
                {
                    elf::Section cf_rel;

                    elf::Section cf_text;

                    std::vector<InstructionData> instructions;

                    InstructionType last_type;

                    elf::RelocationTable reltab;

                    Sequence<r6xx::Symbol> symbols;

                    Generator(const elf::SymbolTable & symtab, const Sequence<r6xx::Symbol> & symbols) :
                        cf_rel(elf::Section::Parameters()
                                .alignment(0x4)
                                .name(".cf.rel")
                                .type(SHT_REL)),
                        cf_text(elf::Section::Parameters()
                                .alignment(0x4)
                                .flags(SHF_ALLOC | SHF_EXECINSTR)
                                .name(".cf")
                                .type(SHT_PROGBITS)),
                        reltab(symtab),
                        symbols(symbols),
                        last_type(it_none)
                    {
                    }

                    unsigned offset_of(const std::string & local_symbol, SectionId sid)
                    {
                        Sequence<r6xx::Symbol>::Iterator i(std::find_if(symbols.begin(), symbols.end(), SymbolComparator(Symbol(local_symbol, 0, sid))));
                        if (symbols.end() == i)
                            throw UnresolvedSymbolError(local_symbol);

                        return i->offset;
                    }

                    // r6xx::SectionVisitor
                    void visit(const alu::Section & c) { }
                    void visit(const tex::Section & t) { }

                    void visit(const cf::Section & a)
                    {
                        for (Sequence<cf::EntityPtr>::Iterator i(a.entities.begin()), i_end(a.entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        unsigned size(sizeof(InstructionData) * instructions.size());
                        cf_text.data().resize(size);
                        cf_text.data().write(0, reinterpret_cast<const char *>(&instructions[0]), size);

                        reltab.write(cf_rel.data());
                    }

                    // cf::EntityVisitor
                    void visit(const cf::Label & l) { }
                    void visit(const cf::Size & s) { }

                    void visit(const cf::ALUClause & a)
                    {
                        // Relocations
                        // FIXME Dummy
                        // TODO Clause relocation
                        // TODO KCache relocations
                        unsigned offset(instructions.size() * sizeof(InstructionData));
                        reltab.append(elf::Relocation(offset, a.clause, cfrel_alu_clause));

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

                    void visit(const cf::LoopInstruction & i)
                    {
                        bool local_branch('.' == i.target[0]);
                        bool needs_cf_const(false);
                        if ((0x4 == i.opcode) || (0x7 == i.opcode))
                            needs_cf_const = true;

                        // Relocations
                        // TODO loop counter relocation if needs_cf_const
                        unsigned offset(instructions.size() * sizeof(InstructionData));
                        if (! local_branch)
                        {
                            reltab.append(elf::Relocation(offset, i.target, cfrel_branch));
                        }
                        if (needs_cf_const)
                        {
                        }

                        // Microcode
                        InstructionData instruction(0);
                        DefaultData * dd(reinterpret_cast<DefaultData *>(&instruction));
                        if (local_branch)
                        {
                            dd->address = offset_of(i.target, sid_cf);
                        }
                        else
                        {
                            dd->address = (1LL << 32) - 1;
                        }
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

            Sequence<elf::Section>
            Generator::generate(const Sequence<r6xx::SectionPtr> & sections, const elf::SymbolTable & symtab,
                    const Sequence<r6xx::Symbol> & symbols)
            {
                Sequence<elf::Section> result;
                internal::Generator g(symtab, symbols);

                for (Sequence<r6xx::SectionPtr>::Iterator s(sections.begin()), s_end(sections.end()) ;
                        s != s_end ; ++s)
                {
                    (*s)->accept(g);
                }

                if (g.cf_text.data().size() > 0)
                    result.append(g.cf_text);

                if (g.cf_rel.data().size() > 0)
                    result.append(g.cf_rel);

                return result;
            }
        }
    }
}
