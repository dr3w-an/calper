#include <string>


struct Date {
    protected:
        int year;
        int month;
        int day;

    public:
        Date();
        Date(int year, int month, int day);
        void set_year(int);
        void set_month(int);
        void set_day(int);
        bool is_date_equal(Date&);
        std::string date_format();
};
