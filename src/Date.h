#include <string>


class Date {
    protected:
        int year;
        int month;
        int day;

    public:
        Date();

        void today();
        void set_year();
        void set_year(int);
        void set_month();
        void set_month(int);
        void set_day();
        void set_day(int);

        bool is_date_equal(const Date &) const;
        std::string date_format() const;
};
