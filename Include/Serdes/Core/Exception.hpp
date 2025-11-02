#ifndef SERDES_CORE_EXCEPTIONS_HPP
#define SERDES_CORE_EXCEPTIONS_HPP
//------------------------------------------------------------------------------
/**	@file

    @brief Exception generation implementation

	@details

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <source_location>

//------------------------------------------------------------------------------
namespace utils
{
    /// Extracts the file name with extension from a full path
    constexpr
    std::string_view GetFileName(std::string_view path) noexcept
    {
        // Handles both separators (/ and \)
        const size_t pos = path.find_last_of("/\\");
        return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
    }

    /// Extracts the function name from a full signature
    constexpr
    std::string_view GetFunctionName(std::string_view pretty) noexcept
    {
        // Extracts the substring from the last "::" up to "("
        const size_t end = pretty.find('(');
        const size_t begin = pretty.find_last_of(':', end);
        if (begin == std::string_view::npos)
            return pretty.substr(0, end);
        return pretty.substr(begin + 1, end - begin - 1);
    }


    /// Throws an exception
    /// @tparam TException Exception type
    /// @param message Exception description message
    /// @param location Standard object describing the source location
    template<typename TException>
    [[noreturn]] constexpr
    void Throw(std::string_view message, const std::source_location& location = std::source_location::current())
    {
        throw TException(std::format(
            "{}:{} in {}: {}",
            GetFileName(location.file_name()),
            location.line(),
            GetFunctionName(location.function_name()),
            message));
    }


} // utils

//------------------------------------------------------------------------------
#endif


