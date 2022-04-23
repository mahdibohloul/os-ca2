#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <sys/wait.h>

#define CHEMISTRY "Chemistry"
#define PHYSICS "Physics"
#define MATH "Math"
#define ENGLISH "English"
#define LITERATURE "Literature"

using namespace std;

vector<double> chemistry_marks;
vector<double> physics_marks;
vector<double> maths_marks;
vector<double> english_marks;
vector<double> literature_marks;
vector<pair<int, int>> pipes;
int current_pipe_index;

long send_through_pipe(const int &pipe_fd, const string &message);

double calculate_mean(const vector<double> &marks) {
    double sum = 0;
    for (double mark: marks) {
        sum += mark;
    }
    return sum / marks.size();
}

void extract_marks_from_csv_file(const string &file_name) {
    ifstream file(file_name);
    string line;
    while (getline(file, line)) {
        stringstream line_stream(line);
        string cell;
        vector<string> cells;
        while (getline(line_stream, cell, ',')) {
            cells.push_back(cell);
        }
        if (cells[0] == PHYSICS)
            physics_marks.push_back(stod(cells[1]));
        else if (cells[0] == CHEMISTRY)
            chemistry_marks.push_back(stod(cells[1]));
        else if (cells[0] == MATH)
            maths_marks.push_back(stod(cells[1]));
        else if (cells[0] == ENGLISH)
            english_marks.push_back(stod(cells[1]));
        else if (cells[0] == LITERATURE)
            literature_marks.push_back(stod(cells[1]));
    }
    file.close();
}

vector<string> get_directories(const string &path) {
    vector<string> directories;
    for (auto &p: std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(p)) {
            directories.push_back(p.path().string());
        }
    }
    return directories;
}

vector<string> get_csv_files(const string &path) {
    vector<string> files;
    for (auto &p: std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(p) && p.path().extension() == ".csv") {
            files.push_back(p.path().string());
        }
    }
    return files;
}

vector<pid_t> create_processes(const int &n, int &cpi = current_pipe_index) {
    vector<pid_t> pids;
    for (int i = 0; i < n; ++i) {
        cpi = i;
        pid_t pid = fork();
        if (pid == 0) {
            break;
        }
        pids.push_back(pid);
    }
    return pids;
}

vector<pid_t> create_course_processes() {
    return create_processes(5);
}

void create_pipes(const int &number, vector<pair<int, int>> &pp = pipes) {
    pp = vector<pair<int, int>>(number);
    for (int i = 0; i < number; ++i) {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            cout << "Pipe creation failed" << endl;
            exit(1);
        }
        pp[i] = make_pair(pipe_fd[0], pipe_fd[1]);
    }
}

template<typename T>
int find_index_of_element(const vector<T> &target, const T &element) {
    for (int i = 0; i < target.size(); ++i) {
        if (target[i] == element) {
            return i;
        }
    }
    return -1;
}

void handle_csv_processes(const vector<string> &csv_files, const vector<pair<int, int>> &pp) {
    int current_pid = getpid();
    int my_pipe_index = current_pipe_index;
    close(pp[my_pipe_index].second);
    char buffer[256];
    sleep(2);
    cout << "Process " << current_pid << " is waiting for data.\t" << getppid() << endl;
    read(pp[my_pipe_index].first, buffer, 256);
    int csv_file_index = buffer[0] - '0';
    cout << "Process " << current_pid << " got data: " << csv_files[csv_file_index] << endl;
    const auto &csv_file = csv_files[csv_file_index];
    close(pp[my_pipe_index].first);
    sleep(2);
    exit(0);
}

void handle_dir_processes(const vector<string> &directories) {
    int current_pid = getpid();
    vector<pair<int, int>> pp;
    int my_pipe_index = current_pipe_index;
//    int pipe_fd[2];
//    pipe_fd[0] = pipes[my_pipe_index].first;
//    pipe_fd[1] = pipes[my_pipe_index].second;
    close(pipes[my_pipe_index].second);
    char buffer[256];
    cout << "Current process: " << current_pid << " Going to wait for pipe to getting file index." << endl;
    read(pipes[my_pipe_index].first, buffer, 256);
    int file_index = buffer[0] - '0';
    cout << "Current process: " << current_pid << " Got file index: " << file_index << endl;
    const auto &dir_addr = directories[file_index];
//    cout << "Current process: " << current_pid << " Going to read files from directory: " << dir_addr << endl;
    close(pipes[my_pipe_index].first);
    sleep(2);
    auto csv_files = get_csv_files(dir_addr);
    create_pipes(csv_files.size(), pp);
    auto csv_processes = create_processes(csv_files.size());
    if (getpid() == current_pid) {
        cout << "CSV processes:" << endl;
        for (auto &pid: csv_processes) {
            cout << "\t" << pid << endl;
        }
        for (int i = 0; i < pp.size(); ++i) {
            close(pp[i].first);
            cout << "Process " << current_pid << " is writing to pipe: " << pp[i].second << endl;
            send_through_pipe(pp[i].second, to_string(i));
            close(pp[i].second);
        }
        wait(NULL);
    } else {
        handle_csv_processes(csv_files, pp);
    }
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <course directory>" << endl;
        return 1;
    }
    string course_dir = argv[1];
    pid_t root_pid = getpid();
    cout << "Root process id: " << root_pid << endl;
    vector<pid_t> dir_processes;
    auto directories = get_directories(course_dir);
    create_pipes(directories.size());
//    auto course_processes = create_course_processes();
    if (getpid() == root_pid) {
        cout << "Current pid: " << getpid() << " Going to create dir processes" << endl;
//        cout << "Course processes: " << endl;
//        for (auto pid: course_processes) {
//            cout << "\t" << pid << endl;
//        }
        dir_processes = create_processes(directories.size());
        if (getpid() == root_pid) {
            cout << "Dir processes: " << endl;
            for (auto pid: dir_processes) {
                cout << "\t" << pid << endl;
            }
            for (int i = 0; i < pipes.size(); i++) {
                close(pipes[i].first);
                cout << "Current process: " << getpid() << " Going to send file index: " << i << endl;
                send_through_pipe(pipes[i].second, to_string(i));
                close(pipes[i].second);
            }
        } else {
            handle_dir_processes(directories);
        }
    }
//    if (find_index_of_element(course_processes, getpid()) == -1 &&
//        getpid() != root_pid) {
//        cout << "****Current pid: " << getpid() << " :" << find_index_of_element(course_processes, getpid()) << endl;
//        auto index = find_index_of_element(dir_processes, getpid());
//        if (index != -1) {
//            auto dir_addr = directories[index];
//            auto csv_files = get_csv_files(dir_addr);
//            auto cpp = getpid();
//            auto csv_processes = create_processes(csv_files.size());
//            if (cpp != getpid()) {
//                auto csv_index = find_index_of_element(csv_processes, getpid());
//                if (csv_index != -1) {
//                    extract_marks_from_csv_file(csv_files[csv_index]);
//                }
//            }
//        }
//    }
    if (getpid() == root_pid) {
        wait(NULL);
        cout << "Current pid: " << getpid() << " Going to wait for csv processes" << endl;
        cout << "Chemistry mean: " << calculate_mean(chemistry_marks) << endl;
        cout << "Physics mean: " << calculate_mean(physics_marks) << endl;
        cout << "Maths mean: " << calculate_mean(maths_marks) << endl;
        cout << "English mean: " << calculate_mean(english_marks) << endl;
        cout << "Literature mean: " << calculate_mean(literature_marks) << endl;
    } else {
        exit(0);
    }
}

long send_through_pipe(const int &pipe_fd, const string &message) {
    return write(pipe_fd, message.c_str(), message.size());
}
