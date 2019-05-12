#include "Date.h"
#include "Time.h"
#include <sstream>


struct Task: Date {
    int id;
    Time start;
    Time end;
    std::string title;

    Task();
    void display();
    friend std::istream& operator >> (std::istream&, Task&);
    friend std::ostream& operator << (std::ostream&, Task&);
};
