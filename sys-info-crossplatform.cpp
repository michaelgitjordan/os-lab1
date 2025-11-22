#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

#ifdef _WIN32
    #include <windows.h>
    #include <versionhelpers.h>
    #include <psapi.h>
    #pragma comment(lib, "psapi.lib")
#elif __linux__
    #include <sys/utsname.h>
    #include <sys/sysinfo.h>
    #include <fstream>
    #include <sstream>
#endif

class SysInfo {
public:
    string GetOSName() const {
        #ifdef _WIN32
            return GetWindowsOSName();
        #elif __linux__
            return GetLinuxOSName();
        #else
            return "Unknown OS";
        #endif
    }

    string GetOSVersion() const {
        #ifdef _WIN32
            return GetWindowsOSVersion();
        #elif __linux__
            return GetLinuxOSVersion();
        #else
            return "Unknown Version";
        #endif
    }

    uint64_t GetFreeMemory() const {
        #ifdef _WIN32
            return GetWindowsFreeMemory();
        #elif __linux__
            return GetLinuxFreeMemory();
        #else
            return 0;
        #endif
    }

    uint64_t GetTotalMemory() const {
        #ifdef _WIN32
            return GetWindowsTotalMemory();
        #elif __linux__
            return GetLinuxTotalMemory();
        #else
            return 0;
        #endif
    }

    unsigned GetProcessorCount() const {
        #ifdef _WIN32
            return GetWindowsProcessorCount();
        #elif __linux__
            return GetLinuxProcessorCount();
        #else
            return 0;
        #endif
    }

private:

    #ifdef _WIN32
    string GetWindowsOSName() const {
        if (IsWindows10OrGreater()) return "Windows 10 or newer";
        if (IsWindows8Point1OrGreater()) return "Windows 8.1";
        if (IsWindows8OrGreater()) return "Windows 8";
        if (IsWindows7OrGreater()) return "Windows 7";
        return "Windows";
    }

    string GetWindowsOSVersion() const {
        return "Windows NT";
    }

    uint64_t GetWindowsFreeMemory() const {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return memInfo.ullAvailPhys / (1024*1024*1024);
        }
        return 0;
    }

    uint64_t GetWindowsTotalMemory() const {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return memInfo.ullTotalPhys / (1024*1024*1024);
        }
        return 0;
    }

    unsigned GetWindowsProcessorCount() const {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
    }
    #endif

    #ifdef __linux__
    string GetLinuxOSName() const {
        ifstream osRelease("/etc/os-release");
        if (osRelease.is_open()) {
            string line;
            while (getline(osRelease, line)) {
                if (line.find("PRETTY_NAME=") == 0) {
                    return line.substr(13, line.length() - 14);
                }
            }
        }
        return "Linux";
    }

    string GetLinuxOSVersion() const {
        struct utsname unameData;
        if (uname(&unameData) == 0) {
            return string(unameData.release);
        }
        return "Unknown Version";
    }

    uint64_t GetLinuxFreeMemory() const {
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            return (si.freeram * si.mem_unit) / (1024*1024*1024);
        }
        return 0;
    }

    uint64_t GetLinuxTotalMemory() const {
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            return (si.totalram * si.mem_unit) / (1024*1024*1024);
        }
        return 0;
    }

    unsigned GetLinuxProcessorCount() const {
        return get_nprocs();
    }
    #endif
};

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    SysInfo sysInfo;
    
    cout << "Системная информация: " << endl;
    cout << "Операционная система: " << sysInfo.GetOSName() << endl;
    cout << "Версия ОС: " << sysInfo.GetOSVersion() << endl;
    cout << "Количество логических процессоров: " << sysInfo.GetProcessorCount() << endl;
    cout << "Свободно оперативной памяти: " << (sysInfo.GetFreeMemory()) << " из " << (sysInfo.GetTotalMemory()) << " GB" << endl;

 
    return 0;
}