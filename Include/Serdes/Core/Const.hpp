#ifndef SERDES_CORE_CONST_HPP
#define SERDES_CORE_CONST_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Шаблон седеса для констант и вспомагательные шаблоны

    @details
           Седес не выполняет никакой фактической сериализации/десериализации
           и служит в качестве заглушки в составных седесах для "передачи" константных значений.
           ValueT<Const> уникален для каждой константы. Для этого каждая числовая константа
           "заворачивается" в специальный вспомагательный шаблон Constant или
           ConstexprString(для строковых литералов).

    @todo


    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <variant>
#include <algorithm>
#include "Concepts.hpp"
#include "Typeids.hpp"
#include "Pod.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Вспомагательный шаблон, который позволяет работать с константами как с типами,
    /// в частности, позволяет определять ValueType в седесе Const
    template<auto v>
    struct Constant
    {
        /// Базовый тип константы
        using Type = decltype(v);

        /// Значение константы
        static constexpr inline Type Value = v;

        /// Константа может быть определенв как объект, а не только использоваться как тип
        constexpr inline
        Constant() {}

        /// Любые копирования и присваивания допустимы, но не изменяют константу
        template<typename T>
        constexpr inline
        Constant(const T &){}

        template<typename T>
        [[nodiscard]] constexpr inline
        Constant operator=(const T &){ return Constant(); }

        /// Приведение к базовому типу для возможности использования в выражениях
        [[nodiscard]] constexpr inline
        operator Type() { return Value; }

        [[nodiscard]] constexpr inline
        operator const Type()const { return Value; }
    };

    //--------------------------------------------------------------------------
    /// Вспомагательный шаблон,позволяющий использовать строковые литералы как параметры-значения
    /// шаблонов, что позволяет определять седесы Const для строковых констант
    template <typename TChar, std::size_t N>
    struct ConstexprString
    {
        using CharT = TChar;

        std::array<TChar, N> _data;

        constexpr inline
        ConstexprString(const TChar (&str)[N]) {  std::copy_n(str, N, _data.begin()); }

        // Перегрузка оператора == является одним из требований к типам,
        // которые могут использоваться в качестве параметров-значений в шаблонах
        constexpr inline
        bool operator==(const ConstexprString&) const = default;

        // Оператор приведения к типу std::string_view
        constexpr inline
        operator std::string_view() const {  return std::string_view(_data.begin()); }

        // Оператор приведения к типу std::array
        constexpr inline
        operator std::array<TChar, N>()const {  return _data; }

        // Размер строки без завершающего '\0'
        constexpr inline
        std::size_t size() const { return N - 1; }

        constexpr
        const TChar* c_str() const { return _data.begin(); }

        // Методы для поддержки концепции std::ranges::forward_range
        constexpr
        const TChar* begin() const { return _data.begin(); }

        constexpr
        const TChar* end() const { return _data + size(); }
    };


    //--------------------------------------------------------------------------
    /// Шаблон седесов для констант
    /// @tparam TSerdes - базовый седес для константы
    /// @tparam Value - константное значение
    template<CSerdes TSerdes, ValueT<TSerdes> Value>
    struct Const
    {
        using ValueType = Constant<Value>;

        using SerdesType = TSerdes;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Const; }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return BufferType::Static; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return 0; }

        template<typename TValue>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValue &)  { return 0; }

        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &)
        {
            return bufpos;
        }

        template<CInputIterator TInputIterator, typename TValue>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &value)
        {
            value = static_cast<TValue>(ValueType::Value);
            return bufpos;
        }
    };

} // serdes

//------------------------------------------------------------------------------
#endif
