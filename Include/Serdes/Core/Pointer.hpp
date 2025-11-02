#ifndef SERDES_CORE_POINTER_HPP
#define SERDES_CORE_POINTER_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serializer/deserializer for values accessed through a pointer

    @details
        Pointer serialization accounts for the nullptr value.

        Memory allocation during deserialization is performed by a callable allocator object,
        which can be customized when defining the serdes.
        By default, the allocator uses the new operator only if the provided pointer is nullptr;
        otherwise, it assumes memory has already been allocated externally.
        Alternative allocation strategies can be supplied, for example:
        - []<typename TPtr>(TPtr &ptr) { ptr = TPtr(new ValueT<TSerdes>); } — always allocates memory
        - []<typename TPtr>(TPtr &) {} — never allocates memory
        - []<typename TPtr>(TPtr &ptr) { delete ptr; ptr = TPtr(new ValueT<TSerdes>); } — deallocates first, then allocates

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Pod.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    namespace details
    {
        template<CSerdes TSerdes>
        struct DefaultAllocator
        {
            using ValueType = ValueT<TSerdes>;

            template<CPointerLike<ValueType> TPtr>
            constexpr
            void operator()(TPtr &ptr) const
            {
                if (!ptr) ptr = TPtr(new ValueType);
            }
        };
    }

    /// Serdes for pointers
    /// @tparam TSerdes Base serdes for the values pointed to
    /// @tparam TPointer Pointer type
    /// @tparam Allocator Callable entity for memory allocation (by default, allocates only if the pointer is nullptr)
    template<CSerdes TSerdes,
             CPointerLike<ValueT<TSerdes>> TPointer,
             typename Allocator = details::DefaultAllocator<TSerdes>>
    struct Pointer
    {
        using ValueType = TPointer;

        using SerdesType = TSerdes;

        using SerdesList = std::tuple<Const<Pod<bool, PodTypeId::Bool>, false>, TSerdes>;

        constexpr inline static
        Allocator alloc{};

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Variant; }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return BufferType::Dynamic; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return SerdesType::Sizeof() + 1; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof(std::nullptr_t) { return 1; }

        template<CPointerLike<ValueT<SerdesType>> TPtr>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TPtr &ptr)
        {
            if(ptr)
                return SerdesType::Sizeof(*ptr) + 1;
            else
                return Sizeof(nullptr);
        }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, std::nullptr_t)
        {
            return Pod<uint8_t>::SerializeTo(bufpos, 0);
        }

        template<COutputIterator TOutputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TPtr &pvalue)
        {
            if (pvalue)
            {
                bufpos = Pod<uint8_t>::SerializeTo(bufpos, 1);
                return SerdesType::SerializeTo(bufpos, *pvalue);
            }
            else
                return SerializeTo(bufpos, nullptr);
        }

        template<CInputIterator TInputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TPtr &pvalue)
        {
            bool notNull;
            bufpos = Pod<uint8_t>::DeserializeFrom(bufpos, notNull);
            if(notNull) 
            {
                alloc(pvalue);
                return SerdesType::DeserializeFrom(bufpos, *pvalue);
            }
            else
            {
                pvalue = nullptr;
                return bufpos;
            }
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif