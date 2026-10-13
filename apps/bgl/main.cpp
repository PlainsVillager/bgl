#include "core/Launcher.h"

int main() {
    std::ios::sync_with_stdio(false);
    bgl::Launcher::getSingleton().startLoop();
    return 0;
}
