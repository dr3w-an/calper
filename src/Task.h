#include "Date.h"
#include "Time.h"
#include <sstream>


class Task: public Date {
    Time start;
    Time end;

    public:
        int id;
        bool done;
        std::string title;

        Task();

        void set_start(Time &);
        void set_end(Time &);

        std::string format() const;

        bool operator < (const Task &) const;
        friend std::istream &operator >> (std::istream &, Task &);
        friend std::ostream &operator << (std::ostream &, Task &);
};