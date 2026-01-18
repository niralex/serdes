#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes descriptor of type POD

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

    class PodDescriptor : public Descriptor
    {
        private:
            using FPodId = details::Field<UInt8, 8>;
            using FEndian = details::Field<Bool, 9>;

        public:

            constexpr inline
            PodDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Pod)
                    utils::Throw<std::invalid_argument>("Invalid POD-serdes identifier");
            }

            /// POD type ID
            [[nodiscard]] constexpr inline
            PodId GetPodId()const
            {
                return static_cast<PodId>(GetValue<FPodId>());
            }

            [[nodiscard]] constexpr inline
            std::endian GetEndianness()const
            {
                return GetValue<FEndian>() ? std::endian::little : std::endian::big;
            }
    };

} // serdes

//------------------------------------------------------------------------------


