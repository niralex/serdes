#ifndef SERDES_CORE_REFERENCE_HPP
#define SERDES_CORE_REFERENCE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Сериализатор/десериализатор значений через указатель без учета значения nullptr

    @details
        В отличии от Pointer, седес Reference предполагает, что указатель не может принимать
        значения nullptr, т.е. концептуально соответствует ссылке на значение.

        Если при десериализации указатель равен nullptr, то память выделяется с помощью ператора new
        Иначе память не выделяется и десериализуется объект по адресу.

    @todo


    @author Niraleks
*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Седес для указателей (без учета значения nullptr)
    /// @tparam TSerdes - базовый седес для значений на которые указывает указатель
    /// @tparam TPointer - тип указателя
    template<CSerdes TSerdes, CPointerLike<ValueT<TSerdes>> TPointer>
    struct Reference
    {
        //using ValueType = ValueT<TSerdes>;
        using ValueType = TPointer;

        using SerdesType = TSerdes;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesType::GetSerdesTypeId(); }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return SerdesType::GetBufferType(); }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return SerdesType::Sizeof(); }

        template<CPointerLike<ValueT<SerdesType>> TPtr>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TPtr &ptr)
        {
            return SerdesType::Sizeof(*ptr);
        }

        template<COutputIterator TOutputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TPtr &pvalue)
        {
            return SerdesType::SerializeTo(bufpos, *pvalue);
        }

        template<CInputIterator TInputIterator, CPointerLike<ValueT<SerdesType>> TPtr>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TPtr &pvalue)
        {
            if(!pvalue) // если указатель нулевой
                pvalue = TPtr(new ValueT<SerdesType>);
            return SerdesType::DeserializeFrom(bufpos, *pvalue);
        }
    };

} // serdes

//------------------------------------------------------------------------------
#endif
