//------------------------------------------------------------------------------
/** @file

    @brief

    @details

    @todo


    @author Niralex
*/
//------------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <Serdes/Serdes.hpp>
#include <nlohmann/json.hpp>
//------------------------------------------------------------------------------
// Пример json-строки
std::string jsonText = R"(
[
  {
    "id": 28457,
    "price": "50200.00",
    "qty": "0.005",
    "quoteQty": "251.0000",
    "time": 1713780001234,
    "isBuyerMaker": true,
    "isBestMatch": true
  },
  {
    "id": 28458,
    "price": "50201.00",
    "qty": "0.010",
    "quoteQty": "502.0100",
    "time": 1713780002234,
    "isBuyerMaker": false,
    "isBestMatch": true
  },
  {
    "id": 28459,
    "price": "50200.50",
    "qty": "0.200",
    "quoteQty": "10040.1000",
    "time": 1713780003234,
    "isBuyerMaker": true,
    "isBestMatch": true
  }
])";

//------------------------------------------------------------------------------
using namespace std;
using namespace serdes;
using namespace std::chrono;

using Json = nlohmann::json;
using Trade = vector<std::tuple<uint64_t, double, double, double, std::chrono::nanoseconds, bool, bool>>;

Trade FromJson(const Json &json)
{
    Trade trade;
    for (const auto& item : json) {
        trade.emplace_back(
            item.at("id").get<uint64_t>(),
            stod(item.at("price").get<string>()),
            stod(item.at("qty").get<string>()),
            stod(item.at("quoteQty").get<string>()),
            chrono::nanoseconds(item.at("time").get<int64_t>() * 1'000'000), // миллисекунды в наносекунды
            item.at("isBuyerMaker").get<bool>(),
            item.at("isBestMatch").get<bool>()
        );
    }
    return trade;
}

void ToJson(const Trade& trade, Json& json)
{
    auto FormatDouble = [](double value, int precision) -> std::string
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    };

    // Преобразование из вектора кортежей в json
    json = Json::array();
    for (const auto& [id, price, qty, quoteQty, time, isBuyerMaker, isBestMatch] : trade)
        json.push_back(
        {
            {"id", id},
            {"price", FormatDouble(price, 2)},    // 2 знака после запятой
            {"qty", FormatDouble(qty, 3)},        // 3 знака после запятой
            {"quoteQty", FormatDouble(quoteQty, 4)}, // 4 знака после запятой
            {"time", std::chrono::duration_cast<std::chrono::milliseconds>(time).count()},
            {"isBuyerMaker", isBuyerMaker},
            {"isBestMatch", isBestMatch}
        });
}

using TradeSerdes = Custom<
    Json,
    Vector<Tuple<UInt64, Double, Double, Double, DateTime, Bool, Bool>>,
    FromJson,
    ToJson>;

//------------------------------------------------------------------------------
int main()
{
    // Парсинг json-строки
    auto json = Json::parse(jsonText);

    // Сериализация
    auto buffer = Serialize<TradeSerdes>(json);
    assert(buffer.size() == 130);

    // Десериализация в виде данных для работы в программе
    Trade trade;
    DeserializeFrom<TradeSerdes>(buffer.begin(), trade);
    auto [id, price, qty, quoteQty, time, isBuyerMaker, isBestMatch] = trade[1];
    assert(trade.size() == 3);
    assert(id == 28458);
    assert(isBestMatch == true);

    // Десериализация в json
    auto _json = DeserializeFrom<TradeSerdes>(buffer.begin());
    assert(_json == json);

    return 0;
}
