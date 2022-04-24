#ifndef CSVPROCESS_COURSEPROCESS_H
#define CSVPROCESS_COURSEPROCESS_H

#include <vector>
#include "string"

class CourseProcess {
public:
    CourseProcess(const std::string &course_name, const std::pair<int, int> &parent_pipe,
                  const std::string &course_data);

    void handle();

private:
    static std::vector<std::string> split(const std::string &s, char delim);

    void set_fifo_names(const std::string &course_data);

    void create_fifos();

    double calculate_mean();

    void unlink_fifos();

private:
    std::string course_name;
    std::pair<int, int> parent_pipe;
    std::vector<std::string> fifo_names;
    std::vector<double> marks;

};


#endif //CSVPROCESS_COURSEPROCESS_H
