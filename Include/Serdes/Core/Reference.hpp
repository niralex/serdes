#ifndef SERDES_CORE_REFERENCE_HPP
#define SERDES_CORE_REFERENCE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serializer/deserializer via pointer without handling nullptr

    @details
        The Reference serdes is similar to Pointer but differs in the following ways:

        1) Reference cannot serialize or deserialize nullptr values.
           If nullptr is passed to the serialization or Sizeof functions, an exception is thrown.
           Reference is intended solely for serializing guaranteed non-null values through a pointer.
           During deserialization, a nullptr pointer may be passed; in this case,
           memory is allocated using the provided allocator (just like in Pointer).
           Otherwise, no allocation occurs, and the object is deserialized in-place.

        2) Pointer always has TypeId::Variant, whereas Reference inherits the base serdes type:
           Reference::GetTypeId() == Reference::SerdesType::GetTypeId()

        3) Pointer always uses a dynamic buffer, while Reference inherits the buffer type from the base serdes:
           Reference::GetBufferType() == Reference::SerdesType::GetBufferType()

        4) A value serialized with Reference occupies one byte less in the buffer than with Pointer.

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Exception.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    /// @tparam TSerdes - base serdes for the values pointed to
    /// @tparam TPointer - pointer type
    template<CSerdes TSerdes,
             CPointerLike<ValueT<TSerdes>> TPointer,
             typename Allocator = details::DefaultAllocator<TSerdes>>
    struct Reference
    {
        using ValueType = ValueT<TSerdes>;

        using SerdesType = TSerdes;

        constexpr inline static
        Allocator alloc{};

        static consteval
        TypeId GetTypeId() { return SerdesType::GetTypeId(); }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return SerdesType::GetBufferType(); }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return SerdesType::Sizeof(); }

        template<CPointerLike<ValueT<SerdesType>> TPtr>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TPtr &refer)
        {
            if(refer)
                return SerdesType::Sizeof(*refer);
            else
                utils::Throw<std::invalid_argument>("Pointer must not be null");
        }

        // Handles Reference<>::Sizeof(nullptr);
        [[nodiscard]] static
        uint32_t Sizeof(std::nullptr_t) { utils::Throw<std::invalid_argument>("Pointer must not be null"); }

        template<COutputIterator TOutputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TPtr &refer)
        {
            if(refer)
                return SerdesType::SerializeTo(bufpos, *refer);
            else
                throw std::invalid_argument("Serdes/Core/Reference.hpp/SerializeTo(..., const TPtr &refer): refer must not be null");
        }

        template<CInputIterator TInputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TPtr &refer)
        {
            if(!refer)
                alloc(refer);
            return SerdesType::DeserializeFrom(bufpos, *refer);
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif
