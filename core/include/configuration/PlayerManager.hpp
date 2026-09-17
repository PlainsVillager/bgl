//
// Created by littl on 2026/9/13.
//
#ifndef BGL_CONFIG_PLAYER_MANAGER_HPP
#define BGL_CONFIG_PLAYER_MANAGER_HPP

#include "Player.hpp"
#include "Utility.h"
#include "configuration/Player.hpp"
#include <string>
#include <vector>

namespace bgl {

// Registered players manager
class PlayerManager {
public:
    static PlayerManager& getPlayerManagerSingleton();

    ~PlayerManager() = default;
    PlayerManager(const PlayerManager& other) = delete;
    PlayerManager(PlayerManager&& other) noexcept = delete;
    PlayerManager& operator=(const PlayerManager& other) = delete;
    PlayerManager& operator=(PlayerManager&& other) = delete;

    ActionResult add(const std::string& name, const std::string& uuid);
    ActionResult remove(const std::string& name);
    ActionResult save();
    ActionResult load();

    const std::vector<Player>& listPlayers();

private:
    std::vector<Player> m_players;
    PlayerManager();
};

} // namespace bgl
#endif