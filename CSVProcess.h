#ifndef CSVPROCESS_CSVPROCESS_H
#define CSVPROCESS_CSVPROCESS_H

#include "string"
#include "vector"
#include "map"
#include "fstream"
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "filesystem"

#define CHEMISTRY "Chemistry"
#define PHYSICS "Physics"
#define MATH "Math"
#define ENGLISH "English"
#define LITERATURE "Literature"

class CSVProcess {
public:
    CSVProcess(const std::string &file_name, const std::pair<int, int> &parent_pipe, const std::string &class_name);

    void handle();

private:
    std::map<std::string, double> extract_marks_from_csv_file();

    std::vector<std::string> create_fifo(const std::vector<std::string> &subjects);

    static void send_through_fifo(const std::string &fifo_name, const std::string &message);

private:
    std::string file_name;
    std::pair<int, int> parent_pipe;
    std::string class_name;
    static std::vector<std::string> courses;
};


#endif //CSVPROCESS_CSVPROCESS_H
