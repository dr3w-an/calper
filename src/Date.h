#include <string>


class Date {
    protected:      // Допустимые значения (включительно):
        int year;   // от 1
        int month;  // от 1 до 12
        int day;    // от 1 до максимального количества дней в заданном месяце

    public:
        Date();

        void set_year(int);
        void set_month(int);
        void set_day(int);
        void set_year();   // Сеттеры без аргументов
        void set_month();  // устанавливают значения
        void set_day();    // текущего системного времени
        void today();

        bool is_date_equal(const Date &) const;
        std::string date_format() const;
};
