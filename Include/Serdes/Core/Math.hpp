#ifndef SERDES_CORE_MATH_HPP
#define SERDES_CORE_MATH_HPP
//------------------------------------------------------------------------------
/**	@file

    @brief Безопасные математические функции

    @details В модуле определен ряд математических функций, которые учитывают переполнение
        разярадной сетки. Поведение функций при возниконовении переполнения определяется политикой,
        которая указывается при вызове функции.

    @todo

    @author Niralex
*/
//------------------------------------------------------------------------------
#include <limits>

//------------------------------------------------------------------------------
namespace utils
{
    //--------------------------------------------------------------------------
    /// Перечисление типов ошибок в математических функциях
    enum class MathError : uint8_t
    {
        overflow = 1 // переполнение разрядной сетки

        // ...
    };

    //--------------------------------------------------------------------------
    /// Концепт политики обработки ошибок математических операций
    template<typename TPolicy>
    concept CMathErrorPolicy = requires(MathError e, const char *msg)
    {
        /// Требование определения constexpr static функции, которая обрабатывает ошибку.
        /// @param e  тип ошибки (MathError),
        /// @param left, right Левый и правый аргументы математической операции
        /// @param msg Cтрока с текстовым сообщением об ошибке.
        { TPolicy::Handle(e, 0, 0, msg) } -> std::convertible_to<uint64_t>;
    };

    //--------------------------------------------------------------------------
    /// Пространство имен для политик обработки ошибок, возникающих при математических операциях
    namespace policy
    {
        /// Генерируется исключение типа std::runtime_error
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

        /// Возвращается максимально возможное значение для типа T
        struct MaxValue
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return std::numeric_limits<T>::max(); }
        };

        /// Возвращается минимально возможное значение для типа T
        struct MinValue
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return std::numeric_limits<T>::min(); }
        };

        /// Возвращается наибольший аргумент
        struct MaxArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T l, T r, const char *) { return std::max(l, r); }
        };

        /// Возвращается наименьший аргумент
        struct MinArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T l, T r, const char *) { return std::min(l, r); }
        };

        /// Возвращается аргумент, стоящий слева от знака операции
        struct LeftArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError e, T l, T r, const char *msg) { return l; }
        };

        /// Возвращается аргумент, стоящий справа от знака операции
        struct RightArg
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T r, const char *) { return r; }
        };

        /// Возвращается нуль
        struct Null
        {
            template<typename T>
            static constexpr
            T Handle(MathError, T, T, const char *) { return 0; }
        };
    }

    //--------------------------------------------------------------------------
    /// Структура определяющая математические функции
    /// @tparam Policy Политика обработки ошибок
    template<CMathErrorPolicy Policy>
    struct Safe
    {
        //----------------------------------------------------------------------
        /// Сложение беззнаковых целых числел с учетом переполнения
        /// @return Если возникает переполнение разрядной сетки, то генерируется исключение
        /// или возвращается значение определяемое политикой Policy
        template<typename T>
        requires std::is_unsigned_v<T>
        [[nodiscard]] static constexpr T Add(T l, T r)
        {
            T res(l + r);
            if(res < l)
                return Policy::Handle(MathError::overflow, l, r, "Serdes/Core/Math.hpp/Add(): overflow occurred");
            return res;
        }

        //--------------------------------------------------------------------------
        /// Умножение беззнаковых целых с учетом переполнения
        /// @return Если возникает переполнение разрядной сетки, то генерируется исключение
        /// или возвращается значение определяемое политикой Policy
        template<typename T>
        requires std::is_unsigned_v<T>
        [[nodiscard]] static constexpr T Mul(T l, T r)
        {
            if(r !=0 && std::numeric_limits<T>::max()/r < l)
                return Policy::Handle(MathError::overflow, l, r, "Serdes/Core/Math.hpp/Mul(): overflow occurred");
            else
                return l * r;
        }
    };
    //--------------------------------------------------------------------------

} // utils
//------------------------------------------------------------------------------
#endif
