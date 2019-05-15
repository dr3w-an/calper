#include <iostream>


class Time {
    int hours;
    int min;

    public:
        Time() = default;
        Time(int hours, int min);

        std::string format() const;

        bool operator == (const Time &) const;
        bool operator >= (const Time &) const;
        bool operator <  (const Time &) const;
        bool operator <= (const Time &) const;
        friend std::istream &operator >> (std::istream &, Time &);
        friend std::ostream &operator << (std::ostream &, Time &);
};
