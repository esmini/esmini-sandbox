#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

// Portable implementation of timegm
namespace {
    time_t portable_timegm(struct tm *tm) {
        time_t ret;
        char *tz;

        tz = getenv("TZ");
        setenv("TZ", "", 1);
        tzset();
        ret = mktime(tm);
        if (tz)
            setenv("TZ", tz, 1);
        else
            unsetenv("TZ");
        tzset();
        return ret;
    }
}

uint64_t GetEpochTimeFromString(const std::string& datetime) {
    std::tm tm = {};
    double milliseconds = 0.0;
    int tz_hour = 0, tz_min = 0;
    char sign = '+';
    char delimiter = ':';

    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    if (ss.peek() == '.') {
        ss.ignore() >> milliseconds;
    }

    // Parse timezone offset
    if (ss >> sign >> tz_hour >> delimiter >> tz_min) {
        int offset = (tz_hour * 3600 + tz_min * 60) * (sign == '+' ? 1 : -1);
        // We'll apply the offset after converting to epoch time
    }

    // Convert to epoch time (treating tm as UTC)
    std::time_t time;
    #ifdef _WIN32
        time = _mkgmtime(&tm);  // Windows-specific
    #else
        time = portable_timegm(&tm);  // Our portable implementation
    #endif

    // Apply the timezone offset from the string
    if (ss) {
        int offset = (tz_hour * 3600 + tz_min * 60) * (sign == '+' ? 1 : -1);
        time -= offset;  // Subtract because we're converting to UTC
    }

    auto tp = std::chrono::system_clock::from_time_t(time) +
              std::chrono::milliseconds(static_cast<int64_t>(milliseconds));

    return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
}

int main() {
    // Example input string
    std::string datetime = "2023-11-15T12:00:00.000+00:00";
    std::cout << "Input datetime: " << datetime << std::endl;
    std::time_t epoch = GetEpochTimeFromString(datetime);
    std::cout << "Unix epoch time: " << epoch << std::endl;

    // Should now give consistent results regardless of local timezone
    std::cout << "----------------------------- " << std::endl;
    datetime = "2023-11-15T12:00:00.000+01:00";  // UTC+1
    std::cout << "Input datetime: " << datetime << std::endl;
    epoch = GetEpochTimeFromString(datetime);
    std::cout << "Unix epoch time: " << epoch << std::endl;

    return 0;
}