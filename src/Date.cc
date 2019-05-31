#include "Date.h"
#include <iomanip>


Date::Date():
    _month(0), _day(0)
{}


void Date::set_year(int year) {
    if (year < 1) {
        throw std::invalid_argument("year must be a number greater or equal than 1");
    } else if (year != _year) {
        _year = year;
        if (_day) set_day(_day);
    }
}


void Date::set_month(int month) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("month must be a number in range from 1 to 12");
    } else if (month != _month) {
        _month = month;
        if (_day) set_day(_day);
    }
}


void Date::set_day(int day) {
    int max_limit;

    switch (_month) {
        case 2:
            if (_year % 400 == 0 || (_year % 4 == 0 && _year % 100 != 0)) {  // Проверка на високосный год
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

    if (day < 1 || day > max_limit) {
        std::ostringstream error_message("day must be a number in range from 1 to ", std::ios::app);
        error_message << max_limit;
        throw std::invalid_argument(error_message.str());
    } else {
        _day = day;
    }
}


std::string Date::date_format() const {
    std::ostringstream stream;
    stream << _year;
    if (_month) stream << '-' << std::setfill('0') << std::setw(2) << _month;
    if (_day) stream << '-' << std::setfill('0') << std::setw(2) << _day;
    return stream.str();
}


Date &Date::operator = (const Date &date) {
    _year = date._year;
    _month = date._month;
    _day = date._day;
    return *this;
}
