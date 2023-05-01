#include <gtest/gtest.h>
#include "bmstu_vector.h"
#include <string>
#include <vector>

struct NoDefaultConstructable {
    int value = 0;

    explicit NoDefaultConstructable(int value) : value(value) {}

    NoDefaultConstructable() = delete;

    friend bool operator==(NoDefaultConstructable l, NoDefaultConstructable r) {
        return l.value == r.value;
    }

    ~NoDefaultConstructable() = default;

    int get_value() const {
        return value;
    }
};

struct NoCopyConstructable {
    int value = 0;

    explicit NoCopyConstructable(int value) : value(value) {}

    NoCopyConstructable() = delete;

    NoCopyConstructable(NoCopyConstructable &other) = delete;

    NoCopyConstructable &operator=(const NoCopyConstructable &other) = delete;

    NoCopyConstructable &operator=(NoCopyConstructable &&other) = default;

    NoCopyConstructable(NoCopyConstructable &&other) = default;

    friend bool operator==(NoCopyConstructable l, NoCopyConstructable r) {
        return l.value == r.value;
    }

    ~NoCopyConstructable() {}

    int get_value() const {
        return value;
    }
};

struct NoMoveConstructable {
    int value = 0;

    explicit NoMoveConstructable(int value) : value(value) {}

    NoMoveConstructable() = delete;

    NoMoveConstructable(NoMoveConstructable &other) = default;

    NoMoveConstructable &operator=(const NoMoveConstructable &other) = default;

    NoMoveConstructable &operator=(NoMoveConstructable &&other) = delete;

    NoMoveConstructable(NoMoveConstructable &&other) = delete;

    friend bool operator==(NoMoveConstructable &l, NoMoveConstructable &r) {
        return l.value == r.value;
    }

    ~NoMoveConstructable() {}

    int get_value() const {
        return value;
    }
};

template<typename T>
void elem_check(bmstu::vector<T> &vec, const T &value = T{}) {
    for (size_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], value);
    }
}

TEST(DefaultConstruct, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec;
    ASSERT_TRUE(vec.empty());
    ASSERT_EQ(vec.capacity(), 0);
}

TEST(DefaultConstruct, Strings) {
    bmstu::vector<std::string> vec;
    ASSERT_TRUE(vec.empty());
    ASSERT_EQ(vec.capacity(), 0);
}

TEST(SizeConstructor, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec(10);
    ASSERT_EQ(vec.size(), 10);
    for (size_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i].value, 0);
    }
}

TEST(SizeConstructor, Integer) {
    bmstu::vector<int> vec(10);
    ASSERT_EQ(vec.size(), 10);
    elem_check(vec, 0);
}

TEST(SizeConstructor, Strings) {
    bmstu::vector<std::string> vec(10);
    ASSERT_EQ(vec.size(), 10);
    for (size_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], "");
    }
}

TEST(InitializerListConstructor, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(666),
                                           NoMoveConstructable(777)};
    ASSERT_EQ(vec[0].get_value(), 1);
    ASSERT_EQ(vec[1].get_value(), 666);
    ASSERT_EQ(vec[2].get_value(), 777);
}

TEST(InitializerListConstructor, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (size_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST(InitializerListConstructor, Strings) {
    bmstu::vector<std::string> vec{"Я", "умный вектор)"};
    ASSERT_EQ(vec[0], "Я");
    ASSERT_EQ(vec[1], "умный вектор)");
}

TEST(CopyConstructor, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(666),
                                           NoMoveConstructable(777)};
    bmstu::vector<NoMoveConstructable> copy(vec);
    ASSERT_EQ(vec.size(), copy.size());
    ASSERT_EQ(vec.capacity(), copy.capacity());
    for (int i = 0; i < copy.size(); ++i) {
        ASSERT_EQ(vec[i].value, copy[i].value);
    }
}

TEST(CopyConstructor, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4, 5};
    bmstu::vector<int> copy(vec);
    ASSERT_TRUE(vec == copy);
}

TEST(CopyConstructor, Strings) {
    bmstu::vector<std::string> vec{"Я", "очень", "умный вектор)"};
    bmstu::vector<std::string> copy = vec;
    ASSERT_TRUE(vec == copy);
}


TEST(MoveConstructor, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(666),
                                           NoMoveConstructable(777)};
    bmstu::vector<NoMoveConstructable> move(std::move(vec));
    ASSERT_EQ(move.size(), 3);
    ASSERT_EQ(move.capacity(), 3);
    ASSERT_EQ(move[0].get_value(), 1);
    ASSERT_EQ(move[1].get_value(), 666);
    ASSERT_EQ(move[2].get_value(), 777);
}

TEST(MoveConstructor, Integer) {
    bmstu::vector<int> vec{1, 1, 1, 1, 1};
    bmstu::vector<int> move(std::move(vec));
    ASSERT_EQ(move.size(), 5);
    elem_check(move, 1);
}

TEST(MoveConstructor, string) {
    bmstu::vector<std::string> vec{"Я наиумнейший вектор", "Я наиумнейший вектор", "Я наиумнейший вектор",
                                   "Я наиумнейший вектор", "Я наиумнейший вектор"};
    bmstu::vector<std::string> move(std::move(vec));
    std::string str = "Я наиумнейший вектор";
    ASSERT_EQ(str, str);
    elem_check(move, str);
}


TEST(CopyAssign, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(666),
                                           NoMoveConstructable(777)};
    bmstu::vector<NoMoveConstructable> copy = vec;
    ASSERT_EQ(vec.size(), copy.size());
    ASSERT_EQ(vec.capacity(), copy.capacity());
    for (int i = 0; i < copy.size(); ++i) {
        ASSERT_EQ(vec[i].value, copy[i].value);
    }
}

TEST(CopyAssign, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4, 5};
    bmstu::vector<int> copy = vec;
    ASSERT_TRUE(vec == copy);
}

TEST(CopyAssign, Strings) {
    bmstu::vector<std::string> vec{"Я", "очень", "умный вектор)"};
    bmstu::vector<std::string> copy = vec;
    ASSERT_TRUE(vec == copy);
}


TEST(MoveAssign, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(666),
                                           NoMoveConstructable(777)};
    bmstu::vector<NoMoveConstructable> move = std::move(vec);
    ASSERT_EQ(move.size(), 3);
    ASSERT_EQ(move.capacity(), 3);
    ASSERT_EQ(move[0].get_value(), 1);
    ASSERT_EQ(move[1].get_value(), 666);
    ASSERT_EQ(move[2].get_value(), 777);
}

TEST(MoveAssign, Integer) {
    bmstu::vector<int> vec{1, 1, 1, 1, 1};
    bmstu::vector<int> move = std::move(vec);
    ASSERT_EQ(move.size(), 5);
    elem_check(move, 1);
}

TEST(MoveAssign, Strings) {
    bmstu::vector<std::string> vec{"Я наиумнейший вектор", "Я наиумнейший вектор", "Я наиумнейший вектор",
                                   "Я наиумнейший вектор", "Я наиумнейший вектор"};
    bmstu::vector<std::string> move = std::move(vec);
    ASSERT_EQ(move.size(), 5);
    for (size_t i = 0; i < move.size(); ++i) {
        ASSERT_EQ(move[i], "Я наиумнейший вектор");
    }
}

TEST(Reserve, Integer) {
    bmstu::vector<int> vec{6, 6, 6};
    vec.reserve(10);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 3);
    elem_check(vec, 6);
    vec.reserve(2);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 3);
    elem_check(vec, 6);
}

TEST(Reserve, Strings) {
    bmstu::vector<std::string> vec{"Я", "ультрамега", "умный вектор!"};
    vec.reserve(10);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], "Я");
    ASSERT_EQ(vec[1], "ультрамега");
    ASSERT_EQ(vec[2], "умный вектор!");
    vec.reserve(2);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], "Я");
    ASSERT_EQ(vec[1], "ультрамега");
    ASSERT_EQ(vec[2], "умный вектор!");
}

TEST(Reserve, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(2)};
    vec.reserve(10);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0].get_value(), 1);
    ASSERT_EQ(vec[1].get_value(), 2);
    vec.reserve(2);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0].get_value(), 1);
    ASSERT_EQ(vec[1].get_value(), 2);
}

TEST(Resize, Integer) {
    bmstu::vector<int> vec{1026, 1026, 1026, 1026, 1026};
    vec.resize(1);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec.capacity(), 5);
    ASSERT_EQ(vec[0], 1026);
    vec.resize(10);
    ASSERT_EQ(vec.size(), 10);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec[0], 1026);
    for (int i = 1; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], 0);
    }
}

TEST(Resize, Strings) {
    bmstu::vector<std::string> vec{"Бебра", "Хантерс", "обязательно", "помогут", "девочкам"};
    vec.resize(1);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec.capacity(), 5);
    ASSERT_EQ(vec[0], "Бебра");
    vec.resize(10);
    ASSERT_EQ(vec.size(), 10);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec[0], "Бебра");
    for (int i = 1; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i], "");
    }
}

TEST(Resize, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    vec.resize(3);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec.capacity(), 3);
    ASSERT_EQ(vec[0].get_value(), 1);
    ASSERT_EQ(vec[1].get_value(), 2);
    ASSERT_EQ(vec[2].get_value(), 0);
    vec.resize(1);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec.capacity(), 3);
    ASSERT_EQ(vec[0].get_value(), 1);
}

TEST(PopBack, Integer) {
    bmstu::vector<int> vec{10, 10, 10};
    vec.pop_back();
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec.capacity(), 3);
    elem_check(vec, 10);
}

TEST(PopBack, Strings) {
    bmstu::vector<std::string> vec{"ИУ10", "ИУ10", "ИУ10"};
    vec.pop_back();
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec.capacity(), 3);
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = "ИУ10";
    }
}

TEST(PopBack, WithoutDefaultConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(10), NoMoveConstructable(10), NoMoveConstructable(10)};
    vec.pop_back();
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec.capacity(), 3);
    for (int i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(vec[i].get_value(), 10);
    }
}

TEST(PushBack, WhithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec{NoDefaultConstructable(1), NoDefaultConstructable(2),
                                              NoDefaultConstructable(3),
                                              NoDefaultConstructable(4)};
    vec.push_back(NoDefaultConstructable(2));
    ASSERT_EQ(vec[4].get_value(), 2);
}

TEST(PushBack, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4};
    vec.push_back(5);
    ASSERT_EQ(vec[4], 5);
}

TEST(PushBack, Strings) {
    bmstu::vector<std::string> vec{"Я", "очень", "крутой", "охотник за"};
    vec.push_back("беброй");
    ASSERT_EQ(vec[4], "беброй");
}

TEST(Incert, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vec.incert(vec.begin() + 3, 1);
    vec.incert(vec.end(), 2);
    ASSERT_EQ(vec[3], 1);
    ASSERT_EQ(vec[vec.size() - 1], 2);
}

TEST(Incert, Strings) {
    bmstu::vector<std::string> vec{"Бебра", "луДшая", "организация", "на"};
    vec.incert(vec.begin() + 1, "Хантерс");
    vec.incert(vec.end(), "гитхабе");
    ASSERT_EQ(vec[1], "Хантерс");
    ASSERT_EQ(vec[vec.size() - 1], "гитхабе");
}

TEST(Incert, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    vec.incert(vec.begin() + 1, NoDefaultConstructable(3));
    ASSERT_EQ(vec[1].get_value(), 3);
}

TEST(VecEqual, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    bmstu::vector<NoDefaultConstructable> vec2{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    ASSERT_TRUE(vec == vec2);
}

TEST(VecEqual, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(2)};
    bmstu::vector<NoMoveConstructable> vec2{NoMoveConstructable(1), NoMoveConstructable(2)};
    ASSERT_TRUE(vec == vec2);
}

TEST(VecEqual, Integer) {
    bmstu::vector<int> vec{1, 2};
    bmstu::vector<int> vec2{1, 2};
    ASSERT_TRUE(vec == vec2);
}

TEST(VecEqual, Stings) {
    bmstu::vector<std::string> vec{"С++", "лучше", "Python"};
    bmstu::vector<std::string> vec2{"С++", "лучше", "Python"};
    ASSERT_TRUE(vec == vec2);
}

TEST(NotVecEqual, WithoutDefaultConstructor) {
    bmstu::vector<NoDefaultConstructable> vec{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    bmstu::vector<NoDefaultConstructable> vec2{NoDefaultConstructable(1), NoDefaultConstructable(2)};
    ASSERT_FALSE(vec != vec2);
}

TEST(NotVecEqual, WithoutMoveConstructor) {
    bmstu::vector<NoMoveConstructable> vec{NoMoveConstructable(1), NoMoveConstructable(2)};
    bmstu::vector<NoMoveConstructable> vec2{NoMoveConstructable(1), NoMoveConstructable(2)};
    ASSERT_FALSE(vec != vec2);
}

TEST(NotVecEqual, Integer) {
    bmstu::vector<int> vec{1, 2};
    bmstu::vector<int> vec2{1, 2};
    ASSERT_FALSE(vec != vec2);
}

TEST(NotVecEqual, Stings) {
    bmstu::vector<std::string> vec{"С++", "лучше", "Python"};
    bmstu::vector<std::string> vec2{"С++", "лучше", "Python"};
    ASSERT_FALSE(vec != vec2);
}

TEST(Cout, Integer) {
    bmstu::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9};
    testing::internal::CaptureStdout();
    std::cout << vec;
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ("[1, 2, 3, 4, 5, 6, 7, 8, 9]", output);
}

TEST(Cout, Strings) {
    bmstu::vector<std::string> vec{"Платон", "Петров", "обязательно", "пройдёт", "стажировку", "в", "ВК"};
    testing::internal::CaptureStdout();
    std::cout << vec;
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ("[Платон, Петров, обязательно, пройдёт, стажировку, в, ВК]", output);
}