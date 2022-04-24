#include "CSVProcess.h"

using namespace std;

vector<string> CSVProcess::courses = {CHEMISTRY, PHYSICS, MATH, ENGLISH, LITERATURE};

CSVProcess::CSVProcess(const std::string &file_name, const std::pair<int, int> &parent_pipe,
                       const std::string &class_name) {
    this->file_name = file_name;
    this->parent_pipe = parent_pipe;
    this->class_name = class_name;
}

std::map<std::string, double> CSVProcess::extract_marks_from_csv_file() {
    ifstream file(file_name);
    string line;
    map<string, double> extracted_marks;
    while (getline(file, line)) {
        stringstream line_stream(line);
        string cell;
        vector<string> cells;
        while (getline(line_stream, cell, ',')) {
            cells.push_back(cell);
        }
        extracted_marks[cells[0]] = stod(cells[1]);
    }
    file.close();
    return extracted_marks;
}

void CSVProcess::handle() {
    auto extracted_course_marks = extract_marks_from_csv_file();
    vector<string> extracted_courses;
    vector<double> extracted_marks;
    for (auto &extracted_mark: extracted_course_marks) {
        extracted_courses.push_back(extracted_mark.first);
        extracted_marks.push_back(extracted_mark.second);
    }
    auto fifos = create_fifo(extracted_courses);
    for (int i = 0; i < fifos.size(); ++i) {
        send_through_fifo(fifos[i], to_string(extracted_marks[i]));
    }
}

std::vector<std::string> CSVProcess::create_fifo(const vector<std::string> &subjects) {
    vector<string> fifo_names;
    auto path = filesystem::path(file_name);
    for (auto &subject: subjects) {
        string fifo_name = class_name + "_" + path.filename().replace_extension("").string() + "_" + subject + "_fifo";
        fifo_names.push_back(fifo_name);
    }
    return fifo_names;
}

void CSVProcess::send_through_fifo(const string &fifo_name, const string &message) {
//    cout << "Sending through " << fifo_name << ": " << message << endl;
    int fd = open(fifo_name.c_str(), O_WRONLY);
    if (fd == -1) {
        int res = mkfifo(fifo_name.c_str(), 0666);
//        cout << "Creating fifo: " << fifo_name << endl;
        if (res == -1) {
            cerr << "Error creating fifo" << endl;
            exit(1);
        }
    } else
        fd = open(fifo_name.c_str(), O_WRONLY);
//    cout << "Opened " << fifo_name << endl;
    write(fd, message.c_str(), message.length() + 1);
    close(fd);
}
