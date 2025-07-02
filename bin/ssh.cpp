#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h> // for MessageBoxA and SetConsoleTitleA

std::string readNonEmptyLine(const std::string& prompt) {
    std::string input;
    do {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (input.empty()) {
            MessageBoxA(NULL, "Input cannot be empty. Please try again.", "Input Error", MB_OK | MB_ICONWARNING);
        }
    } while (input.empty());
    return input;
}

void sshCommand() {
    std::string username = readNonEmptyLine("Username: ");
    std::string ip = readNonEmptyLine("IP: ");
    std::string password = readNonEmptyLine("Password: ");

    std::string saveConfigAnswer;
    std::cout << "Save config? (y/n): ";
    std::getline(std::cin, saveConfigAnswer);

    if (saveConfigAnswer == "y" || saveConfigAnswer == "Y") {
        std::string configPath = "sshconfig.txt";
        FILE* f = fopen(configPath.c_str(), "a");
        if (f) {
            fprintf(f, "%s@%s\n", username.c_str(), ip.c_str());
            fclose(f);
            std::cout << "Config saved to " << configPath << "\n";
        } else {
            std::cout << "Failed to save config.\n";
        }
    }

    std::string cmd = "bin\\sshpass.exe -ssh -pw " + password + " " + username + "@" + ip; // The sshpass.exe thing is just plink.exe renamed!!!

    std::cout << "Connecting...\n";

    int result = system(cmd.c_str());

    if (result == -1) {
        std::cout << "Failed to start ssh client.\n";
    }
}

int main() {
    system("title FastBox SSH Client");
    sshCommand();

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}

#ifdef _WIN32
extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main();
}
#endif
