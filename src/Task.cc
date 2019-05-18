#include "Task.h"
#include <iomanip>
#include <vector>


Task::Task():
    start(0, 0), end(23, 59), done(false), priority(1)
{}


void Task::set_start(const Time &time) {
    if (time >= end)
        throw std::invalid_argument("start must be before end");
    else
        start = time;
}


void Task::set_end(const Time &time) {
    if (time <= start)
        throw std::invalid_argument("end must be after start");
    else
        end = time;
}


std::string Task::format(int id_width, int priority_width) const {
    std::ostringstream stream;
    stream << std::setw(id_width) << id
           << " [" << (done ? 'x' : ' ') << "] "
           << std::setw(priority_width) << priority
           << ' ' << date_format()
           << " (" << start.format() << '-' << end.format() << ") "
           << title;
    return stream.str();
}


bool Task::operator < (const Task &task) const {
    return (
        priority > task.priority || (priority == task.priority && (year < task.year ||
            year < task.year || (year == task.year && (month < task.month ||
                (month == task.month && (day < task.day ||
                    (day == task.day && (start < task.start ||
                        (start == task.start && (end < task.end ||
                            (end == task.end && (id < task.id))
                        ))
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
        stream >> separator;
        stream >> task.priority;

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


std::ostream &operator << (std::ostream &stream, const Task &task) {
    char separator = ',';

    stream << task.id;
    stream << separator;
    stream << task.done;
    stream << separator;
    stream << task.priority;
    stream << separator;
    stream << task.date_format();
    stream << separator;
    stream << task.start.format();
    stream << separator;
    stream << task.end.format();
    stream << separator;
    stream << task.title.size();
    stream << separator;
    stream << task.title;
    stream << std::endl;

    return stream;
}
