//------------------------------------------------------------------------------
/** @file

    @brief The simplest example of serialization-deserialization values of standard types

    @details

    @todo


    @author Niralex
*/
//------------------------------------------------------------------------------
#include <iostream>
#include <cassert>
#include <array>
#include <Serdes/Serdes.hpp>

//------------------------------------------------------------------------------

using namespace std;
using namespace serdes;

//------------------------------------------------------------------------------
int main()
{

    //Serializer-Deserializer type declaration
    using MySerdes = Array<UInt64, 3>;

    // Test data (std::array<uint64_t, 3>)
    constexpr ValueT<MySerdes> data{112345678, 99999999, 123123123123}; //

    // constexpr serialization
    constexpr auto buffer = Serialize<MySerdes>(data);

    static_assert(std::ranges::range<decltype(buffer)>);
    static_assert(buffer.size() == Sizeof<MySerdes>());

    // constexpr deserialization
    constexpr auto _data = DeserializeFrom<MySerdes>(buffer.begin());

    // check
    static_assert(_data == data);

    return 0;
}
