#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

const string fileName = "input.txt";

string readFile(const string& filename) {
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    string buffer(fileSize, '\0');
    if (!file.read(&buffer[0], fileSize)) {
        throw runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}

wstring utf8ToWide(const string& utf8) {
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (wideLen == 0) {
        throw runtime_error("UTF8 to WideChar conversion failed.");
    }

    vector<wchar_t> wideBuffer(wideLen);
    if (MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wideBuffer[0], wideLen) == 0) {
        throw runtime_error("UTF8 to WideChar conversion failed.");
    }

    return wstring(&wideBuffer[0]);
}

void typeText(const wstring& text) {
    wcout << L"Switch to the target window within 2 seconds..." << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));

    for (wchar_t c : text) {
        if (c == L'\n') {
            INPUT inputs[2] = { {0}, {0} };

            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_RETURN;

            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_RETURN;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

            UINT sent = SendInput(2, inputs, sizeof(INPUT));
            if (sent != 2) {
                cerr << "Failed to send ENTER key. Error: " << GetLastError() << endl;
            }
        } else {
            INPUT inputDown = {0};
            inputDown.type = INPUT_KEYBOARD;
            inputDown.ki.wScan = c;
            inputDown.ki.dwFlags = KEYEVENTF_UNICODE;

            INPUT inputUp = inputDown;
            inputUp.ki.dwFlags |= KEYEVENTF_KEYUP;

            INPUT inputs[2] = {inputDown, inputUp};
            UINT sent = SendInput(2, inputs, sizeof(INPUT));
            if (sent != 2) {
                cerr << "Failed to send input. Error: " << GetLastError() << endl;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

int main() {
    string fileContent = readFile(fileName);
    wstring wideContent = utf8ToWide(fileContent);
    typeText(wideContent);
    return 0;
}