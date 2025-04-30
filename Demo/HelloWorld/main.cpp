//------------------------------------------------------------------------------
/** @file

    @brief The simplest example of serialization-deserialization values of standard types.
    A use case in which a buffer of the required size is automatically created.

    @details

    @todo


    @author Niralex
*/
//------------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <Serdes/Serdes.hpp>

//------------------------------------------------------------------------------
int main()
{
    using namespace serdes;

    // Test data
    std::string_view str("Hello World!");
    unsigned num = 2025;

    // Serializer-deserializer type definition
    using MySerdes = Tuple<String8, UInt64>;

    // Creating a buffer
    char buffer[Sizeof<MySerdes>()];

    // Serialization
    SerializeTo<MySerdes>(buffer, str, num); // 264 byte

    // Deserialization
    auto [_str, _num] = DeserializeFrom<MySerdes>(buffer);

    // Check
    assert(_str == str);
    assert(_num == num);

    return 0;
}
