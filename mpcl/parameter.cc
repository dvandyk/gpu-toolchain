/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <mpcl/parameter.hh>
#include <utils/sequence-impl.hh>

namespace gpu
{
    template class Sequence<ParameterPtr>;

    template class WrappedForwardIterator<Sequence<ParameterPtr>::IteratorTag, ParameterPtr>;
}
