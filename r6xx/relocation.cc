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

#include <r6xx/alu_entities.hh>
#include <r6xx/alu_section.hh>
#include <r6xx/cf_entities.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/relocation.hh>
#include <r6xx/tex_entities.hh>
#include <r6xx/tex_section.hh>
#include <utils/sequence-impl.hh>

#include <algorithm>

namespace gpu
{
    namespace r6xx
    {
        namespace internal
        {
            struct RelocationStage :
                public SectionVisitor,
                public cf::EntityVisitor,
                public tex::EntityVisitor
            {
                Sequence<Relocation> relocations;

                unsigned current_offsets[4];

                // SectionVisitor
                void visit(const alu::Section &)
                {
                }

                void visit(const cf::Section & c)
                {
                    current_offsets[sid_cf] = 0;

                    for (Sequence<cf::EntityPtr>::Iterator i(c.entities.begin()), i_end(c.entities.end()) ;
                            i != i_end ; ++i)
                    {
                        (*i)->accept(*this);
                    }
                }

                void visit(const tex::Section & t)
                {
                    current_offsets[sid_tex] = 0;

                    for (Sequence<tex::EntityPtr>::Iterator i(t.entities.begin()), i_end(t.entities.end()) ;
                            i != i_end ; ++i)
                    {
                        (*i)->accept(*this);
                    }
                }

                // cf::EntityVisitor
                void visit(const cf::Label &) { }
                void visit(const cf::ProgramEnd &) { }
                void visit(const cf::Size &) { }

                void visit(const cf::ALUClause & a)
                {
                    current_offsets[sid_cf] += 8; // size of a cf instruction

                    relocations.append(Relocation(sid_cf, a.clause, cfrel_alu_clause));
                }

                void visit(const cf::LoopInstruction & i)
                {
                    current_offsets[sid_cf] += 8; // size of a cf instruction

                    relocations.append(Relocation(sid_cf, i.target, cfrel_branch));
                }

                void visit(const cf::NopInstruction &)
                {
                    current_offsets[sid_cf] += 8; // size of a cf instruction
                }

                void visit(const cf::TextureFetchClause & t)
                {
                    current_offsets[sid_cf] += 8; // size of a cf instruction

                    relocations.append(Relocation(sid_cf, t.clause, cfrel_tex_clause));
                }

                // tex::EntityVisitor
                void visit(const tex::Label &) { }
                void visit(const tex::Size &) { }

                void visit(const tex::LoadInstruction &)
                {
                    current_offsets[sid_tex] += 16; // size of a tex instruction
                    // TODO Relocation for resource id
                }
            };
        }

        Sequence<Relocation>
        RelocationScanner::scan(const Sequence<SectionPtr> & sections)
        {
            internal::RelocationStage rs;

            for (Sequence<SectionPtr>::Iterator s(sections.begin()), s_end(sections.end()) ;
                    s != s_end ; ++s)
            {
                (*s)->accept(rs);
            }

            return rs.relocations;
        }
    }
}
