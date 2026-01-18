#pragma once

//------------------------------------------------------------------------------
/** @file

    @brief  Serdes descriptor of type TypeId::Array

    @details

    @todo

    @author Niraleks
*/
//-----------------------------------------------------------------------------
#include <Serdes/Serdes.hpp>
#include "Descriptor.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    class ArrayDescriptor : public Descriptor
    {
        private:
            using FArraySize = details::Field<UInt32, 8>;

            constexpr static inline
            uint16_t elementDescriptorOffset = 12;

        public:

            constexpr inline
            ArrayDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Array)
                    utils::Throw<std::invalid_argument>("Invalid Array-serdes identifier");
            }

            /// Array size
            [[nodiscard]] constexpr inline
            uint32_t GetArraySize()const
            {
                return GetValue<FArraySize>();
            }

            /// Array element descriptor
            [[nodiscard]] constexpr inline
            Descriptor GetElementDescriptor()const
            {
                return _id + elementDescriptorOffset;
            }
    };

} // serdes

//------------------------------------------------------------------------------


