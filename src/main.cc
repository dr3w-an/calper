#define DATABASE_NAME "calper.csv"

#include "Task.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include <vector>


int usage(char *program_name) {
    std::cerr << "Usage: " << program_name << " <command> [options]\n"
              << '\n'
              << "Commands:\n"
              << "  show             Show tasks\n"
              << "    Options:\n"
              << "      -y YEAR      Year of task (default is system time)\n"
              << "      -m MONTH     Month of task (default is unset or system time)\n"
              << "      -d DAY       Day of task (default is unset or system time)\n"
              << "      -x           Exclude done tasks (default is false)\n"
              << "      -c           Colorize the output (default is false)\n"
              << '\n'
              << "  add <TITLE>      Add task\n"
              << "  edit <IDS>       Edit task(s)\n"
              << "    Options:\n"
              << "      -p PRIORITY  Priority of task (default is 1)\n"
              << "      -y YEAR      Year of task (default is system time)\n"
              << "      -m MONTH     Month of task (default is system time)\n"
              << "      -d DAY       Day of task (default is system time)\n"
              << "      -s HH:MM     Start time of task (default is 00:00)\n"
              << "      -e HH:MM     End time of task (default is 23:59)\n"
              << "      -x           Task is done (default is false)\n"
              << '\n'
              << "  remove <IDS>     Remove task(s)\n"
              << '\n'
              << "Only the first character of these commands is counted.\n";
    return EXIT_FAILURE;
}


bool stoi(const std::string &str, int &value) {
    /* Конвертировать строку в число функцией std::stoi с выводом ошибок */
    try {
        value = std::stoi(str);
        return true;
    } catch (std::invalid_argument &error) {
        std::cerr << "Invalid argument: '" << str << "' cannot be converted to a number" << std::endl;
        return false;
    }
}


int int_width(int n) {
    /* Посчитать количество цифр в числе n */
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
    bool undone_only = false;
    bool colorize = false;

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
                case 'x':
                    undone_only = true;
                    arg++;
                    break;
                case 'c':
                    colorize = true;
                    arg++;
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

    time_t now = time(NULL);
    tm *time_info = localtime(&now);

    Date date;

    if (year_specified)
        date.set_year(year);
    else
        date.set_year(time_info->tm_year + 1900);

    if (month_specified)
        date.set_month(month);
    else if (!year_specified || day_specified)
        date.set_month(time_info->tm_mon + 1);

    bool day_is_set = true;  // Объявляется для использования корректного предлога в выводе
    if (day_specified)
        date.set_day(day);
    else if (!(year_specified || month_specified))
        date.set_day(time_info->tm_mday);
    else
        day_is_set = false;

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

    std::vector<Task> task_vector;
    int tasks_done = 0;
    int max_task_id = 0;
    int max_priority = 0;

    database.seekg(0, std::ios_base::beg);
    bool task_read_success = true;
    while (task_read_success) {
        Task task;
        task_read_success = static_cast<bool>(database >> task);
        if (task_read_success && !(undone_only && task.done) &&
            (task.year() == date.year() &&
                (!date.month() || (task.month() == date.month() &&
                    (!date.day() || task.day() == date.day())
                ))
            )
        ) {
            task_vector.push_back(task);
            if (!undone_only && task.done) tasks_done++;
            if (task.id > max_task_id) max_task_id = task.id;
            if (task.priority > max_priority) max_priority = task.priority;
	}
    }
    database.close();

    std::sort(task_vector.begin(), task_vector.end());
    int id_width = int_width(max_task_id);
    int priority_width = int_width(max_priority);

    char prep[] = {day_is_set ? 'o' : 'i', 'n'};  // "on" day, but "in" month or year
    if (!task_vector.empty()) {
        if (undone_only) {
            std::cout << task_vector.size() << " task";
            if (task_vector.size() != 1) std::cout << 's';
            std::cout << " undone " << prep << ' ' << date.date_format() << '\n';
        } else {
            if ((int)task_vector.size() != tasks_done) std::cout << tasks_done << '/';
            std::cout << task_vector.size() << " task";
            if (tasks_done != 1) std::cout << 's';
            std::cout << " done " << prep << ' ' << date.date_format() << '\n';
        }

        if (colorize) {
            Task pivot;  // За точку отсчёта определения цвета берётся задание, начинающееся в текущее системное время
            time_t now = time(NULL);
            tm *time_info = localtime(&now);
            pivot.set_year(time_info->tm_year + 1900);
            pivot.set_month(time_info->tm_mon + 1);
            pivot.set_day(time_info->tm_mday);
            pivot.set_start(Time(time_info->tm_hour, time_info->tm_min));

            for (Task task: task_vector) {
                if (task.done) {
                    std::cout << "\033[32m" << task.format(id_width, priority_width) << "\033[0m\n";  // Зелёный
                } else {
                    pivot.id = task.id;
                    pivot.priority = task.priority;
                    if (task < pivot)
                        std::cout << "\033[31m" << task.format(id_width, priority_width) << "\033[0m\n";  // Красный
                    else
                        std::cout << task.format(id_width, priority_width) << '\n';
                }
            }
        } else {
            for (Task task: task_vector) {
                std::cout << task.format(id_width, priority_width) << '\n';
            }
        }
        return EXIT_SUCCESS;
    } else {
        std::cerr << "No tasks ";
        if (undone_only) std::cerr << "undone ";
        std::cerr << prep << ' ' << date.date_format() << std::endl;
        return EXIT_FAILURE;
    }
};


int parse_task_argument(char *argv[], int arg, int argc, Task &task) {
    int year, month, day;
    bool year_specified = false;
    bool month_specified = false;
    bool day_specified = false;

    std::ostringstream title;
    bool stream_is_empty = true;

    while (arg < argc) {
        std::string argument = argv[arg];

        if (argument[0] == '-' && argument.size() == 2) {
            switch (argv[arg][1]) {
                /* Аргументы для add и edit */
                case 'p': {
                    int priority;
                    if (!stoi(argv[arg + 1], priority))
                        return -1;
                    task.priority = priority;
                    arg += 2;
                    break;
                }
                case 'y':
                    if (!stoi(argv[arg + 1], year))
                        return -1;
                    year_specified = true;
                    arg += 2;
                    break;
                case 'm':
                    if (!stoi(argv[arg + 1], month))
                        return -1;
                    month_specified = true;
                    arg += 2;
                    break;
                case 'd':
                    if (!stoi(argv[arg + 1], day))
                        return -1;
                    day_specified = true;
                    arg += 2;
                    break;
                case 's': {
                    Time start;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> start;
                    task.set_start(start);
                    arg += 2;
                    break;
                }
                case 'e': {
                    Time end;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> end;
                    task.set_end(end);
                    arg += 2;
                    break;
                }
                case 'x':
                    task.done = not task.done;
                    arg++;
                    break;
                default:
                    std::cerr << "Unknown option: " << argv[arg] << std::endl;
                    arg++;
                    break;
            }
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
    if (year_specified) task.set_year(year);
    if (month_specified) task.set_month(month);
    if (day_specified) task.set_day(day);
    if (!stream_is_empty) task.title = title.str();
    return 0;
}


int add_task(int argc, char *argv[]) {
    if (argc <= 2)
        return usage(argv[0]);

    time_t now = time(NULL);
    tm *time_info = localtime(&now);

    Task task;
    task.set_year(time_info->tm_year + 1900);

    if (parse_task_argument(argv, 2, argc, task) < 0)
        return EXIT_FAILURE;

    if (task.title.empty())
        return usage(argv[0]);

    if (!task.month()) task.set_month(time_info->tm_mon + 1);
    if (!task.day()) task.set_day(time_info->tm_mday);

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
        } while (ch != '\n');  // Поиск последнего переноса строки

        int task_id;
        database >> task_id;
        task.id = task_id + 1;
    } else {
        task.id = 1;
    }

    bool result = static_cast<bool>(database << task);
    database.close();
    if (result) {
        std::cout << "Task successfully added:\n" << task.format() << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cerr << "Can't add task." << std::endl;
        return EXIT_FAILURE;
    }
}


int edit_task(int argc, char *argv[]) {
    if (argc <= 2)
        return usage(argv[0]);

    std::unordered_set<int> task_ids;
    int options_start = 2;
    int task_id;
    while (argv[options_start][0] != '-') {
        if (!stoi(argv[options_start], task_id))
            return EXIT_FAILURE;
        task_ids.insert(task_id);
        options_start++;
        if (options_start >= argc)
            return usage(argv[0]);
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

    bool task_read_success = true;
    int edited_count = 0;
    while (task_read_success) {
        Task task;
        task_read_success = static_cast<bool>(database >> task);
        if (task_read_success) {
            if (task_ids.find(task.id) != task_ids.end()) {
                Date old_date = task;
                Date new_date;
                new_date.set_year(task.year());
                task.Date::operator = (new_date);  // Сброс значений месяца и дня для их изменения
                if (parse_task_argument(argv, options_start, argc, task) < 0)
                    return EXIT_FAILURE;
                if (!task.month()) task.set_month(old_date.month());  // Установка неизменённых значений
                if (!task.day()) task.set_day(old_date.day());
                edited_count++;
            }
            temp_database << task;
        }
    }
    database.close();
    temp_database.close();

    if (edited_count > 0) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        std::cout << edited_count << " task";
        if (edited_count != 1) std::cout << 's';
        std::cout << " successfully edited." << std::endl;
        return EXIT_SUCCESS;
    } else {
        remove(temp_database_name);
        std::cerr << "No tasks edited." << std::endl;
        return EXIT_FAILURE;
    }
};


int remove_task(int argc, char *argv[]) {
    if (argc <= 2)
        return usage(argv[0]);

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

    bool task_read_success = true;
    int removed_count = 0;
    while (task_read_success) {
        Task task;
        task_read_success = static_cast<bool>(database >> task);
        if (task_read_success) {
            if (task_ids.find(task.id) == task_ids.end())
                temp_database << task;
            else
                removed_count++;
        }
    }
    database.close();
    temp_database.close();

    if (removed_count > 0) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        std::cout << removed_count << " task";
        if (removed_count != 1) std::cout << 's';
        std::cout << " successfully removed." << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cerr << "No tasks removed." << std::endl;
        return EXIT_FAILURE;
    }
};


int main(int argc, char *argv[]) {
    if (argc < 2)
        return usage(argv[0]);

    try {
        switch (argv[1][0]) {
            case 's': return show_tasks(argc, argv);
            case 'a': return add_task(argc, argv);
            case 'e': return edit_task(argc, argv);
            case 'r': return remove_task(argc, argv);
            default:  return usage(argv[0]);
        }
    } catch (std::invalid_argument &error) {
        std::cerr << "Invalid argument: " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
}
