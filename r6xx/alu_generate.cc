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

#include <elf/section.hh>
#include <r6xx/alu_entities.hh>
#include <r6xx/alu_generate.hh>
#include <r6xx/alu_section.hh>
#include <r6xx/section.hh>
#include <utils/sequence-impl.hh>

#include <cstring>
#include <vector>

#include <elf.h>

namespace gpu
{
    namespace r6xx
    {
        namespace alu
        {
            namespace internal
            {
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
                    public r6xx::SectionVisitor,
                    public alu::EntityVisitor
                {
                    elf::Section alu_section;

                    // ALU entity visitation
                    Enumeration<3> index_mode;

                    std::vector<InstructionData> instructions;

                    Generator() :
                        alu_section(elf::Section::Parameters()
                                .alignment(0x4)
                                .flags(SHF_ALLOC | SHF_EXECINSTR)
                                .name(".alu")
                                .type(SHT_PROGBITS)),
                        index_mode(4)
                    {
                    }

                    // r6xx::SectionVisitor
                    void visit(const cf::Section &) { }
                    void visit(const tex::Section &) { }

                    void visit(const alu::Section & a)
                    {
                        for (Sequence<alu::EntityPtr>::Iterator i(a.entities.begin()), i_end(a.entities.end()) ;
                                i != i_end ; ++i)
                        {
                            (*i)->accept(*this);
                        }

                        unsigned size(sizeof(InstructionData) * instructions.size());
                        alu_section.data().resize(size);
                        alu_section.data().write(0, reinterpret_cast<const char *>(&instructions[0]), size);
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

            elf::Section
            Generator::generate(const Sequence<r6xx::SectionPtr> & sections, const Sequence<r6xx::Symbol> &)
            {
                internal::Generator g;

                for (Sequence<r6xx::SectionPtr>::Iterator s(sections.begin()), s_end(sections.end()) ;
                        s != s_end ; ++s)
                {
                    (*s)->accept(g);
                }

                return g.alu_section;
            }
        }
    }
}
