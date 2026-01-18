//------------------------------------------------------------------------------
/** @file

    @brief Demonstration of basic features

    @details
    Typed cross-platform serialization and deserialization.

    @todo


    @author Niralex
*/
//------------------------------------------------------------------------------

#include <cassert>
#include <Serdes/Serdes.hpp>

//------------------------------------------------------------------------------
int main()
{
    using namespace serdes;

    // Test data
    int i = 2024;
    float f = 3.14;
    std::vector<std::string> words{"fast ", "cross-platform ", "data exchange ", "tool"};

    // Declaring a serializer/deserializer type
    using CrossPlatformSerdes = Tuple<Int64, Float, Vector8<String>>;

    // Serialization
    auto buffer = SerializeToVector<CrossPlatformSerdes>(i, f, words);

    // Deserialization
    auto [_i, _f, _words] = Deserialize<CrossPlatformSerdes>(buffer.begin());

    // Check
    assert(_i == i);
    assert(_f == f);
    assert(_words == words);

    return 0;
}
