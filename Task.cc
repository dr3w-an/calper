#include "Task.h"
#include <vector>


Task::Task():
    start(0, 0), end(23, 59)
{}

std::istream& operator >> (std::istream& stream, Task& task) {
    char separator;

    if (stream >> task.id) {
        stream >> separator;
        stream >> task.year;

        int m;
        stream >> separator;
        stream >> m;
        task.set_month(m);

        int d;
        stream >> separator;
        stream >> d;
        task.set_day(d);

        stream >> separator;
        stream >> task.start;
        stream >> separator;
        stream >> task.end;

        size_t title_size;
        stream >> separator;
        stream >> title_size;
        stream >> separator;
        std::vector<char> title(title_size);
        stream.read(title.data(), title_size);
        task.title.assign(title.data(), title_size);
    }

    return stream;
}

std::ostream& operator << (std::ostream& stream, Task& task) {
    const char separator = ',';

    stream << task.id;
    stream << separator;
    stream << task.year;
    stream << separator;
    stream << task.month;
    stream << separator;
    stream << task.day;
    stream << separator;
    stream << task.start;
    stream << separator;
    stream << task.end;
    stream << separator;
    stream << task.title.size();
    stream << separator;
    stream << task.title;
    stream << std::endl;

    return stream;
}
