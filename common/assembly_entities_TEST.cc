/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Danny van Dyk <danny.dyk@tu-dortmund.de>
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

#include <tests/tests.hh>
#include <common/assembly_entities.hh>
#include <utils/memory.hh>
#include <utils/sequence-impl.hh>

#include <sstream>

using namespace gpu;
using namespace tests;

struct TestVisitor :
    public AssemblyEntityVisitor
{
    std::stringstream s;

    void visit(const Comment & c)
    {
        s << "Comment '" << c.text << "'" << std::endl;
    }

    void visit(const Directive & d)
    {
        s << "Directive '" << d.name << "' '" << d.params << "'" << std::endl;
    }

    void visit(const Instruction & i)
    {
        s << "Instruction '" << i.mnemonic << "'" << std::endl;

        for (Sequence<std::string>::Iterator j(i.operands.begin()), j_end(i.operands.end()) ; j != j_end ; ++j)
        {
            s << "  Operand '" << *j << "'" << std::endl;
        }
    }
};

struct AssemblyEntitiesTest :
    public Test
{
    AssemblyEntitiesTest() :
        Test("assembly_entities_test")
    {
    }

    virtual void run()
    {
        const std::string reference(
                "Directive 'file' 'stdin'\n"
                "Comment 'tuuuraluuraluu'\n"
                "Directive 'section' 'text'\n"
                "Label 'main'\n"
                "Instruction 'nop'\n"
                "Instruction 'add'\n"
                "  Operand '$1'\n"
                "  Operand '$1'\n"
                "  Operand '0x1234'\n");

        TestVisitor v;
        Sequence<std::tr1::shared_ptr<AssemblyEntity> > entities;

        entities.append(make_shared_ptr(new Directive("file", "stdin")));
        entities.append(make_shared_ptr(new Comment("tuuuraluuraluu")));
        entities.append(make_shared_ptr(new Directive("section", "text")));
        entities.append(make_shared_ptr(new Label("main")));
        entities.append(make_shared_ptr(new Instruction("nop")));

        Instruction * instruction(new Instruction("add"));
        instruction->operands.append("$1");
        instruction->operands.append("$1");
        instruction->operands.append("0x1234");

        entities.append(make_shared_ptr(instruction));

        for (Sequence<std::tr1::shared_ptr<AssemblyEntity> >::Iterator i(entities.begin()), i_end(entities.end()) ;
                i != i_end ; ++i)
        {
            (*i)->accept(v);
        }

        TEST_CHECK_EQUAL(reference, v.s.str());

    }
} assembly_entities_test;

