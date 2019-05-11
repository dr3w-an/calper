#include "Date.h"
#include <iomanip>


Date::Date() {
    time_t now = time(0);
    tm *date = localtime(&now);
    year = date->tm_year + 1900;
    month = date->tm_mon + 1;
    day = date->tm_mday;
}

Date::Date(int y, int m, int d) {
    set_month(m);
    set_day(d);
    year = y;
}

void Date::set_year(int y) {
    year = y;
}

void Date::set_month(int m) {
    if (m < 1 || m > 12)
        throw std::invalid_argument("month must be a number in range from 1 to 12");
    else
        month = m;
}

void Date::set_day(int d) {
    if (d < 1 || d > 31)
        throw std::invalid_argument("day must be a number in range from 1 to 31");
    else
        day = d;
}

bool Date::is_date_equal(Date& date) {
    return (year == date.year && month == date.month && day == date.day);
}

std::string Date::date_format() {
    std::ostringstream stream;
    stream << year << '-'
           << std::setfill('0') << std::setw(2) << month << '-'
           << std::setfill('0') << std::setw(2) << day;
    return stream.str();
}
