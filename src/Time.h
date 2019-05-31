#include <iostream>


class Time {
    private:        // Допустимые значения:
        int hours;  // от 0 до 23
        int min;    // от 0 до 59

    public:
        Time() = default;
        Time(int hours, int min);

        std::string format() const;

        bool operator <  (const Time &) const;
        bool operator == (const Time &) const;
        friend std::istream &operator >> (std::istream &, Time &);
};
