#ifndef SERDES_CORE_MATH_HPP
#define SERDES_CORE_MATH_HPP
//------------------------------------------------------------------------------
/**	@file

    @brief Safe mathematical functions

    @details This module defines a set of mathematical functions that detect arithmetic overflow.
        The behavior upon overflow is determined by a policy specified at the call site.

    @todo

    @author Niralex
*/
//------------------------------------------------------------------------------
#include <limits>

//------------------------------------------------------------------------------
namespace utils
{
    /// Enumeration of error types for mathematical functions
    enum class MathError : uint8_t
    {
        overflow = 1 // arithmetic overflow

        // ...
    };

    /// Concept for error-handling policies in mathematical operations
    template<typename TPolicy>
    concept CMathErrorPolicy = requires(MathError e, const char *msg)
    {
        /// Requirement: a constexpr static function that handles errors must be defined.
        /// @param e  error type (MathError),
        /// @param left, right Left and right operands of the mathematical operation
        /// @param msg C-style string containing an error message.
        { TPolicy::Handle(e, 0, 0, msg) } -> std::convertible_to<uint64_t>;
    };

    /// Namespace for error-handling policies used in mathematical operations
    namespace policy
    {
        /// Throws an exception of type std::runtime_error
        struct Exception
        {
            template<typename T>
            static constexpr
            T Handle(MathError e, T, T, const char *msg)
            {
                switch(e)
                {
                    case MathError::overflow:
                        throw std::overflow_error(msg);
                    // MathError:: ... :
                    default:
                        throw std::runtime_error(msg);
                }
            }
        };

        /// Returns the maximum representable value for type T
        struct MaxValue
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return std::numeric_limits<T>::max(); }
        };

        /// Returns the minimum representable value for type T
        struct MinValue
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return std::numeric_limits<T>::min(); }
        };

        /// Returns the larger of the two operands
        struct MaxArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T l, T r, const char *) { return std::max(l, r); }
        };

        /// Returns the smaller of the two operands
        struct MinArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T l, T r, const char *) { return std::min(l, r); }
        };

        /// Returns the left operand
        struct LeftArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError e, T l, T r, const char *msg) { return l; }
        };

        /// Returns the right operand
        struct RightArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T r, const char *) { return r; }
        };

        /// Returns zero
        struct Null
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return 0; }
        };
    }

    /// Structure defining safe mathematical functions
    /// @tparam Policy Error-handling policy
    template<CMathErrorPolicy Policy>
    struct Safe
    {
        /// Addition of unsigned integers with overflow detection
        /// @return If an overflow occurs, either an exception is thrown
        /// or a value determined by the Policy is returned
        template<typename T>
        requires std::is_unsigned_v<T>
        [[nodiscard]] static constexpr 
		T Add(T l, T r)
        {
            T res(l + r);
            if(res < l)
                return Policy::Handle(MathError::overflow, l, r, "Serdes/Core/Math.hpp/Add(): overflow occurred");
            return res;
        }

        /// Multiplication of unsigned integers with overflow detection
        /// @return If an overflow occurs, either an exception is thrown
        /// or a value determined by the Policy is returned
        template<typename T>
        requires std::is_unsigned_v<T>
        [[nodiscard]] static constexpr 
		T Mul(T l, T r)
        {
            if(r != 0 && std::numeric_limits<T>::max() / r < l)
                return Policy::Handle(MathError::overflow, l, r, "Serdes/Core/Math.hpp/Mul(): overflow occurred");
            else
                return l * r;
        }
    };

} // namespace utils
//------------------------------------------------------------------------------
#endif
