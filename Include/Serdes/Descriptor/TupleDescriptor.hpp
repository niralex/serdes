#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes descriptor of type Tuple

    @details


    @todo

    @author Niraleks
*/
//-----------------------------------------------------------------------------
#include <Serdes/Serdes.hpp>
#include "Descriptor.hpp"
#include "DescriptorIterator.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    class TupleDescriptor : public Descriptor
    {
        private:
            using FCountElements = details::Field<UInt16, 8>;

            constexpr static inline
            uint16_t serdesListOffset = 10;

        public:
            constexpr inline
            TupleDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Tuple)
                    utils::Throw<std::invalid_argument>("Invalid Tuple-serdes identifier");
            }

            [[nodiscard]] constexpr inline
            uint16_t CountElements()const
            {
                return GetValue<FCountElements>();
            }

            [[nodiscard]] constexpr
            DescriptorIterator begin()const
            {
                return _id + serdesListOffset;
            }

            [[nodiscard]] constexpr
            DescriptorIterator end()const
            {
                return _id + GetSize();
            }
    };

} // serdes

//------------------------------------------------------------------------------


