#include "minecraft/client/client.h"

int main() {

    minecraft::MinecraftClientWindow::launch();
    minecraft::MinecraftClientWindow::cleanup();

    return 0;
}
