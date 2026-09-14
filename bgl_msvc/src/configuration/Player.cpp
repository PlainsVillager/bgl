//
// Created by littl on 2026/9/13.
//
#include "configuration/Player.hpp"

namespace bgl {

Player::Player(const std::string& name, const std::string& uuid)
    : m_name(name)
    , m_uuid(uuid)
{
}

const std::string Player::getName() const { return m_name; }
const std::string Player::getUuid() const { return m_uuid; }
void Player::setName(const std::string& name) { m_name = name; }
void Player::setUuid(const std::string& uuid) { m_uuid = uuid; }

}