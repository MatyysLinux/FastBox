#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <filesystem>
#include <conio.h> // _getch

namespace fs = std::filesystem;

std::string currentDir = fs::current_path().string();

// Autocomplete helper
std::vector<std::string> getMatchingDirectories(const std::string& prefix) {
    std::vector<std::string> matches;
    for (const auto& entry : fs::directory_iterator(currentDir)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name.find(prefix) == 0) {
                matches.push_back(name);
            }
        }
    }
    return matches;
}

void processCommand() {
    std::string buffer;

    while (true) {
        std::cout << currentDir << ">> ";
        buffer.clear();

        while (true) {
            char ch = _getch();

            if (ch == '\r') { // ENTER
                std::cout << "\n";
                break;
            } else if (ch == '\b') { // BACKSPACE
                if (!buffer.empty()) {
                    buffer.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch == '\t') { // TAB (autocomplete)
                if (buffer.rfind("cd ", 0) == 0) {
                    std::string partial = buffer.substr(3);
                    auto matches = getMatchingDirectories(partial);
                    if (!matches.empty()) {
                        buffer = "cd " + matches[0];
                        // Přepiš aktuální řádek
                        std::cout << "\r" << currentDir << ">> " << buffer << "    ";
                    }
                }
            } else {
                buffer += ch;
                std::cout << ch;
            }
        }

        const std::string& command = buffer;

        if (command == "help") {
            std::cout << "The commands are:\n";
            std::cout << "print 'example' - Prints out text\n";
            std::cout << "exec 'cmd'     - Executes a command\n";
            std::cout << "cd DIR         - Changes directory\n";
            std::cout << "dir / ls       - Lists current directory\n";
            std::cout << "nano / mkfile  - Runs text editor\n";
            std::cout << "cls / clear    - Clears the screen\n";
            std::cout << "ssh            - Runs ssh client\n";
            std::cout << "exit           - Quits the program\n";
        }
        else if (command == "exit") {
            std::cout << "Exiting...\n";
            break;
        }
        else if (command == "cls" || command == "clear") {
            system("cls");
        }
        else if (command == "dir" || command == "ls") {
            std::string cmd = "dir \"" + currentDir + "\"";
            system(cmd.c_str());
        }
        else if (command == "nano" || command == "mkfile") {
            int result = system("bin\\nano.exe");
            system("cls"); // Clear screen after nano
            SetConsoleTitleA("FastBox");
            if (result == -1) std::cout << "Failed to start nano.\n";
        }
        else if (command == "ssh") {
            int result = system("bin\\ssh.exe");
            if (result == -1) std::cout << "Failed to start ssh client.\n";
            system("cls"); // Clear screen after ssh client closes
            SetConsoleTitleA("FastBox");
        }
        else if (command.rfind("cd ", 0) == 0) {
            std::string path = command.substr(3);
            fs::path newPath = currentDir;
            newPath /= path;
            if (fs::exists(newPath) && fs::is_directory(newPath)) {
                currentDir = fs::canonical(newPath).string();
                SetCurrentDirectoryA(currentDir.c_str());
            } else {
                std::cout << "Directory not found: " << path << "\n";
            }
        }
        else if (command.rfind("print ", 0) == 0) {
            size_t first = command.find('\'');
            size_t last = command.rfind('\'');
            if (first != std::string::npos && last != std::string::npos && first != last) {
                std::string msg = command.substr(first + 1, last - first - 1);
                std::cout << msg << "\n";
            } else {
                std::cout << "Error: Missing quotes.\n";
            }
        }
        else if (command.rfind("exec ", 0) == 0) {
            size_t first = command.find('\'');
            size_t last = command.rfind('\'');
            if (first != std::string::npos && last != std::string::npos && first != last) {
                std::string cmd = command.substr(first + 1, last - first - 1);
                int result = system(cmd.c_str());
                system("cls"); // Clear after external command
                SetConsoleTitleA("FastBox");                
                if (result == -1) std::cout << "Failed to run.\n";
            } else {
                std::cout << "Error: Missing quotes.\n";
            }
        }
        else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
}

int main() {
    system("cls");
    SetConsoleTitleA("FastBox");
    std::cout << "Welcome to FastBox - Version 1.0\n";
    std::cout << "Make your own FastBox! Download source code at: https://www.github.com/MatyysLinux/FastBox\n";
    std::cout << "Owned by MatyysLinux!\n\n";
    SetConsoleTitleA("FastBox");
    processCommand();
    SetConsoleTitleA("FastBox");
    return 0;
}
