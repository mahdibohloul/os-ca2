#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <sys/wait.h>
#include <map>
#include <cstring>

#define CHEMISTRY "Chemistry"
#define PHYSICS "Physics"
#define MATH "Math"
#define ENGLISH "English"
#define LITERATURE "Literature"

using namespace std;
vector<string> courses = {
        CHEMISTRY,
        PHYSICS,
        MATH,
        ENGLISH,
        LITERATURE
};
vector<pair<int, int>> dir_pipes;
map<string, pair<int, int>> dir_pipes_map;
vector<string> course_initiation_data;


vector<string> get_directories(const string &path) {
    vector<string> directories;
    for (auto &p: std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(p)) {
            directories.push_back(p.path().string());
        }
    }
    return directories;
}

void create_pipes(const int &number, vector<pair<int, int>> &pp) {
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

string get_course_initiation_data() {
    stringstream ss;
    string delim;
    for (const auto &item: course_initiation_data) {
        ss << delim << item;
        delim = "%";
    }
    return ss.str();
}

void create_course_process(const pid_t &root_pid) {
    vector<pair<int, int>> pp;
    create_pipes(courses.size(), pp);
    auto course_data = get_course_initiation_data();
    vector<pid_t> pids;
    for (int i = 0; i < courses.size(); i++) {
        auto course = courses[i];
        pid_t pid = fork();
        if (pid == 0) {
            char *argv[6];
            argv[0] = (char *) "./courseProcess";
            argv[1] = (char *) course.c_str();
            argv[2] = (char *) course_data.c_str();
            argv[3] = (char *) to_string(pp[i].first).c_str();
            argv[4] = (char *) to_string(pp[i].second).c_str();
            argv[5] = NULL;
            execvp(argv[0], argv);
        } else
            pids.push_back(pid);
    }
}

void create_dir_processes(const pid_t &root_pid, const string &course_dir) {
    vector<pid_t> dir_processes;
    auto directories = get_directories(course_dir);
    create_pipes(directories.size(), dir_pipes);
    for (int i = 0; i < directories.size(); i++) {
        auto dir = directories[i];
        dir_pipes_map[dir] = dir_pipes[i];
        pid_t pid = fork();
        if (pid == 0) {
            char *argv[5];
            argv[0] = (char *) "./directoryProcess";
            argv[1] = (char *) dir.c_str();
            argv[2] = (char *) (to_string(dir_pipes[i].first)).c_str();
            argv[3] = (char *) (to_string(dir_pipes[i].second)).c_str();
            argv[4] = NULL;
            execvp(argv[0], argv);
        } else if (pid > 0) {
            dir_processes.push_back(pid);
        } else {
            cerr << "Failed to fork directory process" << endl;
            exit(1);
        }
    }
}


void get_fifo_data_from_dir_process() {
    char buffer[1024];
    for (auto &dir_pip: dir_pipes_map) {
        auto dir_name = dir_pip.first;
        auto pipe = dir_pip.second;
        close(pipe.second);
        long rb = read(pipe.first, buffer, 1024);
        if (rb == -1) {
            cerr << "Failed to read from fifo: " << dir_name << endl;
            exit(1);
        } else if (rb == 0) {
            cerr << "Fifo is empty: " << dir_name << endl;
            exit(1);
        } else if (buffer[rb - 1] != '\0') {
            buffer[rb] = '\0';
        }
        close(pipe.first);
        string data = buffer;
        dir_name += "," + data;
        course_initiation_data.push_back(dir_name);
        memset(buffer, 0, 1024);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <course directory>" << endl;
        return 1;
    }
    string course_dir = argv[1];
    pid_t root_pid = getpid();
//    cout << "Root process id: " << root_pid << endl;

    create_dir_processes(root_pid, course_dir);
    get_fifo_data_from_dir_process();
    create_course_process(root_pid);
    wait(NULL);
}