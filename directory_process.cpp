#include "iostream"
#include "DirectoryProcess.h"

using namespace std;


int main(int argc, char const *argv[]) {
//    cout << "*******************Directory Process Execution***********************" << endl;
    string directory_name = argv[1];
    auto pipe = make_pair(stoi(argv[2]), stoi(argv[3]));
//    cout << "*******************Directory Name: " << directory_name << "***********************" << endl;
    auto directory_process = new DirectoryProcess(directory_name, pipe);
    directory_process->handle();
//    cout << "*******************Directory Process Execution Completed***********************" << endl;
}