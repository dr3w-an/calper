#include "calper.h"

#include <fstream>
#include <iostream>
#include <unordered_map>


int string_to_int(char string[]) {
    int result;
    std::istringstream stringstream(string);
    stringstream >> result;
    return result;
}


void show_task(int argc, char *argv[]) {
}


void add_task(int argc, char *argv[]) {
    struct Task task;
    struct Date date;

    int arg = 2;
    std::ostringstream title;
    bool stream_is_empty = true;
    while (arg < argc - 1) {
        std::string argument = argv[arg];

        if (argument == "--day") {
            date.day = string_to_int(argv[arg + 1]);
            arg += 2;
        } else if (argument == "--month") {
            date.month = string_to_int(argv[arg + 1]);
            arg += 2;
        } else if (argument == "--year") {
            date.year = string_to_int(argv[arg + 1]);
            arg += 2;
        } else if (argument == "--start") {
            std::tm start = {};
            std::istringstream stream(argv[arg + 1]);
            if (stream >> std::get_time(&start, "%H:%M"))
                task.start_time = start;
            arg += 2;
        } else if (argument == "--end") {
            std::tm end = {};
            std::istringstream stream(argv[arg + 1]);
            if (stream >> std::get_time(&end, "%H:%M"))
                task.end_time = end;
            arg += 2;
        } else {
            if (stream_is_empty) {
                title << argv[arg];
                stream_is_empty = false;
            } else {
                title << ' ' << argv[arg];
            }
            arg++;
        }
    }
    task.title = title.str();
    task.date = date;
}


void edit_task(int argc, char *argv[]) {
}


void remove_task(int argc, char *argv[]) {
}


void show_usage(char *program_name) {
    std::cerr << "Usage: " << program_name << " <command> [options]\n\n"
              << "Commands:\n"
              << "\tshow [options]          Show calendar\n"
              << "\tadd <TITLE> [options]   Add task\n"
              << "\tedit <ID> [options]     Edit task\n"
              << "\tremove <ID>             Remove task\n"
              << "\n"
              << "Options:\n"
              << "\t--day DAY\n"
              << "\t--month MONTH\n"
              << "\t--year YEAR\n"
              << "\t--start HH:MM\n"
              << "\t--end HH:MM\n"
              << std::endl;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    if (command == "show") {
        show_task(argc, argv);
        return 0;
    } else if (command == "add") {
        add_task(argc, argv);
        return 0;
    } else if (command == "edit") {
        edit_task(argc, argv);
        return 0;
    } else if (command == "remove") {
        remove_task(argc, argv);
        return 0;
    } else {
        show_usage(argv[0]);
        return 1;
    }
}
