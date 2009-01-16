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
#include <r6xx/alu_section.hh>
#include <r6xx/error.hh>

#include <algorithm>
#include <vector>

#include <elf.h>

namespace gpu
{
    template <>
    ConstVisits<r6xx::alu::Section>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace alu
        {
            namespace internal
            {
                struct SymbolScanner :
                    public alu::EntityVisitor
                {
                    Sequence<elf::Symbol> symbols;

                    unsigned current_offset;

                    SymbolScanner(const Sequence<alu::EntityPtr> & alu_entities) :
                        current_offset(0)
                    {
                        add_symbol(".alu", 0, STT_SECTION);

                        for (Sequence<alu::EntityPtr>::Iterator i(alu_entities.begin()), i_end(alu_entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        set_symbol_size(".alu", current_offset);
                    }

                    void add_symbol(const std::string & name, unsigned offset, unsigned type = 0)
                    {
                        elf::Symbol symbol(name);
                        symbol.section = ".alu";
                        symbol.type = type;
                        symbol.value = offset;

                        if (symbols.end() != std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)))
                            throw DuplicateSymbolError(name);

                        symbols.append(symbol);
                    }

                    void set_symbol_size(const std::string & name, unsigned size)
                    {
                        elf::Symbol symbol(name);
                        symbol.section = ".alu";

                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        s->size = size;
                    }

                    void set_symbol_type(const std::string & name, unsigned type)
                    {
                        elf::Symbol symbol(name);
                        symbol.section = ".alu";

                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        s->type = type;
                    }

                    unsigned symbol_lookup(const std::string & name)
                    {
                        if ("." == name)
                            return current_offset;

                        elf::Symbol symbol(name);
                        symbol.section = ".alu";

                        Sequence<elf::Symbol>::Iterator s(std::find_if(symbols.begin(), symbols.end(), elf::SymbolByName(name)));
                        if (symbols.end() == s)
                            throw UnresolvedSymbolError(name);

                        return s->value;
                    }


                    // alu::EntityVisitor
                    void visit(const alu::GroupEnd &) { }
                    void visit(const alu::IndexMode &) { }

                    void visit(const alu::Form2Instruction &)
                    {
                        // TODO literals
                        current_offset += 8; // size of an alu instruction
                    }

                    void visit(const alu::Form3Instruction &)
                    {
                        // TODO literals
                        current_offset += 8; // size of an alu instruction
                    }

                    void visit(const alu::Label & l)
                    {
                        add_symbol(l.text, current_offset);
                    }

                    void visit(const alu::Size & s)
                    {
                        ExpressionEvaluator e(std::tr1::bind(std::tr1::mem_fn(&SymbolScanner::symbol_lookup), *this, std::tr1::placeholders::_1));
                        set_symbol_size(s.symbol, e.evaluate(s.expression));
                    }

                    void visit(const alu::Type & t)
                    {
                        set_symbol_type(t.symbol, t.type);
                    }
                };

                typedef uint64_t InstructionData;

                struct SourceOperandData
                {
                    bool absolute;
                    unsigned channel;
                    bool negated;
                    bool relative;
                    unsigned selector;
                };

                struct SourceOperandGenerator :
                    public alu::SourceOperandVisitor
                {
                    SourceOperandData * data;

                    bool needs_literal;

                    unsigned literal_data;

                    SourceOperandGenerator(SourceOperandData * data) :
                        data(data)
                    {
                    }

                    // alu::SourceOperandVisitor
                    void visit(const alu::SourceGPR & g)
                    {
                        needs_literal = false;

                        data->absolute = false;
                        data->channel = g.channel;
                        data->negated = g.negated;
                        data->relative = g.relative;
                        data->selector = g.index;
                    }

                    void visit(const alu::SourceKCache & k)
                    {
                        needs_literal = false;

                        data->absolute = false;
                        data->channel = k.channel;
                        data->negated = k.negated;
                        data->relative = k.relative;
                        data->selector = k.index + 128;
                    }

                    void visit(const alu::SourceCFile & c)
                    {
                        needs_literal = false;

                        data->absolute = false;
                        data->channel = c.channel;
                        data->negated = c.negated;
                        data->relative = c.relative;
                        data->selector = c.index + 256;
                    }

                    void visit(const alu::SourceLiteral & l)
                    {
                        needs_literal = false;
                        literal_data = l.data;

                        data->absolute = false;
                        data->channel = 0;
                        data->negated = false;
                        data->relative = false;
                        data->selector = 253;
                    }
                };

                struct Form2Data
                {
                    /* ALU_DWORD0 */
                    unsigned src0_sel:9;
                    unsigned src0_rel:1;
                    unsigned src0_chan:2;
                    unsigned src0_neg:1;

                    unsigned src1_sel:9;
                    unsigned src1_rel:1;
                    unsigned src1_chan:2;
                    unsigned src1_neg:1;

                    unsigned index_mode:3;

                    unsigned pred_sel:2;

                    unsigned last:1;

                    /* ALU_DWORD1_OP2 */
                    unsigned src0_abs:1;
                    unsigned src1_abs:1;

                    unsigned update_execute_mask:1;
                    unsigned update_predicate:1;

                    unsigned write_mask:1;

                    unsigned fog_merge:1;

                    unsigned output_modifier:2;

                    unsigned opcode:10;

                    unsigned bank_swizzle:3;

                    unsigned dst_gpr:7;
                    unsigned dst_rel:1;
                    unsigned dst_chan:2;

                    unsigned clamp:1;
                } __attribute__((packed));

                struct Form3Data
                {
                    /* ALU_DWORD0 */
                    unsigned src0_sel:9;
                    unsigned src0_rel:1;
                    unsigned src0_chan:2;
                    unsigned src0_neg:1;

                    unsigned src1_sel:9;
                    unsigned src1_rel:1;
                    unsigned src1_chan:2;
                    unsigned src1_neg:1;

                    unsigned index_mode:3;

                    unsigned pred_sel:2;

                    unsigned last:1;

                    /* ALU_DWORD1_OP3 */
                    unsigned src2_sel:9;
                    unsigned src2_rel:1;
                    unsigned src2_chan:2;
                    unsigned src2_neg:1;

                    unsigned opcode:5;

                    unsigned bank_swizzle:3;

                    unsigned dst_gpr:7;
                    unsigned dst_rel:1;
                    unsigned dst_chan:2;

                    unsigned clamp:1;
                } __attribute__((packed));

                struct Generator :
                    public alu::EntityVisitor
                {
                    elf::Section alu_section;

                    Sequence<elf::Section> sections;

                    // ALU entity visitation
                    Enumeration<3> index_mode;

                    std::vector<InstructionData> instructions;

                    Generator(const Sequence<alu::EntityPtr> & alu_entities) :
                        alu_section(elf::Section::Parameters()
                                .alignment(0x8)
                                .flags(SHF_ALLOC | SHF_EXECINSTR)
                                .name(".alu")
                                .type(SHT_PROGBITS)),
                        index_mode(4)
                    {
                        for (Sequence<alu::EntityPtr>::Iterator i(alu_entities.begin()), i_end(alu_entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        unsigned size(sizeof(InstructionData) * instructions.size());
                        alu_section.data().resize(size);
                        alu_section.data().write(0, reinterpret_cast<const char *>(&instructions[0]), size);

                        sections.append(alu_section);
                    }

                    // alu::EntityVisitor
                    void visit(const alu::Label &) { }
                    void visit(const alu::Size &) { }
                    void visit(const alu::Type &) { }

                    void visit(const alu::Form2Instruction & i)
                    {
                        InstructionData instruction(0);
                        Form2Data * form2(reinterpret_cast<Form2Data *>(&instruction));

                        SourceOperandData sources[2];
                        SourceOperandData * j(sources);
                        for (Sequence<alu::SourceOperandPtr>::Iterator k(i.sources.begin()), k_end(i.sources.end()) ;
                                k != k_end ; ++j, ++k)
                        {
                            SourceOperandGenerator g(j);

                            (*k)->accept(g);
                        }

                        form2->src0_abs = 0;
                        form2->src0_chan = sources[0].channel;
                        form2->src0_neg = sources[0].negated;
                        form2->src0_rel = sources[0].relative;
                        form2->src0_sel = sources[0].selector;

                        form2->src1_abs = 0;
                        form2->src1_chan = sources[1].channel;
                        form2->src1_neg = sources[1].negated;
                        form2->src1_rel = sources[1].relative;
                        form2->src1_sel = sources[1].selector;

                        form2->index_mode = index_mode;
                        form2->write_mask = 1;
                        form2->opcode = i.opcode;
                        form2->dst_gpr = i.destination.index;
                        form2->dst_chan = i.destination.channel;
                        form2->dst_rel = i.destination.relative ? 1 : 0;

                        instructions.push_back(instruction);
                    }

                    void visit(const alu::Form3Instruction & i)
                    {
                        InstructionData instruction(0);
                        Form3Data * form3(reinterpret_cast<Form3Data *>(&instruction));

                        SourceOperandData sources[3];
                        SourceOperandData * j(sources);
                        for (Sequence<alu::SourceOperandPtr>::Iterator k(i.sources.begin()), k_end(i.sources.end()) ;
                                k != k_end ; ++j, ++k)
                        {
                            SourceOperandGenerator g(j);

                            (*k)->accept(g);

                            if (j->absolute)
                                throw InternalError("r6xx", "Cannot use absolute values in Form3 instructions");
                        }

                        form3->src0_chan = sources[0].channel;
                        form3->src0_neg = sources[0].negated;
                        form3->src0_rel = sources[0].relative;
                        form3->src0_sel = sources[0].selector;

                        form3->src1_chan = sources[1].channel;
                        form3->src1_neg = sources[1].negated;
                        form3->src1_rel = sources[1].relative;
                        form3->src1_sel = sources[1].selector;

                        form3->src2_chan = sources[2].channel;
                        form3->src2_neg = sources[2].negated;
                        form3->src2_rel = sources[2].relative;
                        form3->src2_sel = sources[2].selector;

                        form3->index_mode = index_mode;
                        form3->opcode = i.opcode;
                        form3->dst_gpr = i.destination.index;
                        form3->dst_chan = i.destination.channel;
                        form3->dst_rel = i.destination.relative ? 1 : 0;

                        instructions.push_back(instruction);
                    }

                    void visit(const alu::GroupEnd &)
                    {
                        reinterpret_cast<Form2Data *>(&instructions.back())->last = 1;
                    }

                    void visit(const alu::IndexMode & i)
                    {
                        index_mode = i.mode;
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
                return ".alu";
            }

            void
            Section::accept(SectionVisitor & v) const
            {
                static_cast<ConstVisits<r6xx::alu::Section> *>(&v)->visit(*this);
            }

            Sequence<elf::Section>
            Section::sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const
            {
                internal::Generator g(entities);

                return g.sections;
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
