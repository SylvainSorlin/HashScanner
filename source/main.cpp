#include "Config.h"
#include "Scanner.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::filesystem::path iniFile = (argc > 1) ? argv[1] : "config.ini";

    try {
        Config config(iniFile);
        Scanner scanner(config);
        scanner.Run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}