#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief Base descriptor class

    @details

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

    class Descriptor
    {
        protected:
            using FSize = details::Field<UInt16, 0>;
            using FTypeId = details::Field<UInt8, 2>;
            using FBufferType = details::Field<UInt8, 3>;
            using FBufferSize = details::Field<UInt32, 4>;

            const uint8_t *_id; 

            template<typename TField>
            constexpr inline
            auto GetValue()const
            {
                return Deserialize<typename TField::SerdesT>(_id + TField::offset);
            }

        public:
            constexpr inline
            Descriptor(const uint8_t *id)
            : _id(id) {}

            constexpr inline
            operator const uint8_t *()const { return _id; }

            // Descriptor size
            [[nodiscard]] constexpr inline
            uint16_t GetSize()const
            {
                return GetValue<FSize>();
            }

            [[nodiscard]] constexpr inline
            const uint8_t *GetSerdesId()const { return _id; }

            // Serdes type identifier
            [[nodiscard]] constexpr
            TypeId GetTypeId()const
            {
                return static_cast<TypeId>(GetValue<FTypeId>());
            }

            [[nodiscard]] constexpr
            BufferType GetBufferType()const
            {
                return static_cast<BufferType>(GetValue<FBufferType>());
            }

            // Buffer size for value
            [[nodiscard]] constexpr
            uint32_t Sizeof()const
            {
                return GetValue<FBufferSize>();
            }
			
			/// Check descriptors for equality (full compatibility)
			/// @return true if descriptor sizes and data are byte-wise equal
            [[nodiscard]] constexpr inline
            bool operator==(Descriptor descriptor)const
            {
                return std::equal(_id, _id + GetSize(), descriptor._id, descriptor._id + descriptor.GetSize());
            }

            [[nodiscard]] constexpr inline
            bool operator!=(Descriptor descriptor)const
            {
                return !operator==(descriptor);
            }
    };

} // serdes

//------------------------------------------------------------------------------
