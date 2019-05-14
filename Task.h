#include "Date.h"
#include "Time.h"
#include <sstream>


struct Task: Date {
    private:
        Time start;
        Time end;

    public:
        int id;
        std::string title;

        Task();
        void set_start(Time&);
        void set_end(Time&);
        std::string format();
        friend std::istream& operator >> (std::istream&, Task&);
        friend std::ostream& operator << (std::ostream&, Task&);
};
