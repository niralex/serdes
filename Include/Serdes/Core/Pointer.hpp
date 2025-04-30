#ifndef SERDES_CORE_POINTER_HPP
#define SERDES_CORE_POINTER_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Сериализатор/десериализатор значений через указатель

    @details
        Сериализация указателя выполняется с учетом значения nullptr.

        Выделение памяти при десериализации происходит с помощью вызываемой сущности Allocate.
        которая задается в качестве третьего параметра шаблона седеса
        По умолчанию Allocate пвыделяет память с помощью оператора new,
        если переданный указатель равен nullptr, иначе память не выделяется.
        При этом память должна быть выделена внешним кодом или интеллектуальным указателем
        с автоматическим выделением памяти.
        Можно задать другие значения для Allocate, например:
        []<typename TPtr>(TPtr &ptr) { ptr = TPtr(new ValueT<TSerdes>); } // память выделяется всегда
        []<typename TPtr>(TPtr &) {} // память не выделяется никогда
        []<typename TPtr>(TPtr &ptr) { delete ptr; ptr = TPtr(new ValueT<TSerdes>); } //  память вначале освобождается, а затем выделяется

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Pod.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Седес для укзателей
    /// @tparam TSerdes - базовый седес для значений на которые указывает указатель
    /// @tparam TPointer - тип указателя
    /// @tparam Allocate - вызываемая сущность для выделения памяти (по умолчанию память выделяется, если указатель равен nullptr)
    template<CSerdes TSerdes,
             CPointerLike<ValueT<TSerdes>> TPointer,
             auto Allocate = []<typename TPtr>(TPtr &ptr) { if(!ptr) ptr = TPtr(new ValueT<TSerdes>); } >
    struct Pointer
    {
        using ValueType = TPointer;

        using SerdesT = TSerdes;

        using SerdesList = std::tuple<Const<Pod<bool, PodTypeId::Bool>, false>, TSerdes>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Variant; }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return BufferType::Dynamic; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return SerdesT::Sizeof() + 1; }

        // Для случая вызова PointerSerdes::Sizeof(nullptr);
        [[nodiscard]] static constexpr
        uint32_t Sizeof(std::nullptr_t) { return 1; }

        template<CPointerLike<ValueT<SerdesT>> TPtr>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TPtr &ptr)
        {
            if(ptr)
                return SerdesT::Sizeof(*ptr) + 1;
            else return Sizeof(nullptr);
        }

        // Для случая вызова PointerSerdes::SerializeTo(bufpos, nullptr);
        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, std::nullptr_t)
        {
            return Pod<uint8_t>::SerializeTo(bufpos, 0);
        }

        template<COutputIterator TOutputIterator, CPointerLike<ValueT<SerdesT>> TPtr>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TPtr &pvalue)
        {
            if (pvalue) // проверка, что указатель не равен nullptr
            {
                bufpos = Pod<uint8_t>::SerializeTo(bufpos, 1);
                return SerdesT::SerializeTo(bufpos, *pvalue);
            }
            else
                return SerializeTo(bufpos, nullptr);
        }

        // Десериализация
        template<CInputIterator TInputIterator, CPointerLike<ValueT<SerdesT>> TPtr>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TPtr &pvalue)
        {
            bool notNull;
            bufpos = Pod<uint8_t>::DeserializeFrom(bufpos, notNull);
            if(notNull) // если сериализован не нулевой указатель
            {
                Allocate(pvalue);
                return SerdesT::DeserializeFrom(bufpos, *pvalue);
            }
            else
            {
                pvalue = nullptr;
                return bufpos;
            }
        }
    };

} // serdes

//------------------------------------------------------------------------------
#endif
