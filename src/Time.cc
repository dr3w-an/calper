#include "Time.h"
#include <iomanip>


Time::Time(int h, int m) {
    if (h < 0 || h > 23)
        throw std::invalid_argument("hours must be a number in range from 0 to 23");
    else
        hours = h;

    if (m < 0 || m > 59)
        throw std::invalid_argument("min must be a number in range from 0 to 59");
    else
        min = m;
}


std::string Time::format() const {
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(2) << hours << ':'
           << std::setfill('0') << std::setw(2) << min;
    return stream.str();
}


bool Time::operator == (const Time &time) const {
    return (hours == time.hours && min == time.min);
}


bool Time::operator >= (const Time &time) const {
    return (hours > time.hours || (hours == time.hours && min >= time.min));
}


bool Time::operator < (const Time &time) const {
    return (hours < time.hours || (hours == time.hours && min < time.min));
}


bool Time::operator <= (const Time &time) const {
    return (hours < time.hours || (hours == time.hours && min <= time.min));
}


std::istream& operator >> (std::istream &stream, Time &time) {
    char separator;
    int hours, min;

    stream >> hours;
    stream >> separator;
    stream >> min;

    time = Time(hours, min);
    return stream;
}
