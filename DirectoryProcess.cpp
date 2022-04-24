#include <unistd.h>
#include <iostream>
#include "DirectoryProcess.h"

using namespace std;

DirectoryProcess::DirectoryProcess(const std::string &directory_path, const std::pair<int, int> &pipe) {
    this->directory_path = directory_path;
    this->parent_pipe = pipe;
    this->directory_name = filesystem::path(directory_path).filename().string();
}

void DirectoryProcess::handle() {
    auto csv_files = get_csv_files();
    send_child_names_to_parent();
    create_child_pipes(csv_files.size());
    sleep(1);
    for (int i = 0; i < csv_files.size(); ++i) {
        auto csv_file = csv_files[i];
        pid_t pid = fork();
        if (pid == 0) {
            char *argv[6];
            argv[0] = (char *) "./csvProcess";
            argv[1] = (char *) csv_file.c_str();
            argv[2] = (char *) to_string(child_pipes[i].first).c_str();
            argv[3] = (char *) to_string(child_pipes[i].second).c_str();
            argv[4] = (char *) directory_name.c_str();
            argv[5] = NULL;
            execvp(argv[0], argv);
        } else if (pid > 0) {
            child_processes.push_back(pid);
        } else {
            cerr << "Failed to fork csv process" << endl;
            exit(1);
        }
    }
}

std::vector<std::string> DirectoryProcess::get_csv_files() {
    vector<string> files;
    for (auto &p: std::filesystem::directory_iterator(directory_path)) {
        if (std::filesystem::is_regular_file(p) && p.path().extension() == ".csv") {
            files.push_back(p.path().string());
        }
    }
    return files;
}

void DirectoryProcess::create_child_pipes(const int &number) {
    child_pipes = vector<pair<int, int>>(number);
    for (int i = 0; i < number; ++i) {
        int fd[2];
        if (pipe(fd) == -1) {
            cout << "Pipe creation failed" << endl;
            exit(1);
        }
        child_pipes[i] = make_pair(fd[0], fd[1]);
    }
}

void DirectoryProcess::send_child_names_to_parent() {
    auto child_names = prepare_child_names();
    send_through_pipe(parent_pipe, child_names);
}

std::string DirectoryProcess::prepare_child_names() {
    stringstream ss;
    string delim;
    for (auto &p: std::filesystem::directory_iterator(directory_path)) {
        if (std::filesystem::is_regular_file(p) && p.path().extension() == ".csv") {
            ss << delim << p.path().filename().replace_extension("").string();
            delim = "#";
        }
    }
    return ss.str();
}

void DirectoryProcess::send_through_pipe(const pair<int, int> &parent_pipe, const string &message) {
    close(parent_pipe.first);
    write(parent_pipe.second, message.c_str(), message.length());
    close(parent_pipe.second);
}
