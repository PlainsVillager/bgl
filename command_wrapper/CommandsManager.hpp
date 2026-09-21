// CommandsManager.hpp
// Author: PlainsVillager
// Date: 2026/9/17
// License: MIT
// Module: Command Wrapper
//
//

#include "Command.hpp"
// #include <vector>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cmdwpr {
class CommandsManager {
public:
    void invoke(); // parse from stdin and invoke a command handler

private:
    // std::vector<Command> commands;
    // std::unordered_map<Command, std::function<std::vector<std::string>>> commands;
};
}
