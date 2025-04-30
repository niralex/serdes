#ifndef SERDES_SERDES_HPP
#define SERDES_SERDES_HPP

//------------------------------------------------------------------------------
/** @file

	@brief Главный подключаемый файл библиотеки

	@details Для полноценного использования библиотеки достаточно подключить только этот файл.

    @todo
        Добавить демо-примеры: constexpr, сер/дес в файл, RPC, сериализация с другим порядком байт
        Протестировать дескрипторы

        Доработки после выладивания на githab:
        01) Сделать комментарии на английском
        02) Рефакторинг кода
        1) Добавить функцию Match для дескрипторов
        2) Добавить шаблон Sizeof(TInputIterator *bufpos)
        3) Разработать функцию GetValue для PodDescriptor, которая возвращает variant.
           Возможно, сделать GetValue для Descriptor, которая возвращает
           полноценную конструкцию из variant, например: tuple<vector<variant>, variant, ...>
        4) Добавить демо-примеры
        5) Тестирование

        Добавить к функциям noexcept

        01 ?Разнести определения типов седесов из Typedefs в файлы определения седесов
           (так как некоторые седесы используются для реализации других седесов, например Variant, Pointer, Enum)

        02 Доработать тесты для тестирования дополнительных функций сериализации/десериализации

        03. Определить, следует ли сделать седес для потоков
            (потребуется реализовать отдельную сущность Поток как источник/приемник данных без размера)
            (возможно сделать несколько предопределенных сущностей для стандартных потоков)

        1. Попробовать Const для Tuple

        00 Протестировать различные комбинации с Const, Pointer, Variant, Tuple

        00.

        01. Улучшить систему идентификации POD-типов. Основные требования:
          быстрое получение некоторых свойств типов,
          1-2 битовые опепрации(IsChar, IsInteger, IsFloat, IsCalendar, Edianess)

        0. Сделать RuntimeSerdes, виртуальный деструктор, проверить освобождение динамической памяти

        0. Сделать седес для дескриптора

        1. Добавить поддержку аллокаторов в строках

        1. Добавить функции-шаблоны Next в пространство details для Tuple, Range и Array и вызывать их
        в соответствующих классах дескрипторов. В классе Descriptor проверять с помощью if
        Второй вариант - вынести реализацию Descriptor::Next в отдельный файл
        Подумать над другими вариантами реализации

        2. Сделать для классов Buffer огрничение размера для динамического буфера.
           Т.е., если размер динамического буфера меньше заданного, то использовать статический буфер

        3. Рассмотреть переход на 8-байтный размер буфера (минус - не на всех платформах поддерживается)

        4. Придумать демо-примеры для дескрипторов(в применении к биржам):
           0)!!! Сериализация произвольного JSON (парсинг с помощью любого простого парсера,
           и запись данных в двоичный файл
           десериализация двоичных данных и создание JSON или вывод
           Дополнительно можно сравнить разницу размеров файлов json и двоичного json
           Также, можно сохранять данные без ключей, что еще больше сократит размер

           1) сериализация в файл случайных данных, и последующая десериализация
           (возможно сериализация json)
           2) передача данных по сети
           3) передача параметров функций при динамическом связывании(удаленный вызов процедур, RPC)


        2. Добавить GetBufferSize аналогично Next

		3. Рефакторинг функции Match с учетом иерархии DescriptorImpl

		4. Протестировать дескрипторы


        4.1. Рассмотреть добавление в библиотеку сущности View (Datagramm/Frame/Object/Data/View/Flat/Raw/Plain/Value/Bin)


        6. Рефакторинг системы исключений

        Вопросы для оптимизации:
        - оставить Struct(более оптимальный способ объявлять кастомные седесы по-сравнению с Custom)
         возможно, сделать проверку указателей на члены, являются они указателями на поля или на функции-члены

        - оставить Custom(по сравнению с седесом Struct менее оптимальный способ объявлять кастомные седесы,
          но единственно возможный для пользовательских классов, у которых данные приватные и который можно задать только путем вызова функций или конструкторов)
          Сделать реализацию седеса для std::complex как кастомного седеса.
        - добавить ограничения по длине для типов дат, времени и других, длина которых четко не задана стандартом

        - разобраться с Alias(оставлять или нет?), скорее нет.

        Рассмотреть возможность сериализации объектов полиморфных иерархий с помощью Custom-шаблона,
        Определить, как будут сохраняться объекты полиморфных иерархий пользовательских класов через
        указатели на базовые классы(достаточно ли будет функций преобразования типов в кастомном седесе?)

        Усовершенствовать седес Struct для поддержки экстракторов/указателей на поля(для случаев когда доступны фунции, возвращающие ссылки на внутренние данные) и т.д.
        Добавить в седес Tuple поддержку указателей с целью ооптимизации доступа к данным
        в Custom-седесах

        Возможно, учитывать указатели на данные
        для тех случаев, когда нельзя разименовывать указатель
        Вторая и основная причина - возможность создания Custom-седесов без необходимости копирования
        значений при преобразовании пользовательского типа к Value-типу седеса
        сделать проверку на NULL/nullptr,
        Внести изменения в глоббальную функцию десериализация из внешнего буфера с автоматическим созданием значений
        Или добавить в каждый седес функцию для указателя на значение при сериализации и десериализации
        В этой же функции добавить поддержку типов без конструкторов по умолчанию
        Возможно, добавить указатели только для Custom и Struct
        Рассмотреть поддержку полиморфных иерархий(реализовать систему идетификации типов).
        template<typename CSerdes> struct Pointer{...}

        Добавить новые типы, например, потоки, Decimal, any/variant, raw/blob/image, hyperlink/url,
        json, optional, VLQ(упакованные целые), идентификаторы седесов

        Рассмотреть новый тип на базе std::variant/any, в буфер записывать id седеса.
        Рассмотреть универсальный тип седеса, который будет сериализовывать дескриптор вместе с данными

        10. ????Рассмотреть вопрос потоковых типов.
        Все передаваемые данные можно условно разделить на три типа:
        - фиксированного размера;
        - динамического размера(когда размер известен на момент начала передачи данных);
        - потоковые (когда размер передаваемых данных не известен на момент начала передачи данных)
        10.1 Вместо функции IsDynamic в седесах, добавить функцию GetBufferTypeId() которая будет
        возвращать значение перечисления (BufferType::Static, Dynamic, Stream)

        10.1 Добавить новый седес Stream для потоков

        например BinaryStream, который просто копирует данные из
        заданного потока в поток буфера или наоборот. Разобраться только с порциями, которые нужно брать
        с потока и писать в поток, согласованием скоростей, признаками окончания потоков, ожиданием данных
        из потоков, исключения связанные с потоками.
        Второй вариант - сделать потоки на уровне библиотеки elements, в виде типов StreamInput/StreamOutput
        с указанием возможных параметров потоков в виде параметров-типов для шаблона

        ==============================================
        Написать документацию
        1. Добавить в концепцию следующий принцип:
        Любые данные, описывающие седесы должны быть сериализуемыми/десериализуемыми с помощью седесов.
        Как минимум это необходимо для передачи дескрипторов и работы с ними.
        2. Преобразование типов соответствует c++, т.е. седес предназначеный для сохранения UInt8 может
        сохранять значения int64_t, но возможна потеря информации.

    @author Niraleks

*/

//------------------------------------------------------------------------------
//#include <limits>
//#include <type_traits>
//#include <vector>
//#include <list>
//#include <deque>
//#include <map>
//#include <set>
//#include <complex>
//#include <Serdes/Core/Typedef.hpp>
#include "Core/Concepts.hpp"
//#include "Core/Buffers.hpp"
//#include "Core/Pod.hpp"
//#include "Core/Range.hpp"
//#include "Core/Tuple.hpp"
//#include "Core/Array.hpp"
//#include "Core/Custom.hpp"
//#include "Core/Struct.hpp"
#include "Core/Typedefs.hpp"
#include "Core/Api.hpp"
//#include <Serdes/Descriptors.hpp>
//#include <Serdes/Descriptor/PodDescriptor.hpp>
//#include <Serdes/Descriptor/TupleDescriptor.hpp>
//#include <Serdes/Descriptor/RangeDescriptor.hpp>
//#include <Serdes/Descriptor/ArrayDescriptor.hpp>
//#include <Serdes/Descriptor/MakeTraits.hpp>
//#include <Serdes/Descriptor/Next.hpp>
//#include <Serdes/Descriptor/Sizeof.hpp>
//#include <Serdes/Descriptor/Match.hpp>
//#include <Serdes/Descriptor/Make.hpp>

//-----------------------------------------------------------------------------
namespace serdes
{
//    // Функция возвращает массив с сериализованными значениями
//    // Данная версия функции выбирается компилятором в случаях, если все седесы статические
//    // не являются базовыми типами соответствующих седесов и не могут быть неявно приведены к ним
//    // В этих случаях соответствующий седес должен иметь функцию сериализации для соответствующего типа
//    // (например, Serialize<Tuple<Int, Int>>(std::array<int, int>{1,2}))
//    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename ...TValues>
//    requires (!Tuple<TSerdes...>::IsDynamic())
//    constexpr inline
//    auto Serialize(TOutputIterator bufpos, const TValues &...values) // (2)
//    {
//        std::array<uint8_t, Tuple<TSerdes...>::Sizeof()> buffer;
//        Tuple<TSerdes...>::SerializeTo(buffer.begin(), values...);
//        return buffer;
//    }
//
//    // Функция возвращает массив с сериализованными значениями
//    // Данная версия функции выбирается компилятором в случаях, если все седесы статические
//    // не являются базовыми типами соответствующих седесов и не могут быть неявно приведены к ним
//    // В этих случаях соответствующий седес должен иметь функцию сериализации для соответствующего типа
//    // (например, Serialize<Tuple<Int, Int>>(std::array<int, int>{1,2}))
//    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename ...TValues>
//    requires (!Tuple<TSerdes...>::IsDynamic())
//    //requires (sizeof...(TSerdes) > 1)
//    //requires (!std::is_convertible_v<TValues, ValueT<TSerdes>> || ...) // типы не должны неявно приводится(для исключения неоднозначности)
//    constexpr inline
//    auto Serialize(TOutputIterator bufpos, const TValues &...values) // (2)
//    {
//        std::array<uint8_t, Tuple<TSerdes...>::Sizeof()> buffer;
//        Tuple<TSerdes...>::SerializeTo(buffer.begin(), values...);
//        return buffer;
//        //return Serialize<Tuple<TSerdes...>>(bufpos, values...);
//        //((bufpos = TSerdes::SerializeTo(bufpos, values)), ...);
//        //return bufpos;
//    }
//
//    /// Десериализация из внешнего буфера с автоматическим созданием значений
//    template<CSerdes ...TSerdes, CInputIterator TInputIterator>
//    constexpr inline
//    auto Deserialize(TInputIterator bufpos)
//    {
//        std::tuple<ValueT<TSerdes>...> values;
//        Tuple<TSerdes...>::DeserializeFrom(bufpos, values);
//        if constexpr (sizeof...(TSerdes) > 1)
//            return values;
//        else
//            return std::get<0>(values);
//    }

}

//------------------------------------------------------------------------------
#endif


//----------------------------------------------------------------------
/*
int main()
{

    constexpr auto sd = GetDescriptor<UInt32>();
    constexpr auto td = GetDescriptor<Tuple<UInt32, UInt32>>();

    std::byte buffer[10];

    cout << sd.SizeofBuffer(buffer) << endl;
    cout << td.SizeofBuffer(buffer) << endl;

    const uint8_t *sd_data = sd.GetData();
    const uint8_t *td_data = td.GetData();

    auto dsd = GetDescriptor(sd_data);
    auto dtd = GetDescriptor(td_data);

    //cout << dsd.SizeofBuffer(buffer) << endl;
    //cout << dtd.SizeofBuffer(buffer) << endl;

    return 0;
}
*/

//-----------------------------------------------------------------------------
/*
// Макрос формирующий список из своих аргументов
#define ARGLIST(...) __VA_ARGS__

// Макрос, вставляющий типовой код при опредлении пользовательских сериализаторов

#define CUSTOM_SERDES(SerdesName, CustomType, TSerdes, Fields)

        using ValueType = T;        \
        using Serdes = TSerdes;     \
        using SerdesValueType = ValueT<Serdes>; \
        [[nodiscard]] static consteval const auto &PodTypeId() { return Serdes::PodTypeId(); }  \
        [[nodiscard]] static consteval bool GetBufferType() { return Serdes::IsStatic(); } \
        [[nodiscard]] static consteval uint32_t Sizeof() { return Serdes::Sizeof(); } \
        [[nodiscard]] static consteval uint32_t Sizeof(const ValueType &) { return Serdes::Sizeof(); } \
        [[nodiscard]] static consteval auto GetDescriptor() { return Serdes::GetDescriptor(); }
*/
