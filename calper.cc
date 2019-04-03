#include "calper.h"

Date::Date() {
    time_t now = time(0);
    tm *date = localtime(&now);
    day = date->tm_mday;
    month = date->tm_mon + 1;
    year = date->tm_year + 1900;
}

Date::Date(int d, int m, int y):
    day(d), month(m), year(y)
{}

std::string Date::format(char specifier = 'D') {
    std::ostringstream stream;
    switch (specifier) {
        case 'F':
            stream << day << "." << month << "." << year;
            break;
        case 'D':
        default:
            stream << year << "-" << month << "-" << day;
            break;
    }
    return stream.str();
}
