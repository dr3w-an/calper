#define DATABASE_NAME "calper.dat"

#include "Task.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include <vector>


int show_usage(char *program_name) {
    std::cerr << "Usage: " << program_name << " <command> [options]\n\n"
              << "Commands:\n"
              << "\tshow [options]         Show tasks\n"
              << "\tadd <TITLE> [options]  Add task\n"
              << "\tedit <ID> [options]    Edit task\n"
              << "\tremove <ID>            Remove task\n"
              << "\n"
              << "Options:\n"
              << "\t-p PRIORITY            Priority of task (default is 1)\n"
              << "\t-d DAY                 Day of task (default is system time)\n"
              << "\t-m MONTH               Month of task (default is system time)\n"
              << "\t-y YEAR                Year of task (default is system time)\n"
              << "\t-s HH:MM               Start time of task (default is 00:00)\n"
              << "\t-e HH:MM               End time of task (default is 23:59)\n"
              << "\t-x                     Task is done (default is false)"
              << std::endl;
    return EXIT_FAILURE;
}


bool stoi(const std::string &str, int &value) {
    try {
        value = std::stoi(str);
        return true;
    } catch (std::invalid_argument &error) {
        std::cerr << "Invalid argument: '" << str << "' cannot be converted to a number" << std::endl;
        return false;
    }
}


int int_width(int n) {
    int width = 0;
    while (n) {
        n /= 10;
        width++;
    }
    return width;
}


int show_tasks(int argc, char *argv[]) {
    int year, month, day;
    bool year_specified = false;
    bool month_specified = false;
    bool day_specified = false;

    int arg = 2;
    while (arg < argc) {
        std::string argument = argv[arg];
        if (argument[0] == '-' && argument.size() == 2) {
            switch (argument[1]) {
                case 'y':
                    if (!stoi(argv[arg + 1], year))
                        return EXIT_FAILURE;
                    year_specified = true;
                    arg += 2;
                    break;
                case 'm':
                    if (!stoi(argv[arg + 1], month))
                        return EXIT_FAILURE;
                    month_specified = true;
                    arg += 2;
                    break;
                case 'd':
                    if (!stoi(argv[arg + 1], day))
                        return EXIT_FAILURE;
                    day_specified = true;
                    arg += 2;
                    break;
                default:
                    std::cerr << "Unknown option: " << argument << std::endl;
                    arg++;
                    break;
            }
            continue;
        }
        arg++;
    }

    Date date;

    if (year_specified)
        date.set_year(year);
    else
        date.set_year();

    if (month_specified)
        date.set_month(month);
    else if (!year_specified || day_specified)
        date.set_month();

    if (day_specified)
        date.set_day(day);
    else if (!(year_specified || month_specified))
        date.set_day();

    std::ifstream database;
    database.open(DATABASE_NAME, std::ios::ate);
    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return EXIT_FAILURE;
    }
    if ((int)database.tellg() == 0) {
        std::cerr << "Database is empty." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<Task> task_vector = {};
    int max_task_id = 0;
    int max_priority = 0;

    database.seekg(0, std::ios_base::beg);
    bool task_read = true;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (task_read && task.is_date_equal(date)) {
            task_vector.push_back(task);
            if (task.id > max_task_id) max_task_id = task.id;
            if (task.priority > max_priority) max_priority = task.priority;
	}
    }
    database.close();

    std::sort(task_vector.begin(), task_vector.end());
    int id_width = int_width(max_task_id);
    int priority_width = int_width(max_priority);

    if (!task_vector.empty()) {
        std::cout << "Tasks on " << date.date_format() << '\n';
        for (Task task: task_vector) {
            std::cout << task.format(id_width, priority_width) << '\n';
        }
        return EXIT_SUCCESS;
    } else {
        std::cerr << "Can't get tasks on " << date.date_format() << '.' << std::endl;
        return EXIT_FAILURE;
    }
};


int parse_task_argument(char *argv[], int arg, Task &task) {
    switch (argv[arg][1]) {
        case 'p': {
            int priority;
            if (!stoi(argv[arg + 1], priority))
                return -1;
            task.priority = priority;
            return arg + 2;
        }
        case 'y': {
            int year;
            if (!stoi(argv[arg + 1], year))
                return -1;
            task.set_year(year);
            return arg + 2;
        }
        case 'm': {
            int month;
            if (!stoi(argv[arg + 1], month))
                return -1;
            task.set_month(month);
            return arg + 2;
        }
        case 'd': {
            int day;
            if (!stoi(argv[arg + 1], day))
                return -1;
            task.set_day(day);
            return arg + 2;
        }
        case 's': {
            Time start;
            std::istringstream stream(argv[arg + 1]);
            stream >> start;
            task.set_start(start);
            return arg + 2;
        }
        case 'e': {
            Time end;
            std::istringstream stream(argv[arg + 1]);
            stream >> end;
            task.set_end(end);
            return arg + 2;
        }
        case 'x': {
            task.done = not task.done;
            return arg + 1;
        }
    }
    std::cerr << "Unknown option: " << argv[arg] << std::endl;
    return arg + 1;
}


int add_task(int argc, char *argv[]) {
    if (argc <= 2)
        return show_usage(argv[0]);

    Task task;
    task.today();

    int arg = 2;
    std::ostringstream title;
    bool stream_is_empty = true;
    while (arg < argc) {
        std::string argument = argv[arg];
        if (argument[0] == '-' && argument.size() == 2) {
            arg = parse_task_argument(argv, arg, task);
            if (arg < 0)
                return EXIT_FAILURE;
            else
                continue;
        }
        if (stream_is_empty) {
            title << argument;
            stream_is_empty = false;
        } else {
            title << ' ' << argument;
        }
        arg++;
    }

    if (stream_is_empty)
        return show_usage(argv[0]);

    task.title = title.str();

    std::fstream database;
    database.open(DATABASE_NAME, std::ios::ate | std::ios::app | std::ios::in | std::ios::out);

    if ((int)database.tellg() > 0) {
        char ch;
        do {
            database.seekg(-2, std::ios_base::cur);
            if ((int)database.tellg() <= 1) {
                database.seekp(0, std::ios_base::beg);
                break;
            }
            database.get(ch);
        } while (ch != '\n');

        int task_id;
        database >> task_id;
        task.id = task_id + 1;
    } else {
        task.id = 1;
    }

    if (database << task) {
        std::cout << "Task successfully added:\n" << task.format() << std::endl;
    } else {
        std::cerr << "Can't add task." << std::endl;
    }

    database.close();
    return EXIT_SUCCESS;
}


int edit_task(int argc, char *argv[]) {
    if (argc <= 2)
        return show_usage(argv[0]);

    std::unordered_set<int> task_ids;
    int options_start = 2;
    int task_id;
    while (argv[options_start][0] != '-') {
        if (!stoi(argv[options_start], task_id))
            return EXIT_FAILURE;
        task_ids.insert(task_id);
        options_start++;
        if (options_start >= argc)
            return show_usage(argv[0]);
    }

    std::ifstream database;
    database.open(DATABASE_NAME);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return EXIT_FAILURE;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name);
    std::ostringstream title;

    bool task_read = true;
    int task_count = 0;
    bool stream_is_empty = true;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (task_read) {
            int arg = options_start;
            if (task_ids.find(task.id) != task_ids.end()) {
                task_count++;
                while (arg < argc) {
                    std::string argument = argv[arg];
                    if (argument[0] == '-' && argument.size() == 2) {
                        arg = parse_task_argument(argv, arg, task);
                        if (arg < 0)
                            return EXIT_FAILURE;
                        else
                            continue;
                    }
                    if (task_ids.size() == 1 && stream_is_empty) {
                        title << argument;
                        stream_is_empty = false;
                    } else {
                        title << ' ' << argument;
                    }
                    arg++;
                }
                if (!stream_is_empty) task.title = title.str();
            }
            temp_database << task;
        }
    }

    if (task_count > 0) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        if (task_count == 1)
            std::cout << "1 task";
        else
            std::cout << task_count << " tasks";
        std::cout << " successfully edited." << std::endl;
    } else {
        std::cerr << "No tasks edited." << std::endl;
    }

    database.close();
    temp_database.close();
    return EXIT_SUCCESS;
};


int remove_task(int argc, char *argv[]) {
    if (argc <= 2)
        return show_usage(argv[0]);

    std::unordered_set<int> task_ids;
    int task_id;
    for (int i = 2; i < argc; i++) {
        if (!stoi(argv[i], task_id))
            return EXIT_FAILURE;
        task_ids.insert(task_id);
    }

    std::ifstream database;
    database.open(DATABASE_NAME);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return EXIT_FAILURE;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name);

    bool task_read = true;
    int task_count = 0;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (task_read) {
            if (task_ids.find(task.id) == task_ids.end()) {
                temp_database << task;
            } else {
                task_count++;
            }
        }
    }

    if (task_count > 0) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        if (task_count == 1)
            std::cout << "1 task";
        else
            std::cout << task_count << " tasks";
        std::cout << " successfully removed." << std::endl;
    } else {
        std::cerr << "No tasks removed." << std::endl;
    }

    database.close();
    temp_database.close();
    return EXIT_SUCCESS;
};


int main(int argc, char *argv[]) {
    if (argc < 2)
        return show_usage(argv[0]);

    try {
        switch (argv[1][0]) {
            case 's': return show_tasks(argc, argv);
            case 'a': return add_task(argc, argv);
            case 'e': return edit_task(argc, argv);
            case 'r': return remove_task(argc, argv);
            default:  return show_usage(argv[0]);
        }
    } catch (std::invalid_argument &error) {
        std::cerr << "Invalid argument: " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
}
