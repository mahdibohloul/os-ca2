#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CourseProcess.h"
#include "filesystem"
#include "iostream"

using namespace std;

CourseProcess::CourseProcess(const std::string &course_name, const std::pair<int, int> &parent_pipe,
                             const std::string &course_data) {
    this->course_name = course_name;
    this->parent_pipe = parent_pipe;
    set_fifo_names(course_data);
//    cout << "CourseProcess: " << course_name << endl;
//    for (const auto &item: fifo_names)
//        cout << "\t" << item << "";
//    cout << endl;
    create_fifos();
}

std::vector<std::string> CourseProcess::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void CourseProcess::set_fifo_names(const std::string &course_data) {
    auto dir_data = split(course_data, '%');
    for (const auto &item: dir_data) {
        auto data = split(item, ',');
        auto dir_name = data[0];
        auto csv_names = split(data[1], '#');
        auto path = filesystem::path(dir_name);
        for (const auto &csv: csv_names) {
            auto fifo_path = path.filename().string() + "_" + csv + "_" + course_name + "_fifo";
            fifo_names.push_back(fifo_path);
        }
    }
}

void CourseProcess::create_fifos() {
    for (const auto &fifo_name: fifo_names) {
        int fd = open(fifo_name.c_str(), O_RDONLY);
        if (fd == -1) {
            mkfifo(fifo_name.c_str(), 0666);
        }
        close(fd);
    }
}

void CourseProcess::handle() {
    for (const auto &item: fifo_names) {
        int fd = open(item.c_str(), O_RDONLY);
        char buffer[1024];
        read(fd, buffer, 1024);
        close(fd);
        string data(buffer);
//        string msg = course_name + " received: " + data + " " + item + "\n";
//        cout << msg;
        double mark = stod(data);
        marks.push_back(mark);
    }
    unlink_fifos();
//    cout << "Fifos unlinked" << endl;
    cout << course_name << " Mean: " << calculate_mean() << endl;
}

double CourseProcess::calculate_mean() {
    double sum = 0;
    for (const auto &mark: marks) {
        sum += mark;
    }
    if (marks.empty()) {
        return 0;
    }
    return sum / marks.size();
}

void CourseProcess::unlink_fifos() {
    for (const auto &item: fifo_names) {
        remove(item.c_str());
    }
}
