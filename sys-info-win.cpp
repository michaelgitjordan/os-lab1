#include <windows.h>
#include <versionhelpers.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <pdh.h>

using namespace std;

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "advapi32.lib")
//#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class SystemInfo {
    public:
        void PrintSystemInfo() {
            PrintOSInfo();
            PrintComputerInfo();
            PrintArchitecture();
            PrintMemoryInfo();
            PrintProcessorInfo();
            PrintDrivesInfo();
        }
    
    private:
        void PrintOSInfo(){
            cout << "OS: ";
            if (IsWindows10OrGreater()){
                cout << "Windows 10 or Greater";
            } else if (IsWindows8Point1OrGreater()){
                cout << "Windows 8.1";
            } else if (IsWindows8OrGreater()){
                cout << "Windows 8";
            } else if (IsWindows7OrGreater()){
                cout << "Windows 7";
            } else {
                cout << "Older Windows version";
            }
            cout << endl;
        }

        void PrintComputerInfo() {
            CHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
            DWORD size = sizeof(computerName);
            
            if (GetComputerNameA(computerName, &size)) {
                std::cout << "Computer Name: " << computerName << std::endl;
            }

            CHAR userName[256];
            DWORD userNameSize = sizeof(userName);
            
            if (GetUserNameA(userName, &userNameSize)) {
                std::cout << "User: " << userName << std::endl;
            }
        }

        void PrintArchitecture(){
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);

            cout << "Architecture: ";
            switch(sysInfo.wProcessorArchitecture){
                case PROCESSOR_ARCHITECTURE_AMD64:
                    cout << "x64 (AMD64)";
                    break;
                case PROCESSOR_ARCHITECTURE_ARM64:
                    cout << "ARM64";
                    break;
                case PROCESSOR_ARCHITECTURE_INTEL:
                    cout << "x86";
                    break;
                default:
                    cout << "Unknown";
                    break;
            }
            cout << endl;
        }

        void PrintMemoryInfo(){
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);

            if (GlobalMemoryStatusEx(&memInfo)){
                DWORDLONG totalRam = memInfo.ullTotalPhys / (1024 * 1024);
                DWORDLONG usedRam = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);

                cout << "RAM: " << usedRam << "MB / " << totalRam << "MB" << endl;

                DWORDLONG totalVirtual = memInfo.ullTotalVirtual / (1024 * 1024);
                cout << "Virtual Memory: " << totalVirtual << "MB" << endl;

                cout << "Memory load: " << memInfo.dwMemoryLoad << "%" << endl;
            }

            PERFORMANCE_INFORMATION perfInfo;
            perfInfo.cb = sizeof(perfInfo);

            if (GetPerformanceInfo(&perfInfo, sizeof(perfInfo))){
                DWORDLONG totalPagefile = perfInfo.CommitLimit * perfInfo.PageSize / (1024 * 1024);
                DWORDLONG usedPagefile = perfInfo.CommitTotal * perfInfo.PageSize / (1024 * 1024);

                cout << "Pagefile: " << usedPagefile << "MB / " << totalPagefile << "MB" << endl;
            }
        }

        void PrintProcessorInfo(){
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            cout << "Processors: " << sysInfo.dwNumberOfProcessors << endl;
        }

        void PrintDrivesInfo(){
            cout << "Drives: " << endl;

            DWORD drives = GetLogicalDrives(); // Получение битовой маски дисков.

            for (char drive = 'A'; drive <= 'Z'; drive++){
                if (drives & 1) {
                    string rootPath = string(1, drive) + ":\\";
                    ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;

                    if (GetDiskFreeSpaceExA(rootPath.c_str(), &freeBytes, &totalBytes, &totalFreeBytes)){
                        DWORDLONG freeGB = totalFreeBytes.QuadPart / (1024 * 1024 * 1024);
                        DWORDLONG totalGB = totalBytes.QuadPart / (1024 * 1024 * 1024);

                        cout << " - " << drive << ":\\ " << freeGB << " GB free / " << totalGB << " GB total" << endl;
                    }
                }
                drives >>=1;
            }

        }
        
};

int main(){

    // SetConsoleOutputCP(CP_UTF8);
    // SetConsoleCP(CP_UTF8);
    SystemInfo sysInfo;
    sysInfo.PrintSystemInfo();

    cout << "\nPress Enter to exit...";
    cin.get();

    return 0;
}