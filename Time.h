#include <iostream>


struct Time {
    private:
        int hours;
        int min;

    public:
        Time(int h, int m);
        std::string format();
        friend std::istream& operator >> (std::istream& stream, Time& time);
        friend std::ostream& operator << (std::ostream& stream, Time& time);
};
