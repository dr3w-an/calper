#include "main.h"
#include <ctime>
#include <fstream>
#include <iomanip>


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
              << "\t-e HH:MM               End time of task (default is 23:59)"
              << std::endl;
    return 1;
}


int string_to_int(char string[]) {
    int result;
    std::istringstream stringstream(string);
    stringstream >> result;
    return result;
}


int show_tasks(int argc, char *argv[]) {
    int year, month, day;
    bool year_specified = false;
    bool month_specified = false;
    bool day_specified = false;

    int arg = 2;
    while (arg < argc) {
        std::string argument = argv[arg];

        if (argument.rfind("-", 0) != 0) {
            arg++;
            continue;
        }

        if (argument == "-y") {
            year = string_to_int(argv[arg + 1]);
            year_specified = true;
            arg += 2;
        } else if (argument == "-m") {
            month = string_to_int(argv[arg + 1]);
            month_specified = true;
            arg += 2;
        } else if (argument == "-d") {
            day = string_to_int(argv[arg + 1]);
            day_specified = true;
            arg += 2;
        }
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
    database.open(DATABASE_NAME, std::ios::binary | std::ios::ate);
    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return 1;
    }
    if ((int)database.tellg() == 0) {
        std::cerr << "Database is empty." << std::endl;
        return 1;
    }

    char ch;
    do {
        database.seekg(-2, std::ios_base::cur);
        if ((int)database.tellg() <= 1) {
            database.seekg(0, std::ios_base::beg);
            break;
        }
        database.get(ch);
    } while (ch != '\n');

    int task_id;
    database >> task_id;

    int digits = 0;
    while (task_id) {
        task_id /= 10;
        digits++;
    }

    Task task;
    std::ostringstream stream;
    database.seekg(0, std::ios_base::beg);
    while (database >> task) {
        if (task.is_date_equal(date)) {
            stream << std::setw(digits) << task.id << ' ' << task.format() << ' '
                   << task.title << '\n';
	}
    }
    database.close();

    std::string task_list = stream.str();
    if (!task_list.empty()) {
        std::cout << "Tasks on " << date.date_format() << '\n' << task_list;
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

        if (argument.rfind("-", 0) == 0) {
            if (arg + 1 < argc) {
                if (argument == "-y") {
                    task.set_year(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-m") {
                    task.set_month(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-d") {
                    task.set_day(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-s") {
                    Time start;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> start;
                    task.set_start(start);
                    arg += 2;
                } else if (argument == "-e") {
                    Time end;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> end;
                    task.set_end(end);
                    arg += 2;
                }
            }
            continue;
        }
        if (stream_is_empty) {
            title << argv[arg];
            stream_is_empty = false;
        } else {
            title << ' ' << argv[arg];
        }
        arg++;
    }

    if (stream_is_empty)
        return show_usage(argv[0]);

    task.title = title.str();

    std::fstream database;
    database.open(DATABASE_NAME, std::ios::binary | std::ios::ate | std::ios::app | std::ios::in | std::ios::out);

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

    std::ifstream database;
    database.open(DATABASE_NAME, std::ios::binary);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return 1;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name, std::ios::binary);

    int task_id;
    task_id = string_to_int(argv[2]);

    Task task;
    bool task_exists = false;
    while (database >> task) {
        if (task.id == task_id) {
            task_exists = true;

            int arg = 2;
            while (arg < argc) {
                std::string argument = argv[arg];

                if (argument.rfind("-", 0) != 0) {
                    arg++;
                    continue;
                }

                if (argument == "-y") {
                    task.set_year(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-m") {
                    task.set_month(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-d") {
                    task.set_day(string_to_int(argv[arg + 1]));
                    arg += 2;
                } else if (argument == "-s") {
                    Time start;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> start;
                    task.set_start(start);
                    arg += 2;
                } else if (argument == "-e") {
                    Time end;
                    std::istringstream stream(argv[arg + 1]);
                    stream >> end;
                    task.set_end(end);
                    arg += 2;
                }
            }
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

    std::ifstream database;
    database.open(DATABASE_NAME, std::ios::binary);

    if (!database) {
        std::cerr << "Database can't be accessed." << std::endl;
        return 1;
    }

    const char temp_database_name[] = ".~" DATABASE_NAME;
    std::ofstream temp_database;
    temp_database.open(temp_database_name, std::ios::binary);

    int task_id;
    task_id = string_to_int(argv[2]);

    Task task;
    bool task_exists = false;
    while (database >> task) {
        if (task.id != task_id) {
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

    std::string command = argv[1];
    if (command == "show")
        return show_tasks(argc, argv);
    else if (command == "add")
        return add_task(argc, argv);
    else if (command == "edit")
        return edit_task(argc, argv);
    else if (command == "remove")
        return remove_task(argc, argv);
    else
        return show_usage(argv[0]);
}
