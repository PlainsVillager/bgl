//
// Created by littl on 2026/9/13.
//

#ifndef BGL_CONFIG_PLAYER_HPP
#define BGL_CONFIG_PLAYER_HPP
#include <string>

namespace bgl {
class Player {
public:
    explicit Player(const std::string& name, const std::string& uuid);

    // ~Player() = default;
    // Player(const Player& other) = delete;
    // Player(Player&& other) noexcept = delete;
    // Player& operator=(const Player& other) = delete;
    // Player& operator=(Player&& other) = delete;

    [[nodiscard]] const std::string getName() const;
    [[nodiscard]] const std::string getUuid() const;
    void setName(const std::string& name);
    void setUuid(const std::string& uuid);

private:
    std::string m_name;
    std::string m_uuid;
};
} // namespace bgl

#endif // BGL_CONFIG_PLAYER_HPP