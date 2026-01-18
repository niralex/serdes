#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief Serdes descriptor of type Variant

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

    class VariantDescriptor : public Descriptor
    {
        private:
            using FCountTypes = details::Field<UInt16, 8>;

            constexpr static inline
            uint16_t serdesListOffset = 10;

        public:
            constexpr inline
            VariantDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Variant)
                    utils::Throw<std::invalid_argument>("Invalid Variant-serdes identifier");
            }

            [[nodiscard]] constexpr inline
            uint16_t CountTypes()const
            {
                return GetValue<FCountTypes>();
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


