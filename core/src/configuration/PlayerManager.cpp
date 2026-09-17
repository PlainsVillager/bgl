//
// Created by littl on 2026/9/13.
//
#include "configuration/PlayerManager.hpp"
#include "Utility.h"
#include "configuration/Player.hpp"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace bgl {
PlayerManager& PlayerManager::getPlayerManagerSingleton()
{
    static PlayerManager playerManager;
    return playerManager;
}

PlayerManager::PlayerManager()
    : m_players()
{
}

ActionResult PlayerManager::add(const std::string& name, const std::string& uuid)
{
    std::vector<std::string> player_names { };
    player_names.reserve(m_players.size());
    std::ranges::transform(
        m_players,
        std::back_inserter(player_names),
        [](Player& player) { return player.getName(); });
    if (std::ranges::find(player_names, name) != std::end(player_names)) { // name already exists
        return ActionResult::FAIL;
    } else {
        m_players.emplace_back(name, uuid);
        return ActionResult::SUCCESS;
    }
}
ActionResult PlayerManager::remove(const std::string& name)
{
    for (std::size_t i { }; i < m_players.size(); ++i) {
        if (m_players[i].getName() == name) {
            m_players.erase(std::begin(m_players) + i); // NOLINT
            return ActionResult::SUCCESS;
        }
    }
    return ActionResult::FAIL;
}
ActionResult PlayerManager::save()
{
    std::ofstream ofs("players.txt", std::ios::binary);
    if (!ofs.is_open())
        return ActionResult::FAIL;
    for (auto& player : m_players) {
        ofs << std::string { player.getName() + ' ' + player.getUuid() + '\n' };
    }
    ofs.close();
    return ActionResult::SUCCESS;
}
ActionResult PlayerManager::load()
{
    m_players.clear();
    std::ifstream ifs { "players.txt" };

    if (!ifs.is_open()) {
        return ActionResult::FAIL;
    }

    std::string line;

    while (std::getline(ifs, line)) {
        auto blankPos { line.find_first_of(' ') };
        if (blankPos == std::string::npos) {
            throw std::logic_error { "players.txt format inproperly" };
            return ActionResult::FAIL;
        }
        std::string name = line.substr(0, blankPos);
        std::string uuid = line.substr(blankPos + 1);
        this->add(name, uuid);
    }
    ifs.close();
    return ActionResult::SUCCESS;
}

const std::vector<Player>& PlayerManager::listPlayers()
{
    return m_players;
}

} // namespace bgl
