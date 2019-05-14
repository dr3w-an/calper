#include <iostream>


struct Time {
    private:
        int hours;
        int min;

    public:
        Time() = default;
        Time(int h, int m);
        std::string format();
        friend bool operator >= (Time& time_1, Time& time_2);
        friend bool operator <= (Time& time_1, Time& time_2);
        friend std::istream& operator >> (std::istream& stream, Time& time);
        friend std::ostream& operator << (std::ostream& stream, Time& time);
};
