#ifndef SERDES_CORE_TYPEIDS_HPP
#define SERDES_CORE_TYPEIDS_HPP

//------------------------------------------------------------------------------
/** @file

	@brief ѕеречеслени€ идентификаторов типов

	@details

    @todo


    @author Niraleks

*/

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// ѕеречисление типов седесов
    enum class SerdesTypeId : uint8_t
    {
        Null,   // специальное значение (дл€ ограничени€ списка дескрипторов)
        Pod,
        Range,
        Array,
        Tuple,
        Variant,
        Const
    };

    //--------------------------------------------------------------------------
    /// ѕеречисление типов значений POD-седесов
    // ћладший бит кодирует пор€док байт
    // (четные идентификаторы - little-endian, нечетные - big-endian)
    enum class PodTypeId : uint8_t // GetValueTypeId() == PodTypeId::Pod   Vti Sti
    {
        UChar8    = 0,
        Char8     = 1,
        Char16    = 2,
        Char16B   = 3,
        Char32    = 4,
        Char32B   = 5,

        UInt8     = 16,
        Int8      = 17,
        UInt16    = 18,
        UInt16B   = 19,
        Int16     = 20,
        Int16B    = 21,
        UInt32    = 22,
        UInt32B   = 23,
        Int32     = 24,
        Int32B    = 25,
        UInt64    = 26,
        UInt64B   = 27,
        Int64     = 28,
        Int64B    = 29,

        Float     = 32,
        FloatB    = 33,
        Double    = 34,
        DoubleB   = 35,

        Day       = 64,
        Month     = 65,
        Weekday   = 66,
        Year      = 68,
        YearB     = 69,
        Date      = 70,
        DateB     = 71,
        Time      = 72,
        TimeB     = 73,
        DateTime  = 74,
        DateTimeB = 75,
        Bool      = 128,
        Unspecified = 130, // неопределенный Pod-тип
    };

    //--------------------------------------------------------------------------
    /// ѕеречисление типов буферов
    enum class BufferType : bool
    {
        Static, // размер буфера можно определить на этапе компил€ции без значени€
        Dynamic // необходимый размер буфера можно определить по значению
    };

    //--------------------------------------------------------------------------
    /// Ўаблон сокращенной записи базового типа седеса
    template<typename TSerdes>
    using ValueT = typename TSerdes::ValueType;
}

//------------------------------------------------------------------------------
#endif


