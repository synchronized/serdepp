#include "serdepp/serde.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"

#include <catch2/catch_all.hpp>

struct Object {
    DERIVE_SERDE(Object,
                 (&Self::radius, "radius")
                 (&Self::width, "width")
                 (&Self::height, "height"))
    std::optional<int> radius;
    std::optional<int> width;
    std::optional<int> height;
};
struct Test {
    DERIVE_SERDE(Test,
                 (&Self::type, "type")
                 (&Self::object, "object", flatten)
                 )
    std::string type;
    Object object;
};

struct Test_v2 {
    DERIVE_SERDE(Test_v2,
                 (&Self::type, "type")
                 [attributes(flatten)]
                 (&Self::object, "object")
                 )
    std::string type;
    Object object;
};

nlohmann::json jflat = R"([{"type": "circle", "radius": 5}, {"type": "rectangle", "width": 6, "height": 5}])"_json;

nlohmann::json j = R"([{"type": "circle", "object": {"radius" : 5}},
 {"type": "rectangle", "object": {"width": 6, "height": 5}}])"_json;

TEST_CASE("set_with_sugar_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = jflat;
        meter.measure([&] { 
            serde::deserialize<std::vector<Test>>(test_data);
        });
    };
}

TEST_CASE("set_with_default_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = jflat;
        meter.measure([&] { 
            serde::deserialize<std::vector<Test_v2>>(test_data);
        });
    };
}