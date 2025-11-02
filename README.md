# Serdes

C++ serialization/deserialization library.

---

## Key Features

- C++20 header-only;
- No external dependencies, no macros, no source code generation;
- Optimized for speed and space;
- Cross-platform compatibility (the format of serialized data is strictly specified);
- Built-in support for standard data types, including STL containers, pointers and tuples;
- Automatic selection of serializers for standard types;
- Provides mechanisms to define serializers/deserializers for custom types;
- Support for constexpr serialization, deserialization;
- Support for different byte order(endianess);
- Includes tools to determine buffer size before serialization;
- Supports using arrays, standard containers, files, sockets, and more as buffers;
- Serializers and deserializers are represented by types rather than data;
- Supports GCC, MinGW, and Clang compilers;

---

## Usage example 1 (simplest usage with automatic selection of serializer types)

```cpp
#include <cassert>
#include <Serdes/Serdes.hpp>

int main()
{
    // All library types and functions are in the "serdes" namespace.
    using namespace serdes;

    // Test data
    std::string_view str("Hello World!");
    unsigned num = 2025;

    // Creating a buffer: 12 + 8 = 20 bytes (for a 64-bit unsigned)
    char buffer[Sizeof(str, num)]; 

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
```

---

## Usage example 2 (typed cross-platform serialization and deserialization)

```cpp
#include <cassert>
#include <Serdes/Serdes.hpp>

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
    auto buffer = Serialize<CrossPlatformSerdes>(i, f, words);

    // Deserialization
    auto [_i, _f, _words] = DeserializeFrom<CrossPlatformSerdes>(buffer.begin());

    // Check
    assert(_i == i);
    assert(_f == f);
    assert(_words == words);

    return 0;
}
```

---

## Usage example 3 (constexpr Serialization/Deserialization)

```cpp
#include <cassert>
#include <array>
#include <Serdes/Serdes.hpp>

int main()
{
	using namespace serdes;
	
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
```
## Usage example 4 (JSON serialization/deserialization)
For a full usage example, see [Demo/JsonArray/main.cpp](./Demo/JsonArray/main.cpp).

## Installation and Usage
To use the library, simply download the include folder, add its path to the compiler's search directories, and include the header file:
```cpp
#include <Serdes/Serdes.hpp>
```

---

## Requirements

C++20 compiler

---

## License

Serdes project is licensed under the [MIT License](./LICENSE).

---

## Feedback

If you find a bug or have suggestions for improvement, feel free to open an issue or submit a pull request. Contributions are welcome!