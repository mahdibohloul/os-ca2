#ifndef MEANCALCULATOR_DIRECTORYPROCESS_H
#define MEANCALCULATOR_DIRECTORYPROCESS_H

#include "string"
#include "vector"
#include "filesystem"

class DirectoryProcess {
public:
    DirectoryProcess(const std::string &directory_path, const std::pair<int, int> &pipe);

    void handle();

private:
    std::vector<std::string> get_csv_files();

    void create_child_pipes(const int &number);

    void send_child_names_to_parent();

    std::string prepare_child_names();

    static void send_through_pipe(const std::pair<int, int> &parent_pipe, const std::string &message);

private:
    std::string directory_path;
    std::pair<int, int> parent_pipe;
    std::vector<std::pair<int, int>> child_pipes;
    std::vector<pid_t> child_processes;
    std::string directory_name;
};


#endif //MEANCALCULATOR_DIRECTORYPROCESS_H
