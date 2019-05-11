#include <string>


struct Date {
    protected:
        int year;
        int month;
        int day;

    public:
        Date();
        Date(int d, int m, int y);
        void set_year(int y);
        void set_month(int m);
        void set_day(int d);
        bool is_date_equal(Date&);
        std::string date_format();
};
