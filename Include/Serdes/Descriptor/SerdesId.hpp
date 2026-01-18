#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief Serdes identifiers

    @details They are constexpr objects of type std::array<uint8_t, N> 
	    that encode the characteristics and properties of serdes and 
		serve as a unique identifier for a specific serdes type.


    @todo

    @author Niraleks
*/
//-----------------------------------------------------------------------------
#include <algorithm>
#include <memory>
#include "../Serdes.hpp"
#include "SerdesId.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    // Forward declarations for composite serdes
    template<CRangeSerdes TRangeSerdes> constexpr const auto &GetSerdesId();
    template<CArraySerdes TArraySerdes> constexpr const auto &GetSerdesId();
    template<CTupleSerdes TTupleSerdes> constexpr const auto &GetSerdesId();
    template<CVariantSerdes TVariantSerdes> constexpr const auto &GetSerdesId();
    template<CConstSerdes TConstSerdes> constexpr const auto &GetSerdesId();
    template<CCustomSerdes TCustomSerdes> constexpr const auto &GetSerdesId();

    namespace details
    {
        template<CSerdes TSerdes, uint16_t offsetValue>
        struct Field
        {
            using SerdesT = TSerdes;

            static constexpr
            uint16_t offset = offsetValue;
        };


        template<CSerdes TSerdes, typename ...TExtraTraits>
        consteval
        auto MakeSerdesId(TExtraTraits ...extraTraits)
        {
            return SerializeToArray(
                    uint16_t{Sizeof(extraTraits...) + 8},  
                    TSerdes::GetTypeId(),                  
                    TSerdes::GetBufferType(),              
                    TSerdes::Sizeof(),                     
                    extraTraits...);                      
        }
    }

    template<CVoidSerdes TVoidSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        constexpr static
        auto id = details::MakeSerdesId<TVoidSerdes>();

        return id;
    };

    template<CPodSerdes TPodSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        constexpr static
        auto id = details::MakeSerdesId<TPodSerdes>(
            TPodSerdes::GetPodId(),
            TPodSerdes::GetEndianness() == std::endian::little ? true  : false);
        return id;
    };

    template<CRangeSerdes TRangeSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        constexpr static
        auto id = details::MakeSerdesId<TRangeSerdes>(
            TRangeSerdes::sizelen,
            GetSerdesId<typename TRangeSerdes::ElementSerdes>());
        return id;
    };

    template<CArraySerdes TArraySerdes>
    constexpr
    const auto &GetSerdesId()
    {
        constexpr static
        auto id = details::MakeSerdesId<TArraySerdes>(
            TArraySerdes::arraySize,
            GetSerdesId<typename TArraySerdes::ElementSerdes>());
        return id;
    };

    template<CTupleSerdes TTupleSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        using SerdesList = typename TTupleSerdes::SerdesList;

        static constexpr
        auto id = details::MakeSerdesId<TTupleSerdes>(
            TTupleSerdes::CountElements(),
            []<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return SerializeToArray(GetSerdesId<std::tuple_element_t<Is, SerdesList>>()...);
            }(std::make_index_sequence<std::tuple_size_v<SerdesList>>{}));

        return id;
    }

    template<CVariantSerdes TVariantSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        using SerdesList = typename TVariantSerdes::SerdesList;

        static constexpr
        auto id = details::MakeSerdesId<TVariantSerdes>(
            TVariantSerdes::CountTypes(),
            []<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return SerializeToArray(GetSerdesId<std::tuple_element_t<Is, SerdesList>>()...);
            }(std::make_index_sequence<std::tuple_size_v<SerdesList>>{}));

        return id;
    }

    template<CConstSerdes TConstSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        using SerdesType = typename TConstSerdes::SerdesType;

        constexpr static
        auto id = details::MakeSerdesId<TConstSerdes>(
            GetSerdesId<SerdesType>(),
            SerializeToArray<SerdesType>(TConstSerdes::ValueType::Value));

        return id;
    };

	// Custom serdes are transparent for identification — the base serdes identifier is used
    template<CCustomSerdes TCustomSerdes>
    constexpr
    const auto &GetSerdesId()
    {
        constexpr static
        auto id = details::MakeSerdesId<typename TCustomSerdes::BaseSerdes>();

        return id;
    };

} // serdes

//------------------------------------------------------------------------------
