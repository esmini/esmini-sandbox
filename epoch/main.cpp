#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

uint64_t GetEpochTimeFromString(const std::string& datetime)
{
    std::tm tm           = {};
    double  milliseconds = 0.0;
    int     tz_hour = 0, tz_min = 0;
    char    sign      = '+';
    char    delimiter = ':';

    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    if (ss.peek() == '.')
    {
        ss.ignore() >> milliseconds;
    }

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm)) + std::chrono::milliseconds(static_cast<int64_t>(milliseconds));
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}

int main() {
    // Example input string
    std::string datetime = "2023-11-15T12:00:00.000+00:00";
    std::time_t epoch = GetEpochTimeFromString(datetime);
    std::cout << "Unix epoch time: " << epoch << std::endl;

    datetime = "2023-11-15T12:00:00.000+01:00";
    epoch = GetEpochTimeFromString(datetime);
    std::cout << "Unix epoch time: " << epoch << std::endl;

    return 0;
}