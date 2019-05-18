#include "Date.h"
#include <iomanip>


Date::Date():
    month(0), day(0)
{}


void Date::today() {
    set_year();
    set_month();
    set_day();
}


tm *current_tm() {
    time_t now = time(NULL);
    return localtime(&now);
}


void Date::set_year() {
    tm *date = current_tm();
    year = date->tm_year + 1900;
}


void Date::set_year(int y) {
    if (y < 1) {
        throw std::invalid_argument("year must be a number greater or equal than 1");
    } else if (y != year) {
        year = y;
        if (day != 0) set_day(day);
    }
}


void Date::set_month() {
    tm *date = current_tm();
    month = date->tm_mon + 1;
}


void Date::set_month(int m) {
    if (m < 1 || m > 12) {
        throw std::invalid_argument("month must be a number in range from 1 to 12");
    } else if (m != month) {
        month = m;
        if (day != 0) set_day(day);
    }
}


void Date::set_day() {
    tm *date = current_tm();
    day = date->tm_mday;
}


void Date::set_day(int d) {
    int max_limit;

    switch (month) {
        case 2:
            if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0))) {
                max_limit = 29;
            } else {
                max_limit = 28;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            max_limit = 30;
            break;
        default:
            max_limit = 31;
            break;
    }

    if (d < 1 || d > max_limit) {
        std::ostringstream error_message("day must be a number in range from 1 to ");
        error_message << max_limit;
        throw std::invalid_argument(error_message.str());
    } else {
        day = d;
    }
}


bool Date::is_date_equal(const Date &date) const {
    return (year == date.year && (date.month == 0 || (month == date.month && (date.day == 0 || day == date.day))));
}


std::string Date::date_format() const {
    std::ostringstream stream;
    stream << year;
    if (month != 0)
        stream << '-' << std::setfill('0') << std::setw(2) << month;
    if (day != 0)
        stream << '-' << std::setfill('0') << std::setw(2) << day;
    return stream.str();
}
