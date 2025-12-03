
#include <rttr/registration>
#include <rttr/type>

#include <serdepp/extend/rttr/fmt.hpp>
#include <serdepp/extend/rttr/nlohmann_json.hpp>
#include <serdepp/extend/rttr/rapidjson.hpp>
#include <serdepp/extend/rttr/toml11.hpp>
#include <serdepp/extend/rttr/yaml-cpp.hpp>

#include <string>
#include <vector>
#include <map>

struct Person {
public:
    Person() {}
    Person(int id, std::string_view name) :id(id), name(name) {}

    int id{};
    std::string name{};
    bool operator<(const Person& other) const {
        // 使用std::tie简化比较逻辑
        return std::tie(id, name) < std::tie(other.id, other.name);
    }
};

struct Base {
public:
    int id{};

    RTTR_ENABLE()
};

struct Vector_Prop : Base {
public:
    Vector_Prop() {}
    Vector_Prop(std::vector<Person>&& _persons) 
            : persons(std::forward<std::vector<Person>>(_persons)) {}

    std::vector<Person> persons;

    RTTR_ENABLE(Base)
};

struct Map_Prop : Base {
public:
    Map_Prop() {}
    Map_Prop(
            std::map<int, Person> personById, 
            std::map<std::string, Person> personByStr, 
            std::map<Person, Person> personByP
        ) : personById(personById), 
            personByStr(personByStr), 
            personByP(personByP) {}

    std::map<int, Person> personById;
    std::map<std::string, Person> personByStr;
    std::map<Person, Person> personByP;

    RTTR_ENABLE(Base)
};

struct Set_Prop : Base {
public:
    Set_Prop() {}
    Set_Prop(
            std::set<int> intSet, 
            std::set<std::string> strSet, 
            std::set<Person> pSet
        ) : intSet(intSet), 
            strSet(strSet), 
            pSet(pSet) {}

    std::set<int> intSet;
    std::set<std::string> strSet;
    std::set<Person> pSet;

    RTTR_ENABLE(Base)
};

enum Color {
    None,
    Red, 
    Blue, 
    Green,
};

struct Enum_Prop : Base {
public:
    Enum_Prop() {}
    Enum_Prop(
            Color background,
            std::vector<Color> pens,
            std::map<Color, std::string> namesByPens,
            std::set<Color> penset
            ) 
        : background(background), 
          pens(pens), 
          namesByPens(namesByPens), 
          penset(penset) {}

    Color background;
    std::vector<Color> pens;
    std::map<Color, std::string> namesByPens;
    std::set<Color> penset;

    RTTR_ENABLE(Base)
};

class poly_base1 {
public:
    poly_base1() {}
    poly_base1(const poly_base1& other) : val(other.val) {}
    poly_base1(poly_base1&& other) : val(other.val) {}
    poly_base1& operator=(const poly_base1& other) {
        if (this == (void*)&other) {
            return *this;
        }
        val = other.val;
        return *this;
    }
    poly_base1& operator=(poly_base1&& other) {
        if (this == (void*)&other) {
            return *this;
        }
        val = other.val;
        return *this;
    }

    poly_base1(int val) : val(val) {}

    int val{};
    RTTR_ENABLE()
};

class poly_sub1 : public poly_base1 {
public:
    poly_sub1() : poly_base1() { }
    poly_sub1(const poly_sub1& other) : poly_base1(other) {
        sub_val = other.sub_val;
    }
    poly_sub1(poly_sub1&& other) {
        val = other.val;
        sub_val = other.sub_val;
    }
    poly_sub1& operator=(const poly_sub1& other) {
        if (this == (void*)&other) {
            return *this;
        }
        val = other.val;
        sub_val = other.sub_val;
        return *this;
    }
    poly_sub1& operator=(poly_sub1&& other) {
        if (this == (void*)&other) {
            return *this;
        }
        val = other.val;
        sub_val = other.sub_val;
        return *this;
    }

    poly_sub1(int val) {
        this->val = val;
    }

    poly_sub1(int val, int sub_val) {
        this->val = val;
        this->sub_val = sub_val;
    }

    int sub_val{};
    RTTR_ENABLE(poly_base1)
};

class poly_ptr {
public:
    poly_ptr() = default;
    poly_ptr(const poly_ptr&) = default;
    poly_ptr(poly_ptr&&) = default;
    poly_ptr& operator=(const poly_ptr&) = default;
    poly_ptr& operator=(poly_ptr&&) = default;

    poly_ptr(rttr::polymoph_ptr<poly_base1> value_ptr) {
        this->value_ptr = value_ptr;
    }

    rttr::polymoph_ptr<poly_base1> value_ptr;
};

using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<Person>("Person")
         .constructor<>() 
         .property("id", &Person::id)
         .property("name", &Person::name);

    registration::class_<Base>("Base")
         .constructor<>()
         .property("id", &Base::id);

    registration::class_<Vector_Prop>("Vector_Prop")
         .constructor<>()
         .property("persons", &Vector_Prop::persons);

    registration::class_<Map_Prop>("Map_Prop")
         .constructor<>()
         .property("personById", &Map_Prop::personById)
         .property("personByStr", &Map_Prop::personByStr)
         .property("personByP", &Map_Prop::personByP);

    registration::class_<Set_Prop>("Set_Prop")
         .constructor<>()
         .property("intSet", &Set_Prop::intSet)
         .property("strSet", &Set_Prop::strSet)
         .property("pSet", &Set_Prop::pSet)
         ;

    registration::enumeration<Color>("Color")
        (
            value("Red", Color::Red),
            value("Blue", Color::Blue),
            value("Green", Color::Green)
        );

    registration::class_<Enum_Prop>("Enum_Prop")
        .constructor<>()
        .property("background", &Enum_Prop::background)
        .property("pens", &Enum_Prop::pens)
        .property("namesByPens", &Enum_Prop::namesByPens);

    registration::class_<poly_base1>("poly_base1")
        .constructor<>()
        .constructor<int>()
        .property("val", &poly_base1::val);

    registration::class_<poly_sub1>("poly_sub1")
        .constructor<>()
        .constructor<int>()
        .constructor<int, int>()
        .property("sub_val", &poly_sub1::sub_val);

    rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<poly_sub1>>();

    registration::class_<poly_ptr>("poly_ptr")
        .constructor<>()
        .constructor<rttr::polymoph_ptr<poly_base1>>()
        .property("value_ptr", &poly_ptr::value_ptr);
}

template<typename Adapter>
void test_serialize() {

    Person p{ 1001, "person1" };

    Vector_Prop vp{
        {
            {1002, "person2"}, 
            {1003, "person3"},
        }
    };

    Map_Prop mp{
        {
            {1004, {1004, "person4"}},
            {1005, {1005, "person5"}},
        },
        {
            {"person4", {1004, "person4-1"}},
            {"person5", {1005, "person5-1"}},
        },
        {
            {{1006, "person6"}, {1007, "person7"}},
            {{1008, "person8"}, {1009, "person9"}},
        },
    };

    Set_Prop sp{
        {1, 2, 3, 4, 5},
        {"s1", "s2", "字符串3"},
        {{2001, "person2001"}, {2002, "person2002"}},
    };

    Enum_Prop ep{
        Color::Red,
        {
            Color::Red,
            Color::Blue,
            Color::Green,
        },
        {
            {Color::Red, "red"},
            {Color::Blue, "blue"},
            {Color::Green, "green"},
        },
        {
            Color::Red,
            Color::Blue,
        },
    };

    poly_ptr pp{
        rttr::polymoph_ptr<poly_base1>(poly_sub1(20, 21)),
    };

    rttr::variant var1_1 = p;
    rttr::variant var1_2 = vp;
    rttr::variant var1_3 = mp;
    rttr::variant var1_4 = sp;
    rttr::variant var1_5 = ep;
    rttr::variant var1_6 = pp;

    {
        Adapter adapter1_1 = serde::serialize<Adapter>(var1_1);
        Adapter adapter1_2 = serde::serialize<Adapter>(var1_2);
        Adapter adapter1_3 = serde::serialize<Adapter>(var1_3);
        Adapter adapter1_4 = serde::serialize<Adapter>(var1_4);
        Adapter adapter1_5 = serde::serialize<Adapter>(var1_5);
        Adapter adapter1_6 = serde::serialize<Adapter>(var1_6);
        fmt::print("-------------------------\n");
        fmt::print("adapter1_1 : {}\n", serde::to_string(adapter1_1));
        fmt::print("-------------------------\n");
        fmt::print("adapter1_2 : {}\n", serde::to_string(adapter1_2));
        fmt::print("-------------------------\n");
        fmt::print("adapter1_3 : {}\n", serde::to_string(adapter1_3));
        fmt::print("-------------------------\n");
        fmt::print("adapter1_4 : {}\n", serde::to_string(adapter1_4));
        fmt::print("-------------------------\n");
        fmt::print("adapter1_5 : {}\n", serde::to_string(adapter1_5));
        fmt::print("-------------------------\n");
        fmt::print("adapter1_6 : {}\n", serde::to_string(adapter1_6));
        std::fflush(stdout);  // 强制刷新输出缓冲区

        rttr::variant var2_1 = rttr::type::get<Person>().create();
        rttr::variant var2_2 = rttr::type::get<Vector_Prop>().create();
        rttr::variant var2_3 = rttr::type::get<Map_Prop>().create();
        rttr::variant var2_4 = rttr::type::get<Set_Prop>().create();
        rttr::variant var2_5 = rttr::type::get<Enum_Prop>().create();
        rttr::variant var2_6 = rttr::type::get<poly_ptr>().create();

        auto ep1 = var2_4.get_value<std::shared_ptr<Enum_Prop>>();

        serde::deserialize_to(adapter1_1, var2_1);
        fmt::print("-------------------------\n");
        fmt::print("var2_1 : {}\n", var2_1);
        std::fflush(stdout);  // 强制刷新输出缓冲区

        serde::deserialize_to(adapter1_2, var2_2);
        fmt::print("-------------------------\n");
        fmt::print("var2_2 : {}\n", var2_2);
        std::fflush(stdout);  // 强制刷新输出缓冲区

        serde::deserialize_to(adapter1_3, var2_3);
        fmt::print("-------------------------\n");
        fmt::print("var2_3 : {}\n", var2_3);
        std::fflush(stdout);  // 强制刷新输出缓冲区

        serde::deserialize_to(adapter1_4, var2_4);
        fmt::print("-------------------------\n");
        fmt::print("var2_4 : {}\n", var2_4);
        std::fflush(stdout);  // 强制刷新输出缓冲区

        serde::deserialize_to(adapter1_5, var2_5);
        fmt::print("-------------------------\n");
        fmt::print("var2_5 : {}\n", var2_5);
        std::fflush(stdout);  // 强制刷新输出缓冲区

        serde::deserialize_to(adapter1_6, var2_6);
        fmt::print("-------------------------\n");
        fmt::print("var2_6 : {}\n", var2_6);
        std::fflush(stdout);  // 强制刷新输出缓冲区
    }

}

int main(int argc, char *argv[])
{
    //nlohmann::json json_v;

    if (true) {
        fmt::print("############# nlohmann::json ############\n");
        using Adapter = nlohmann::json;
        test_serialize<Adapter>();
    }

    if (true) {
        fmt::print("############# rapidjson ############\n");
        using Adapter = rapidjson::Document;
        test_serialize<Adapter>();
    }

    if (true) {
        fmt::println("############# toml11 ############");
        using Adapter = toml::value;
        test_serialize<Adapter>();
    }
    
    if (true) {
        fmt::println("############# yaml ############");
        using Adapter = YAML::Node;
        test_serialize<Adapter>();
    }
    return 0;
}