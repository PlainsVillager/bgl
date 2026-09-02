#include "include/Launcher.h"

int main() {
    bgl::Launcher launcher = bgl::Launcher::getSingleton();
    launcher.start();
    return 0;
}
