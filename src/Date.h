#include <string>


class Date {
    protected:       // Присваемое значение при инициализации:
        int _year;   // значение не присваивается
        int _month;  // 0
        int _day;    // 0

    public:
        Date();

        int year() { return _year; };
        int month() { return _month; };
        int day() { return _day; };
        void set_year(int);   // Допустимые значения: от 1
        void set_month(int);  // Допустимые значения: от 1 до 12
        void set_day(int);    // Допустимые значения: от 1 до максимального количества дней в установленном месяце или 31, если месяц не установлен

        std::string date_format() const;

        Date &operator = (const Date &);
};
