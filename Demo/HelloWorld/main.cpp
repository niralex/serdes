//------------------------------------------------------------------------------
/** @file

    @brief Simplest usage with automatic selection of serializer types.

    @author Niralex
*/
//------------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <Serdes/Serdes.hpp>

//------------------------------------------------------------------------------
int main()
{
    // All library types and functions are in the "serdes" namespace.
    using namespace serdes;

    // Test data
    std::string_view str("Hello World!");
    unsigned num = 2025;

    // Creating a buffer
    char buffer[Sizeof(str, num)]; // 12 + 8 = 20 bytes (for a 64-bit unsigned)

    // Serialization
    SerializeTo(buffer, str, num);

    // Deserialization
    std::string _str("Hello World!");
    unsigned _num;
    DeserializeFrom(buffer, _str, _num);

    // Check
    assert(_str == str);
    assert(_num == num);

    return 0;
}
