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

        void set_start(const Time &);
        void set_end(const Time &);

        std::string format() const;

        bool operator < (const Task &) const;
        friend std::istream &operator >> (std::istream &, Task &);
        friend std::ostream &operator << (std::ostream &, const Task &);
};
