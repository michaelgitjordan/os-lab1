#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include <unistd.h>
#include <pwd.h>

using namespace std;

class LinuxSystemInfo {
    public:
        void PrintSystemInfo() {
            PrintOSInfo();
            PrintKernelInfo();
            PrintArchitecture();
            PrintHostname();
            PrintUserInfo();
            PrintMemoryInfo();
            PrintProcessorInfo();
            PrintLoadAverage();
            PrintDrivesInfo();
        }

    private:
        void PrintOSInfo(){
            ifstream osRelease("/etc/os-release");
            if (osRelease.is_open()){
                string line;
                string name, version;

                while(getline(osRelease, line)){
                    if (line.find("PRETTY_NAME=") == 0){
                        name = line.substr(13, line.length() - 14);
                    } else if (line.find("VERSION_ID=") == 0){
                        version = line.substr(11, line.length() - 12);
                    }
                }

                if (!name.empty()){
                    cout << "OS: " << name;
                    if (!version.empty()){
                        cout << " " << version;
                    }
                    cout << endl;
                    return;
                }
            }
        }

        void PrintKernelInfo(){
            struct utsname unameData;
            if (uname(&unameData) == 0){
                cout << "Kernel: " << unameData.sysname << " " << unameData.release << endl;
            }

        }

        void PrintArchitecture(){
            struct utsname unameData;
            if (uname(&unameData) == 0){
                cout << "Architecture: " << unameData.machine << endl;
            }
        }

        void PrintHostname(){
            struct utsname unameData;
            if (uname(&unameData) == 0){
                cout << "Hostname: " << unameData.nodename << endl;
            }
        }

        void PrintUserInfo(){
            char *username = getlogin();
            if (username != nullptr){
                cout << "User: " << username << endl;
            } else {
                struct passwd *pw = getpwuid(getuid());
                if (pw != nullptr){
                    cout << "User: " << pw->pw_name << endl;
                }
            }
        }

        void PrintMemoryInfo(){
            struct sysinfo si;
            if (sysinfo(&si) == 0){
                unsigned long totalRAM = si.totalram * si.mem_unit / (1024 * 1024);
                unsigned long freeRAM = si.freeram * si.mem_unit / (1024 * 1024);
                cout << "RAM: " << freeRAM << "MB free / " << totalRAM << "MB total" << endl;

                unsigned long totalSwap = si.totalswap * si.mem_unit / (1024 * 1024);
                unsigned long freeSwap = si.freeswap * si.mem_unit / (1024 * 1024);
                cout << "Swap: " << totalSwap << "MB total / " << freeSwap << "MB free" << endl;

                unsigned long totalVirtual = totalRAM + totalSwap;
                cout << "Virtual memory: " << totalVirtual << " MB" << endl;
            }
        }

        void PrintProcessorInfo(){
            cout << "Processors: " <<get_nprocs() << endl;
        }

        void PrintLoadAverage(){
            ifstream loadavg("/proc/loadavg");
            if(loadavg.is_open()){
                string line;
                if (getline(loadavg, line)){
                    istringstream iss(line);
                    string load1, load5, load15;
                    iss >> load1 >> load5 >> load15;
                    cout << "Load average: " << load1 << ", " << load5 << ", " << load15 << endl;
                }
            }
        }

        void PrintDrivesInfo(){
            cout << "Drives: " << endl;

            FILE *mtab = setmntent("/proc/mounts", "r");
            if (mtab == nullptr){
                cout << " Unable to read mount information" << endl;
                return;
            }

            struct mntent *entry;
            while ((entry = getmntent(mtab)) != nullptr){
                string mountPoint = entry->mnt_dir;
                string fsType = entry->mnt_type;

                if(mountPoint == "/" || mountPoint.find("/mnt/") == 0){
                    struct statvfs vfs;
                    if (statvfs(mountPoint.c_str(), &vfs) == 0){
                        unsigned long totalGB = (vfs.f_blocks * vfs.f_frsize) / (1024 * 1024 * 1024);
                        unsigned long freeGB = (vfs.f_bfree * vfs.f_frsize) / (1024 * 1024 * 1024);

                        if (totalGB > 0){
                            cout << " " << mountPoint << " (" << fsType << "): " << freeGB << "GB free / " << totalGB << "GB total" << endl;
                        }
                    }
                }                              
            }

            endmntent(mtab);

        }


};

int main(){
    LinuxSystemInfo sysInfo;
    sysInfo.PrintSystemInfo();
    return 0;
}