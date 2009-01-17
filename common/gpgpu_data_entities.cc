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
#include <common/gpgpu_data_entities.hh>
#include <common/syntax.hh>
#include <utils/sequence-impl.hh>
#include <utils/visitor-impl.hh>

namespace gpu
{
    template <>
    ConstVisits<common::Buffer>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<common::Counter>::~ConstVisits()
    {
    }

    namespace common
    {
        namespace internal
        {
            struct DataEntityConverter :
                public AssemblyEntityVisitor
            {
                DataEntityPtr result;

                DataEntityConverter(const AssemblyEntityPtr & entity)
                {
                    entity->accept(*this);
                }

                // AssemblyEntityVisitor
                void visit(const Comment &) { }
                void visit(const Label &) { }

                void visit(const Data &)
                {
                    throw CommonSyntaxError("Unexpted directive '.data' in section '.gpgpu.data'");
                }

                void visit(const Directive & d)
                {
                    if ("buffer" == d.name)
                    {
                        result = DataEntityPtr(new Buffer(d.params));
                    }
                    else if ("counter" == d.name)
                    {
                        result = DataEntityPtr(new Counter(d.params));
                    }
                    else
                    {
                        throw CommonSyntaxError("Unexpected directive '." + d.name + "' in section '.gpgpu.data'");
                    }
                }

                void visit(const Instruction & i)
                {
                    throw CommonSyntaxError("Unexpected instruction '" + i.mnemonic + "' in section '.gpgpu.data'");
                }

                void visit(const Line & l)
                {
                    SyntaxContext::Line(l.number);
                }
            };
        }

        DataEntity::~DataEntity()
        {
        }

        Buffer::Buffer(const std::string & name) :
            name(name)
        {
        }

        Buffer::~Buffer()
        {
        }

        void
        Buffer::accept(DataEntityVisitor & v) const
        {
            static_cast<ConstVisits<Buffer> *>(&v)->visit(*this);
        }

        Counter::Counter(const std::string & name) :
            name(name)
        {
        }

        Counter::~Counter()
        {
        }

        void
        Counter::accept(DataEntityVisitor & v) const
        {
            static_cast<ConstVisits<Counter> *>(&v)->visit(*this);
        }

        DataEntityPtr
        DataEntityConverter::convert(const AssemblyEntityPtr & entity)
        {
            internal::DataEntityConverter c(entity);

            return c.result;
        }
    }
}
