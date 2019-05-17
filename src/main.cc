#include "main.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
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
              << "\t-d DAY                 Day of task (default is system time)\n"
              << "\t-m MONTH               Month of task (default is system time)\n"
              << "\t-y YEAR                Year of task (default is system time)\n"
              << "\t-s HH:MM               Start time of task (default is 00:00)\n"
              << "\t-e HH:MM               End time of task (default is 23:59)\n"
              << "\t-x                     Task is done (default is false)"
              << std::endl;
    return 1;
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
                    year = atoi(argv[arg + 1]);
                    year_specified = true;
                    arg += 2;
                    break;
                case 'm':
                    month = atoi(argv[arg + 1]);
                    month_specified = true;
                    arg += 2;
                    break;
                case 'd':
                    day = atoi(argv[arg + 1]);
                    day_specified = true;
                    arg += 2;
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
        return 1;
    }
    if ((int)database.tellg() == 0) {
        std::cerr << "Database is empty." << std::endl;
        return 1;
    }

    std::vector<Task> task_vector = {};
    int max_task_id = 0;

    database.seekg(0, std::ios_base::beg);
    bool task_read = true;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (task_read && task.is_date_equal(date)) {
            task_vector.push_back(task);
            if (task.id > max_task_id) max_task_id = task.id;
	}
    }
    database.close();

    std::sort(task_vector.begin(), task_vector.end());

    int id_space = 0;
    while (max_task_id) {
        max_task_id /= 10;
        id_space++;
    }

    if (!task_vector.empty()) {
        std::cout << "Tasks on " << date.date_format() << '\n';
        for (Task task: task_vector) {
            char mark = task.done ? 'x' : ' ';
            std::cout << '[' << mark << "] " << std::setw(id_space) << task.id << ' '
                      << task.format() << ' ' << task.title << '\n';
        }
        return 0;
    } else {
        std::cerr << "Can't get tasks on " << date.date_format() << '.' << std::endl;
        return 1;
    }
};


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
            switch (argument[1]) {
                case 'y':
                    task.set_year(atoi(argv[arg + 1]));
                    arg += 2;
                    break;
                case 'm':
                    task.set_month(atoi(argv[arg + 1]));
                    arg += 2;
                    break;
                case 'd':
                    task.set_day(atoi(argv[arg + 1]));
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
                    task.done = true;
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
        std::cout << "Task successfully added on " << task.format() << " with ID "
                  << task.id << std::endl;
    } else {
        std::cerr << "Can't add task." << std::endl;
    }

    database.close();
    return 0;
}


int edit_task(int argc, char *argv[]) {
    if (argc <= 2)
        return show_usage(argv[0]);

    int task_id;
    task_id = atoi(argv[2]);

    std::ifstream database;
    database.open(DATABASE_NAME);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return 1;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name);
    std::ostringstream title;

    bool task_read = true;
    bool task_exists = false;
    bool stream_is_empty = true;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (!task_read) break;
        if (task.id == task_id) {
            task_exists = true;
            int arg = 3;
            while (arg < argc) {
                std::string argument = argv[arg];
                if (argument[0] == '-' && argument.size() == 2) {
                    switch (argument[1]) {
                        case 'y':
                            task.set_year(atoi(argv[arg + 1]));
                            arg += 2;
                            break;
                        case 'm':
                            task.set_month(atoi(argv[arg + 1]));
                            arg += 2;
                            break;
                        case 'd':
                            task.set_day(atoi(argv[arg + 1]));
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
            if (!stream_is_empty) task.title = title.str();
        }
        temp_database << task;
    }

    if (task_exists) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        std::cout << "Task successfully edited." << std::endl;
    } else {
        std::cerr << "Can't edit task." << std::endl;
    }

    database.close();
    temp_database.close();
    return 0;
};


int remove_task(int argc, char *argv[]) {
    if (argc <= 2)
        return show_usage(argv[0]);

    int task_id;
    task_id = atoi(argv[2]);

    std::ifstream database;
    database.open(DATABASE_NAME);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return 1;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name);

    bool task_read = true;
    bool task_exists = false;
    while (task_read) {
        Task task;
        task_read = static_cast<bool>(database >> task);
        if (task_read && task.id != task_id) {
            temp_database << task;
	} else {
            task_exists = true;
        }
    }

    if (task_exists) {
        remove(DATABASE_NAME);
        rename(temp_database_name, DATABASE_NAME);
        std::cout << "Task successfully removed." << std::endl;
    } else {
        std::cerr << "Can't remove task." << std::endl;
    }

    database.close();
    temp_database.close();
    return 0;
};


int main(int argc, char *argv[]) {
    if (argc < 2)
        return show_usage(argv[0]);

    switch (argv[1][0]) {
        case 's': return show_tasks(argc, argv);
        case 'a': return add_task(argc, argv);
        case 'e': return edit_task(argc, argv);
        case 'r': return remove_task(argc, argv);
        default:  return show_usage(argv[0]);
    }
}
