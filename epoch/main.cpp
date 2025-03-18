#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// Function to convert tm to time_t in UTC
std::time_t tmToTimeTUTC(const std::tm& tm) {
#ifdef _WIN32
    // Windows: Use _mkgmtime
    return _mkgmtime(&const_cast<std::tm&>(tm));
#else
    // Unix-like systems: Use timegm
    return timegm(&const_cast<std::tm&>(tm));
#endif
}

std::time_t stringToEpoch(const std::string& datetime) {
    // Define the format of the input string
    const char* format = "%Y-%m-%dT%H:%M:%S";

    // Create a stringstream to parse the input string
    std::istringstream ss(datetime);

    // Create a tm struct to hold the parsed date and time
    std::tm tm = {};

    // Parse the date and time from the string
    ss >> std::get_time(&tm, format);

    // If parsing fails, return -1
    if (ss.fail()) {
        return -1;
    }

    // Extract milliseconds and timezone offset
    double milliseconds = 0;
    int timezoneOffset = 0;
    char dot, sign;
    std::string tzHour, tzMin;

    // Read the milliseconds and timezone offset
    ss >> dot >> milliseconds >> sign >> std::setw(2) >> tzHour >> std::setw(2) >> tzMin;

    // Convert timezone offset to seconds
    timezoneOffset = std::stoi(tzHour) * 3600 + std::stoi(tzMin) * 60;

    // Adjust the timezone offset based on the sign
    if (sign == '-') {
        timezoneOffset = -timezoneOffset;
    }

    // Debug: Print the parsed tm struct
    std::cout << "Parsed tm struct: "
              << tm.tm_year + 1900 << "-" << tm.tm_mon + 1 << "-" << tm.tm_mday << " "
              << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << std::endl;

    // Debug: Print the timezone offset
    std::cout << "Timezone offset: " << timezoneOffset << " seconds" << std::endl;

    // Treat the tm struct as UTC time by adjusting for the timezone offset
    std::time_t utcEpoch = tmToTimeTUTC(tm) - timezoneOffset;

    // Add milliseconds to the epoch time
    utcEpoch += static_cast<time_t>(milliseconds / 1000.0);

    return utcEpoch;
}

int main() {
    // Example input string
    std::string datetime = "2023-11-15T10:30:00.123+0530";

    // Convert the string to Unix epoch time
    std::time_t epoch = stringToEpoch(datetime);

    if (epoch == -1) {
        std::cerr << "Failed to parse the date and time string." << std::endl;
        return 1;
    }

    // Output the Unix epoch time
    std::cout << "Unix epoch time: " << epoch << std::endl;

    return 0;
}