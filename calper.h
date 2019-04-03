#include <iomanip>

struct Date {
    int day;
    int month;
    int year;

    Date();
    Date(int d, int m, int y);
    std::string format(char specifier);
};


struct Task {
    struct Date date;
    int id;
    int priority;
    std::tm start_time;
    std::tm end_time;
    std::string title;
};
