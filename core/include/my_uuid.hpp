// my_uuid.hpp a simplest header-only library implementation of uuid-v4
// CXX Standard: C++ 17
// Author: plainsvillager
// Date: Sept 21 2026
// License: 100% Public Domain
#pragma once
#include <array>
#include <random>
#include <string>

namespace my_uuid::v4 {

std::array<char, 7> charset { "abcdef" };
constexpr int UUID_HEX_DIGIT_SIZE { 32 };

char dec_to_hex(int dec_val) {
    if (dec_val <= 9) {
        return static_cast<char>(dec_val + 48);
    }
    // char charset[] { "abcdef" };
    return charset[dec_val - 10];
}

struct uuid_v4 {
private:
    std::string uuid_str_value { };
    std::array<int, UUID_HEX_DIGIT_SIZE> uuid_int_value { };

public:
    uuid_v4() {
        uuid_str_value.resize(36);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 15);
        for (size_t i = 0; i < UUID_HEX_DIGIT_SIZE; ++i) {
            uuid_int_value[i] = dist(gen);
        }
        uuid_int_value[12] = 4;
        // 16
        std::uniform_int_distribution<> dist2(8, 11);
        uuid_int_value[16] = dist2(gen);
        for (size_t i = 0; i < UUID_HEX_DIGIT_SIZE; i++) {
            uuid_str_value[i] = dec_to_hex(uuid_int_value[i]);
        }
    }

    std::string to_string() {
        return uuid_str_value;
    }

    std::string to_string_with_connect() {
        std::string temp { uuid_str_value };
        temp.insert(std::cbegin(temp) + 8, '-');
        temp.insert(std::cbegin(temp) + 12 + 1, '-');
        temp.insert(std::cbegin(temp) + 16 + 2, '-');
        temp.insert(std::cbegin(temp) + 20 + 3, '-');
        return temp;
    }
};

} // namespace my_uuid::v4
