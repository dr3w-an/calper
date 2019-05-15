#include "Task.h"
#include <vector>


Task::Task():
    start(0, 0), end(23, 59), done(false)
{}


void Task::set_start(Time &time) {
    if (time >= end)
        throw std::invalid_argument("start must be before end");
    else
        start = time;
}


void Task::set_end(Time &time) {
    if (time <= start)
        throw std::invalid_argument("end must be after start");
    else
        end = time;
}


std::string Task::format() const {
    std::ostringstream stream;
    stream << date_format() << " (" << start.format() << '-' << end.format() << ")";
    return stream.str();
}


bool Task::operator < (const Task &task) const {
    return (
        year < task.year || (year == task.year && (month < task.month ||
            (month == task.month && (day < task.day ||
                (day == task.day && (start < task.start ||
                    (start == task.start && (end < task.end ||
                        (end == task.end && (id < task.id))
                    ))
                ))
            ))
        ))
    );
}


std::istream &operator >> (std::istream &stream, Task &task) {
    char separator;

    if (stream >> task.id) {
        stream >> separator;
        stream >> task.done;

        int y;
        stream >> separator;
        stream >> y;
        task.set_year(y);

        int m;
        stream >> separator;
        stream >> m;
        task.set_month(m);

        int d;
        stream >> separator;
        stream >> d;
        task.set_day(d);

        Time start;
        stream >> separator;
        stream >> start;
        task.set_start(start);

        Time end;
        stream >> separator;
        stream >> end;
        task.set_end(end);

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


std::ostream &operator << (std::ostream &stream, Task &task) {
    const char separator = ',';

    stream << task.id;
    stream << separator;
    stream << (int)task.done;
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
