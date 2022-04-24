#include "CSVProcess.h"


using namespace std;


int main(int argc, char **argv) {
//    cout << "*******************CSV Process Execution***********************" << endl;
    string file_path = argv[1];
    auto pipe = make_pair(stoi(argv[2]), stoi(argv[3]));
    string class_name = argv[4];
//    cout << "*******************CSV File Path: " << file_path << "***********************" << endl;
    auto csv_process = new CSVProcess(file_path, pipe, class_name);
    csv_process->handle();
//    cout << "*******************CSV Process Execution Completed***********************" << endl;
    return 0;
}