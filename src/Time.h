#include <iostream>


class Time {
    int hours;
    int min;

    public:
        Time() = default;
        Time(int hours, int min);

        std::string format() const;

        friend bool operator >= (Time &, Time &);
        friend bool operator <= (Time &, Time &);
        friend std::istream &operator >> (std::istream &, Time &);
        friend std::ostream &operator << (std::ostream &, Time &);
};
