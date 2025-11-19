#include <map>
#include <optional>
#include <variant>

#include <nameof.hpp>
#include <fmt/format.h>
#include <catch2/catch_all.hpp>

#include "serdepp/serializer.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"
#include "serdepp/adaptor/toml11.hpp"
#include "serdepp/adaptor/yaml-cpp.hpp"
#include "serdepp/adaptor/rapidjson.hpp"
#include "serdepp/utility.hpp"


struct test {
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
};

namespace ns {
    struct nl_test {
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
        std::string str;
        int i;
        std::vector<std::string> vec;
        std::map<std::string, std::string> sm;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(nl_test, str, i, vec, sm)
    };
}


using namespace toml::literals;

nlohmann::json json_v = R"({
"str" : "hello",
"i": 10,
"vec": [ "one", "two", "three"],
"sm": { "one" : "tone", "two" : "ttwo"}
})"_json;

test base_t = serde::deserialize<test>(json_v);

//toml::value toml_v = R"(
//vec = ["one", "two"]
//i = 10
//str = "hello"
//[sm]
//one = "tone"
//two = "ttwo"
//)"_toml;


TEST_CASE("nljson_set_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = json_v;
        meter.measure([&] { 
            serde::deserialize<test>(test_data);
        });
    };
}

TEST_CASE("nljson_set_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = json_v;
        meter.measure([&] { 
            auto v = test_data.get<ns::nl_test>();
        });
    };
}

TEST_CASE("nljson_get_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = base_t;
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(test_data);
        });
    };
}

TEST_CASE("nljson_get_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        ns::nl_test t = serde::deserialize<ns::nl_test>(json_v);
        meter.measure([&] { 
            nlohmann::json{t};
        });
    };
}

namespace ext
{
struct test
{
    DERIVE_SERDE(test,
          (&Self::str, "str")
          (&Self::i,   "i")
          (&Self::vec, "vec")
          (&Self::sm,  "sm"))
    std::string str;
    int i;
    std::vector<std::string> vec;
    std::map<std::string, std::string> sm;
    void from_toml(const toml::value& v)
    {
        this->str = toml::find<std::string>(v, "str");
        this->i   = toml::find<int>(v, "i");
        this->vec = toml::find<std::vector<std::string>>(v, "vec");
        this->sm  = toml::find<std::map<std::string, std::string>>(v, "sm");
        return;
    }

    toml::value into_toml() const // you need to mark it const.
    {
        return toml::value(toml::table{
            {"str", this->str}, 
            {"i", this->i}, 
            {"vec", this->vec}, 
            {"sm", this->sm},
        });
    }
};
} // ext

TEST_CASE("toml11_set_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        toml::value toml_v = serde::serialize<toml::value>(base_t);
        meter.measure([&] { 
            serde::deserialize<test>(toml_v);
        });
    };
}

TEST_CASE("toml11_set_tl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        toml::value toml_v = serde::serialize<toml::value>(base_t);
        meter.measure([&] { 
            toml::get<ext::test>(toml_v);
        });
    };
}

TEST_CASE("toml11_get_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = base_t;
        meter.measure([&] { 
            serde::serialize<serde::toml_v>(test_data);
        });
    };
}

TEST_CASE("toml11_get_tl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        ext::test t = serde::deserialize<ext::test>(json_v);
        meter.measure([&] { 
            toml::value v(t);
        });
    };
}

namespace YAML {
    template<> struct convert<test> {
        static Node encode(const test& v) {
            Node node;
            node["str"] = v.str;
            node["i"] = v.i;
            node["vec"] = v.vec;
            node["sm"] = v.sm;
            return node;
        }

        static bool decode(const Node& node, test& v) {
            v.str = node["str"].as<std::string>();
            v.i   = node["i"].as<int>();
            v.vec = node["vec"].as<std::vector<std::string>>();
            v.sm  = node["sm"].as<std::map<std::string, std::string>>();
            return true;
        }
    };
}

TEST_CASE("yaml_set_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        YAML::Node yaml_v = serde::serialize<YAML::Node>(base_t);
        meter.measure([&] { 
            serde::deserialize<test>(yaml_v);
        });
    };
}

TEST_CASE("yaml_set_tl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        YAML::Node yaml_v = serde::serialize<YAML::Node>(base_t);
        meter.measure([&] { 
            yaml_v.as<test>();
        });
    };
}

TEST_CASE("yaml_get_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = base_t;
        meter.measure([&] { 
            serde::serialize<serde::yaml>(test_data);
        });
    };
}

TEST_CASE("yaml_get_tl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = base_t;
        meter.measure([&] { 
            YAML::Node v(test_data);
        });
    };
}

TEST_CASE("rapid_json_set_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        rapidjson::Document rapid_v = serde::serialize<rapidjson::Document>(base_t);
        meter.measure([&] { 
            serde::deserialize<test>(rapid_v);
        });
    };
}

//TEST_CASE("rapid_json_set_tl_bench") {
//    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
//        rapidjson::Document rapid_v = serde::serialize<rapidjson::Document>(base_t);
//        meter.measure([&] { 
//            //rapid_v.as<test>();
//        });
//    };
//}

TEST_CASE("rapid_json_get_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto test_data = base_t;
        meter.measure([&] { 
            serde::serialize<rapidjson::Document>(test_data);
        });
    };
}

//[[maybe_unused]]
//TEST_CASE("rapid_json_get_tl_bench") {
//    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
//        auto test_data = base_t;
//        meter.measure([&] { 
//            //RAPID_JSON::Node v(base_t);
//        });
//    };
//}
