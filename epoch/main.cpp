#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdint>

uint64_t iso8601ToEpoch(const std::string& iso8601String) {
    std::istringstream ss(iso8601String);
    std::tm t{};
    char timezoneSign;
    int timezoneHour;
    int timezoneMinute;
    char    delimiter = ':';

    // Attempt to parse the date and time part, ignoring milliseconds for simplicity
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");


    // Extract timezone information if present
    if (ss >> timezoneSign >> timezoneHour)
    {
        if (ss >> delimiter) {
            ss >> timezoneMinute;
        } else {
            timezoneMinute = 0; // Assume 0 minutes if not specified
        }

        long timezoneOffsetSeconds = (timezoneHour * 3600 + timezoneMinute * 60);
        if (timezoneSign == '-') {
            timezoneOffsetSeconds = -timezoneOffsetSeconds;
        }

        // Convert std::tm to time_t (which is usually seconds since epoch, but UTC)
        std::time_t timeSinceEpochUTC = std::mktime(&t);

        // Adjust for the timezone offset to get local epoch time
        return static_cast<uint64_t>(timeSinceEpochUTC - timezoneOffsetSeconds);
    }
    else
    {
        // If no timezone is specified, assume the time is in the local timezone
        std::time_t timeSinceEpochLocal = std::mktime(&t);
        return static_cast<uint64_t>(timeSinceEpochLocal);
    }
}

int main() {
    std::string iso8601Time = "2023-11-15T12:00:00.000+00:00";
    uint64_t epochTime = iso8601ToEpoch(iso8601Time);

    if (epochTime != 0) {
        std::cout << "ISO 8601 String: " << iso8601Time << std::endl;
        std::cout << "Epoch Time: " << epochTime << std::endl;

        // Optional: Convert the epoch time back to a human-readable format (UTC)
        std::time_t timeT = static_cast<std::time_t>(epochTime);
        std::cout << "As UTC: " << std::put_time(std::gmtime(&timeT), "%Y-%m-%d %H:%M:%S UTC") << std::endl;

        // Optional: Convert the epoch time back to a human-readable format (local timezone)
        std::cout << "As Local: " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S %Z") << std::endl;
    }

    return 0;
}