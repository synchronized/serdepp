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
#include "serdepp/utility.hpp"

using namespace toml::literals;

int64_t integer64 = 1024;
TEST_CASE("nljson_set_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_int64_t = serde::serialize<nlohmann::json>(integer64); 
        meter.measure([&] { 
            serde::deserialize<int64_t>(json_int64_t);
        });
    };
}

TEST_CASE("nljson_set_int64_t_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_int64_t = serde::serialize<nlohmann::json>(integer64); 
        meter.measure([&] { 
            json_int64_t.get<int64_t>();
        });
    };
}

TEST_CASE("nljson_get_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_int64_t = serde::serialize<nlohmann::json>(integer64); 
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(integer64);
        });
    };
}

TEST_CASE("nljson_get_int64_t_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_int64_t = serde::serialize<nlohmann::json>(integer64); 
        meter.measure([&] { 
            nlohmann::json{integer64}; 
        });
    };
}

TEST_CASE("toml11_set_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_int64_t = serde::serialize<toml::value>(integer64); 
        meter.measure([&] { 
            serde::deserialize<int64_t>(toml11_int64_t);
        });
    };
}

TEST_CASE("toml11_set_int64_t_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_int64_t = serde::serialize<toml::value>(integer64); 
        meter.measure([&] { 
            toml::get<int64_t>(toml11_int64_t);
        });
    };
}

TEST_CASE("toml11_get_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_int64_t = serde::serialize<toml::value>(integer64); 
        meter.measure([&] { 
            serde::serialize<toml::value>(integer64);
        });
    };
}

TEST_CASE("toml11_get_int64_t_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_int64_t = serde::serialize<toml::value>(integer64); 
        meter.measure([&] { 
            toml::value v(integer64);
        });
    };
}

TEST_CASE("yaml_cpp_set_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_int64_t = serde::serialize<YAML::Node>(integer64); 
        meter.measure([&] { 
            serde::deserialize<int64_t>(yaml_cpp_int64_t);
        });
    };
}

TEST_CASE("yaml_cpp_set_int64_t_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_int64_t = serde::serialize<YAML::Node>(integer64); 
        meter.measure([&] { 
            yaml_cpp_int64_t.as<int64_t>();
        });
    };
}

TEST_CASE("yaml_cpp_get_int64_t_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_int64_t = serde::serialize<YAML::Node>(integer64); 
        meter.measure([&] { 
            serde::serialize<YAML::Node>(integer64);
        });
    };
}

TEST_CASE("yaml_cpp_get_int64_t_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_int64_t = serde::serialize<YAML::Node>(integer64); 
        meter.measure([&] { 
            YAML::Node{integer64};
        });
    };
}

float float32 = 1024.1;
TEST_CASE("nljson_set_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_float = serde::serialize<nlohmann::json>(float32); 
        meter.measure([&] { 
            serde::deserialize<float>(json_float);
        });
    };
}
TEST_CASE("nljson_set_float_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_float = serde::serialize<nlohmann::json>(float32); 
        meter.measure([&] { 
            json_float.get<float>();
        });
    };
}
TEST_CASE("nljson_get_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_float = serde::serialize<nlohmann::json>(float32); 
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(float32);
        });
    };
}
TEST_CASE("nljson_get_float_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_float = serde::serialize<nlohmann::json>(float32); 
        meter.measure([&] { 
            nlohmann::json{float32};
        });
    };
}

TEST_CASE("toml11_set_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_float = serde::serialize<toml::value>(float32); 
        meter.measure([&] { 
            serde::deserialize<float>(toml11_float);
        });
    };
}
TEST_CASE("toml11_set_float_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_float = serde::serialize<toml::value>(float32); 
        meter.measure([&] { 
            toml::get<float>(toml11_float);
        });
    };
}
TEST_CASE("toml11_get_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_float = serde::serialize<toml::value>(float32); 
        meter.measure([&] { 
            serde::serialize<toml::value>(float32);
        });
    };
}
TEST_CASE("toml11_get_float_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_float = serde::serialize<toml::value>(float32); 
        meter.measure([&] { 
            toml::value v(float32);
        });
    };
}
TEST_CASE("yaml_cpp_set_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_float = serde::serialize<YAML::Node>(float32); 
        meter.measure([&] { 
            serde::deserialize<float>(yaml_cpp_float);
        });
    };
}
TEST_CASE("yaml_cpp_set_float_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_float = serde::serialize<YAML::Node>(float32); 
        meter.measure([&] { 
            yaml_cpp_float.as<float>();
        });
    };
}
TEST_CASE("yaml_cpp_get_float_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_float = serde::serialize<YAML::Node>(float32); 
        meter.measure([&] { 
            serde::serialize<YAML::Node>(float32);
        });
    };
}
TEST_CASE("yaml_cpp_get_float_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_float = serde::serialize<YAML::Node>(float32); 
        meter.measure([&] { 
            YAML::Node{float32};
        });
    };
}

std::string std_string = "hello";
TEST_CASE("nljson_set_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_string = serde::serialize<nlohmann::json>(std_string); 
        meter.measure([&] { 
            serde::deserialize<std::string>(json_string);
        });
    };
}
TEST_CASE("nljson_set_string_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_string = serde::serialize<nlohmann::json>(std_string); 
        meter.measure([&] { 
            json_string.get<std::string>();
        });
    };
}
TEST_CASE("nljson_get_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_string = serde::serialize<nlohmann::json>(std_string); 
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(std_string);
        });
    };
}
TEST_CASE("nljson_get_string_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_string = serde::serialize<nlohmann::json>(std_string); 
        meter.measure([&] { 
            nlohmann::json{std_string};
        });
    };
}

TEST_CASE("toml11_set_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_string = serde::serialize<toml::value>(std_string); 
        meter.measure([&] { 
            serde::deserialize<std::string>(toml11_string);
        });
    };
}
TEST_CASE("toml11_set_string_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_string = serde::serialize<toml::value>(std_string); 
        meter.measure([&] { 
            toml::get<std::string>(toml11_string);
        });
    };
}
TEST_CASE("toml11_get_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_string = serde::serialize<toml::value>(std_string); 
        meter.measure([&] { 
            serde::serialize<toml::value>(std_string);
        });
    };
}
TEST_CASE("toml11_get_string_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_string = serde::serialize<toml::value>(std_string); 
        meter.measure([&] { 
            toml::value v(std_string);
        });
    };
}

TEST_CASE("yaml_cpp_set_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_string = serde::serialize<YAML::Node>(std_string); 
        meter.measure([&] { 
            serde::deserialize<std::string>(yaml_cpp_string);
        });
    };
}
TEST_CASE("yaml_cpp_set_string_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_string = serde::serialize<YAML::Node>(std_string); 
        meter.measure([&] { 
            yaml_cpp_string.as<std::string>();
        });
    };
}
TEST_CASE("yaml_cpp_get_string_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_string = serde::serialize<YAML::Node>(std_string); 
        meter.measure([&] { 
            serde::serialize<YAML::Node>(std_string); 
        });
    };
}
TEST_CASE("yaml_cpp_get_string_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_string = serde::serialize<YAML::Node>(std_string); 
        meter.measure([&] { 
            YAML::Node{std_string}; 
        });
    };
}

std::vector<int> std_vector{1,2,3,4,5,6,7,8,9,10};
TEST_CASE("nljson_set_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_vector = serde::serialize<nlohmann::json>(std_vector); 
        meter.measure([&] { 
            serde::deserialize<std::vector<int>>(json_vector); 
        });
    };
}
TEST_CASE("nljson_set_vector_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_vector = serde::serialize<nlohmann::json>(std_vector); 
        meter.measure([&] { 
            json_vector.get<std::vector<int>>(); 
        });
    };
}
TEST_CASE("nljson_get_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_vector = serde::serialize<nlohmann::json>(std_vector); 
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(std_vector); 
        });
    };
}
TEST_CASE("nljson_get_vector_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_vector = serde::serialize<nlohmann::json>(std_vector); 
        meter.measure([&] { 
            nlohmann::json{std_vector}; 
        });
    };
}

TEST_CASE("toml11_set_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_vector = serde::serialize<toml::value>(std_vector); 
        meter.measure([&] { 
            serde::deserialize<std::vector<int>>(toml11_vector); 
        });
    };
}
TEST_CASE("toml11_set_vector_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_vector = serde::serialize<toml::value>(std_vector); 
        meter.measure([&] { 
            toml::get<std::vector<int>>(toml11_vector); 
        });
    };
}
TEST_CASE("toml11_get_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_vector = serde::serialize<toml::value>(std_vector); 
        meter.measure([&] { 
            serde::serialize<toml::value>(std_vector); 
        });
    };
}
TEST_CASE("toml11_get_vector_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_vector = serde::serialize<toml::value>(std_vector); 
        meter.measure([&] { 
            toml::value v(std_vector); 
        });
    };
}

TEST_CASE("yaml_cpp_set_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_vector = serde::serialize<YAML::Node>(std_vector); 
        meter.measure([&] { 
            serde::deserialize<std::vector<int>>(yaml_cpp_vector); 
        });
    };
}
TEST_CASE("yaml_cpp_set_vector_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_vector = serde::serialize<YAML::Node>(std_vector); 
        meter.measure([&] { 
            std::vector<int> vec;
            for (auto i = 0; i < yaml_cpp_vector.size(); ++i) 
                vec.push_back(yaml_cpp_vector[i].as<int>());
        });
    };
} 
TEST_CASE("yaml_cpp_get_vector_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_vector = serde::serialize<YAML::Node>(std_vector); 
        meter.measure([&] { 
            serde::serialize<YAML::Node>(std_vector); 
        });
    };
}
TEST_CASE("yaml_cpp_get_vector_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto yaml_cpp_vector = serde::serialize<YAML::Node>(std_vector); 
        meter.measure([&] { 
            YAML::Node{std_vector}; 
        });
    };
}

std::map<std::string, std::string> std_map{{"hello", "h"}, {"hello2", "h2"}, {"hello3", "h3"}};

TEST_CASE("nljson_set_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_map = serde::serialize<nlohmann::json>(std_map); 
        meter.measure([&] { 
            serde::deserialize<std::map<std::string, std::string>>(json_map); 
        });
    };
}
TEST_CASE("nljson_set_map_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_map = serde::serialize<nlohmann::json>(std_map); 
        meter.measure([&] { 
            json_map.get<std::map<std::string, std::string>>(); 
        });
    };
}
TEST_CASE("nljson_get_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_map = serde::serialize<nlohmann::json>(std_map); 
        meter.measure([&] { 
            serde::serialize<nlohmann::json>(std_map); 
        });
    };
}
TEST_CASE("nljson_get_map_nl_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto json_map = serde::serialize<nlohmann::json>(std_map); 
        meter.measure([&] { 
            nlohmann::json{std_map}; 
        });
    };
}

TEST_CASE("toml11_set_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_map = serde::serialize<toml::value>(std_map); 
        meter.measure([&] { 
            serde::deserialize<std::map<std::string, std::string>>(toml11_map); 
        });
    };
}
TEST_CASE("toml11_set_map_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        auto toml11_map = serde::serialize<toml::value>(std_map); 
        meter.measure([&] { 
            toml::get<std::map<std::string, std::string>>(toml11_map); 
        });
    };
}
TEST_CASE("toml11_get_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            serde::serialize<toml::value>(std_map); 
        });
    };
}
TEST_CASE("toml11_get_map_tm_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            toml::value v(std_map); 
        });
    };
}
auto yaml_cpp_map = serde::serialize<YAML::Node>(std_map); 

TEST_CASE("yaml_cpp_set_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            serde::deserialize<std::map<std::string, std::string>>(yaml_cpp_map); 
        });
    };
}
TEST_CASE("yaml_cpp_set_map_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            std::map<std::string, std::string> map;
            for(YAML::const_iterator it = yaml_cpp_map.begin(); it!=yaml_cpp_map.end(); ++it) {
                auto key_ = it->first, value_ = it->second;
                map[key_.as<std::string>()] = value_.as<std::string>();
            }
        });
    };
}
TEST_CASE("yaml_cpp_get_map_se_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            serde::serialize<YAML::Node>(std_map); 
        });
    };
}
TEST_CASE("yaml_cpp_get_map_ym_bench") {
    BENCHMARK_ADVANCED("serdepp")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] { 
            YAML::Node{std_map}; 
        });
    };
}


