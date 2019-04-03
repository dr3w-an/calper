#include <iostream>


void show_task(int argc, char *argv[]) {
}


void add_task(int argc, char *argv[]) {
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
