#ifndef SERDES_CORE_CONST_HPP
#define SERDES_CORE_CONST_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes template for constants

    @details
           This serdes performs no actual serialization/deserialization
           and serves as a placeholder in composite serdes to "pass through" constant values.
          
           ValueT<Const> is unique for each constant. To achieve this, each numeric constant is wrapped 
		   in a special helper template called Constant, or ConstexprString (for string literals).

    @todo


    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <variant>
#include <algorithm>
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Typeids.hpp"
#include "Pod.hpp"


using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Helper template that allows treating constants as types,
    /// particularly enabling the definition of ValueType in the Const serdes.
    template<auto v>
    struct Constant
    {
        /// Base type of the constant
        using Type = decltype(v);

        /// Constant value
        static constexpr inline Type Value = v;

        /// The constant can be instantiated as an object, not just used as a typeп
        constexpr inline
        Constant() {}

        /// Any copy or assignment is allowed but does not modify the constant
        template<typename T>
        constexpr inline
        Constant(const T &){}

        template<typename T>
        [[nodiscard]] constexpr inline
        Constant operator=(const T &){ return Constant(); }

        /// Conversion to the base type for use in expressions
        [[nodiscard]] constexpr inline
        operator Type() { return Value; }

        [[nodiscard]] constexpr inline
        operator const Type()const { return Value; }
    };

    /// Helper template enabling string literals to be used as non-type template parameters,
    /// which allows defining Const serdes for string constants.
    template <typename TChar, std::size_t N>
    struct ConstexprString
    {
        using CharT = TChar;

        std::array<CharT, N> _data;

        constexpr inline
        ConstexprString(const TChar (&str)[N]) {  std::copy_n(str, N, _data.begin()); }

		// Overload of operator== is required for types used as non-type template parameters
        constexpr inline
        bool operator==(const ConstexprString&) const = default;

        // Conversion operator to std::string_view
        constexpr inline
        operator std::string_view() const { return std::string_view(_data.data()); }

        // Conversion operator to std::array
        constexpr inline
        operator std::array<CharT, N>()const {  return _data; }

        // String length excluding the terminating '\0'
        constexpr inline
        std::size_t size() const { return N - 1; }

        constexpr
        const CharT* c_str() const { return _data.begin(); }

        // Methods to satisfy std::ranges::forward_range concept
        constexpr
        const CharT* begin() const { return _data.begin(); }

        constexpr
        const CharT* end() const { return _data + size(); }
    };

    /// Serdes template for constants
    /// @tparam TSerdes - base serdes for the constant
    /// @tparam Value - constant value
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
