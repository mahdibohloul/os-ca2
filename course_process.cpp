#include <iostream>
#include "CourseProcess.h"

using namespace std;

int main(int argc, char *argv[]) {
//    cout << "*********************CourseProcess*********************" << endl;
    string course_name = argv[1];
    string course_data = argv[2];
    auto parent_pipe = make_pair(stoi(argv[3]), stoi(argv[4]));
//    cout << "*********************" << course_name << ": " << course_data << "*********************" << endl;
    auto course_process = new CourseProcess(course_name, parent_pipe, course_data);
    course_process->handle();
//    cout << "*********************Course Process End*********************" << endl;
    delete course_process;
}