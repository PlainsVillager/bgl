#include "include/Launcher.h"

int debug()
{
    int a = 0;
    return 1 / a;
}

int main()
{
    std::ios::sync_with_stdio(false);
    bgl::Launcher::getSingleton().startLoop();
    // debug();
    return 0;
}
