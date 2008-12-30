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

#ifndef GPU_GUARD_R6XX_ALU_SOURCE_OPERAND_HH
#define GPU_GUARD_R6XX_ALU_SOURCE_OPERAND_HH 1

#include <r6xx/alu_source_operand-fwd.hh>
#include <utils/enumeration.hh>
#include <utils/visitor.hh>

namespace gpu
{
    namespace r6xx
    {
        namespace alu
        {
            typedef gpu::ConstVisitorTag<r6xx::alu::SourceGPR, r6xx::alu::SourceKCache, r6xx::alu::SourceCFile, r6xx::alu::SourceLiteral> SourceOperands;

            typedef ConstVisitor<SourceOperands> SourceOperandVisitor;

            struct SourceOperand :
                public ConstVisitable<SourceOperands>
            {
                virtual ~SourceOperand() = 0;
            };

            struct SourceGPR :
                SourceOperand
            {
                Enumeration<2> channel;

                Enumeration<7> index;

                bool negated;

                bool relative;

                SourceGPR(const Enumeration<2> & channel, const Enumeration<7> & index, bool negated, bool relative);

                ~SourceGPR();

                void accept(SourceOperandVisitor & v) const;
            };

            struct SourceKCache :
                SourceOperand
            {
                Enumeration<2> channel;

                Enumeration<6> index;

                bool negated;

                bool relative;

                SourceKCache(const Enumeration<2> & channel, const Enumeration<6> & index, bool negated, bool relative);

                ~SourceKCache();

                void accept(SourceOperandVisitor & v) const;
            };

            struct SourceCFile :
                SourceOperand
            {
                Enumeration<2> channel;

                Enumeration<8> index;

                bool negated;

                bool relative;

                SourceCFile(const Enumeration<2> & channel, const Enumeration<8> & index, bool negated, bool relative);

                ~SourceCFile();

                void accept(SourceOperandVisitor & v) const;
            };

            struct SourceLiteral :
                SourceOperand
            {
                Enumeration<32> data;

                SourceLiteral(const Enumeration<32> & d);

                ~SourceLiteral();

                void accept(SourceOperandVisitor & v) const;
            };

            typedef std::tr1::shared_ptr<SourceOperand> SourceOperandPtr;

            struct SourceOperandParser
            {
                static SourceOperandPtr parse(const std::string & operand);
            };

            class SourceOperandPrinter :
                public SourceOperandVisitor
            {
                private:
                    std::string _output;

                public:
                    void visit(const SourceGPR &);

                    void visit(const SourceKCache &);

                    void visit(const SourceCFile &);

                    void visit(const SourceLiteral &);

                    std::string print(const SourceOperandPtr & operand);
            };
        }
    }
}

#endif
