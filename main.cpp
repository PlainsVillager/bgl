#include "include/Launcher.h"

int main() {
    // std::cout << "Hello, World!" << std::endl;
    // std::cout << __GNUC__ << '.' << __GNUC_MINOR__ <<'.'<<__GNUC_PATCHLEVEL__<< '\n';
    bgl::Launcher launcher = bgl::Launcher::getSingleton();
    launcher.start();
    return 0;
}
