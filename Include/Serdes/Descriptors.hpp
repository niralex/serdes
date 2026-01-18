#pragma once
//------------------------------------------------------------------------------
/** @file

	@brief Subsystem of serdes identifiers and descriptors

	@details


    @todo


    @author Niraleks

*/

//------------------------------------------------------------------------------
#include <Serdes/Serdes.hpp>
#include <Serdes/Descriptor/SerdesId.hpp>
#include <Serdes/Descriptor/Descriptor.hpp>
#include <Serdes/Descriptor/PodDescriptor.hpp>
#include <Serdes/Descriptor/RangeDescriptor.hpp>
#include <Serdes/Descriptor/ArrayDescriptor.hpp>
#include <Serdes/Descriptor/TupleDescriptor.hpp>
#include <Serdes/Descriptor/VariantDescriptor.hpp>
#include <Serdes/Descriptor/ConstDescriptor.hpp>

//-----------------------------------------------------------------------------
namespace serdes
{
    template<CSerdes ...TSerdes>
    consteval
    auto GetDescriptor()
    {
        using BaseSerdes = SerdesT<TSerdes...>;

        constexpr const
        uint8_t *serdesId = GetSerdesId<BaseSerdes>().begin();

        constexpr
        TypeId typeId = Descriptor(serdesId).GetTypeId();

        if constexpr (typeId == TypeId::Void)
            return Descriptor(serdesId);

        else if constexpr (typeId == TypeId::Pod)
            return PodDescriptor(serdesId);

        else if constexpr (typeId == TypeId::Range)
            return RangeDescriptor(serdesId);

        else if constexpr (typeId == TypeId::Array)
            return ArrayDescriptor(serdesId);

        else if constexpr (typeId == TypeId::Tuple)
            return TupleDescriptor(serdesId);

        else if constexpr (typeId == TypeId::Variant)
            return VariantDescriptor(serdesId);

        else if constexpr (typeId == TypeId::Const)
            return ConstDescriptor(serdesId);

        else
            static_assert(false, "Unknown descriptor type");
    }
}

//------------------------------------------------------------------------------

