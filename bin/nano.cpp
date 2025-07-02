#include <iostream>
#include <vector>
#include <conio.h>
#include <string>
#include <windows.h>
#include <fstream>

// Nastaví pozici kurzoru v konzoli
void setCursorPosition(short x, short y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x, y };
    SetConsoleCursorPosition(hConsole, pos);
}

// Vymaže konzoli efektivně bez blikání
void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    DWORD charsWritten;
    COORD homeCoord = {0, 0};
    FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, homeCoord, &charsWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, homeCoord, &charsWritten);
    SetConsoleCursorPosition(hConsole, homeCoord);
}

// Zobrazí informační popup
void showMessage(const std::string& text, const std::string& title = "Info") {
    MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

// Zobrazí chybový popup
void showError(const std::string& text, const std::string& title = "Error") {
    MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}

// Otevře dialog pro uložení souboru a vrátí cestu, nebo "" pokud uživatel zruší
std::string getSaveFileName() {
    char filename[MAX_PATH] = "";
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "txt";

    if (GetSaveFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
}

// Otevře dialog pro načtení souboru a vrátí cestu, nebo "" pokud uživatel zruší
std::string getOpenFileName() {
    char filename[MAX_PATH] = "";
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
}

// Uloží obsah lines do souboru
void saveToFile(const std::vector<std::string>& lines) {
    std::string filename = getSaveFileName();
    if (filename.empty()) {
        showMessage("Saving stopped.");
        return;
    }
    std::ofstream ofs(filename);
    if (!ofs) {
        showError("Error while opening file.");
        return;
    }
    for (const auto& line : lines) {
        ofs << line << "\n";
    }
    ofs.close();
    showMessage("File has been saved: " + filename);
}

// Načte obsah souboru do lines
bool loadFromFile(std::vector<std::string>& lines, int& curLine, int& curPos) {
    std::string filename = getOpenFileName();
    if (filename.empty()) {
        showMessage("Loading stopped.");
        return false;
    }
    std::ifstream ifs(filename);
    if (!ifs) {
        showError("Loading failed. Cant open file.");
        return false;
    }
    lines.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        lines.push_back(line);
    }
    if (lines.empty()) lines.push_back("");
    curLine = 0;
    curPos = 0;
    ifs.close();
    showMessage("File loaded: " + filename);
    return true;
}

// Vykreslí všechny řádky s čísly řádků a kurzor na správném místě
void renderText(const std::vector<std::string>& lines, int curLine, int curPos) {
    clearScreen();

    // Hlavička
    std::cout << "FastBox Text Editor (Ctrl+S save, Ctrl+O open, ESC exit)\n";
    std::cout << "-----------------------------------------------------------\n";

    int lineNumberWidth = std::to_string(lines.size()).length();

    for (size_t i = 0; i < lines.size(); ++i) {
        // Formátuj číslo řádku na pevnou šířku (vpravo zarovnané)
        std::cout.width(lineNumberWidth);
        std::cout << (i + 1) << ". " << lines[i] << "\n";
    }

    // Nastav kurzor na správnou pozici (za číslo řádku + ". " + posun v textu)
    int cursorX = lineNumberWidth + 2 + curPos;
    int cursorY = 2 + (SHORT)curLine;

    setCursorPosition((SHORT)cursorX, (SHORT)cursorY);
}

// Editor s klávesnicí a ovládáním
void runEditor() {
    std::vector<std::string> lines = {""};
    int curLine = 0;
    int curPos = 0;

    renderText(lines, curLine, curPos);

    while (true) {
        SHORT key = _getch();

        if (key == 27) { // ESC - konec
            break;
        }
        else if (key == 19) { // Ctrl+S - Uložit
            saveToFile(lines);
            // Po uložení vyčisti text a začni nový soubor
            lines = {""};
            curLine = 0;
            curPos = 0;
            renderText(lines, curLine, curPos);
        }
        else if (key == 15) { // Ctrl+O - Otevřít
            if (loadFromFile(lines, curLine, curPos)) {
                renderText(lines, curLine, curPos);
            }
        }
        else if (key == 13) { // Enter
            std::string currentLine = lines[curLine];
            std::string newLine = currentLine.substr(curPos);
            lines[curLine] = currentLine.substr(0, curPos);
            lines.insert(lines.begin() + curLine + 1, newLine);
            curLine++;
            curPos = 0;
            renderText(lines, curLine, curPos);
        }
        else if (key == 8) { // Backspace
            if (curPos > 0) {
                lines[curLine].erase(curPos - 1, 1);
                curPos--;
            }
            else if (curLine > 0) {
                curPos = (int)lines[curLine - 1].size();
                lines[curLine - 1] += lines[curLine];
                lines.erase(lines.begin() + curLine);
                curLine--;
            }
            renderText(lines, curLine, curPos);
        }
        else if (key == 224) { // Šipky
            SHORT arrow = _getch();
            if (arrow == 72) { // Up
                if (curLine > 0) {
                    curLine--;
                    if (curPos > (int)lines[curLine].size()) curPos = (int)lines[curLine].size();
                }
            }
            else if (arrow == 80) { // Down
                if (curLine < (int)lines.size() - 1) {
                    curLine++;
                    if (curPos > (int)lines[curLine].size()) curPos = (int)lines[curLine].size();
                }
            }
            else if (arrow == 75) { // Left
                if (curPos > 0) {
                    curPos--;
                }
                else if (curLine > 0) {
                    curLine--;
                    curPos = (int)lines[curLine].size();
                }
            }
            else if (arrow == 77) { // Right
                if (curPos < (int)lines[curLine].size()) {
                    curPos++;
                }
                else if (curLine < (int)lines.size() - 1) {
                    curLine++;
                    curPos = 0;
                }
            }
            renderText(lines, curLine, curPos);
        }
        else if (key >= 32 && key <= 126) { // Normální znaky
            lines[curLine].insert(lines[curLine].begin() + curPos, (char)key);
            curPos++;
            renderText(lines, curLine, curPos);
        }
        // jinak ignoruj
    }
}

int main() {
    system("title FastBox Text Editor");

    runEditor();

    return 0;
}
