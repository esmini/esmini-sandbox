#define MCAP_IMPLEMENTATION  // Define this in exactly one .cpp file
#include <mcap/writer.hpp>
#include "mcap/reader.hpp"

#include <chrono>
#include <cstring>
#include <iostream>

// Returns the system time in nanoseconds. std::chrono is used here, but any
// high resolution clock API (such as clock_gettime) can be used.
mcap::Timestamp now() {
  return mcap::Timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count());
}

using mcap::ByteOffset;

static std::string to_string(const std::string& arg) {
  return arg;
}
static std::string to_string(std::string_view arg) {
  return std::string(arg);
}
static std::string to_string(const char* arg) {
  return std::string(arg);
}
template <typename... T>
[[nodiscard]] static std::string StrCat(T&&... args) {
  using ::to_string;
  using std::to_string;
  return ("" + ... + to_string(std::forward<T>(args)));
}

std::string ToString(const mcap::Message& message) {
  return StrCat("[Message] channel_id=", message.channelId, ", sequence=", message.sequence,
                ", publish_time=", message.publishTime, ", log_time=", message.logTime, ", data=<",
                message.dataSize, " bytes>");
}

int writerFile()
{
  // Initialize an MCAP writer with the "ros1" profile and write the file header
  mcap::McapWriter writer;
  auto status = writer.open("output.mcap", mcap::McapWriterOptions("ros1"));
  if (!status.ok()) {
    std::cerr << "Failed to open MCAP file for writing: " << status.message << "\n";
    return 1;
  }

  // Register a Schema
  mcap::Schema stdMsgsString("std_msgs/String", "ros1msg", "string data");
  writer.addSchema(stdMsgsString);

  // Register a Channel
  mcap::Channel chatterPublisher("/chatter", "ros1", stdMsgsString.id);
  writer.addChannel(chatterPublisher);

  // Create a message payload. This would typically be done by your own
  // serialization library. In this example, we manually create ROS1 binary data
  std::array<std::byte, 4 + 13> payload;
  const uint32_t length = 13;
  std::memcpy(payload.data(), &length, 4);
  std::memcpy(payload.data() + 4, "Hello, world!", 13);

  // Write our message
  mcap::Message msg;
  msg.channelId = chatterPublisher.id;
  msg.sequence = 1; // Optional
  msg.logTime = now(); // Required nanosecond timestamp
  msg.publishTime = msg.logTime; // Set to logTime if not available
  msg.data = payload.data();
  msg.dataSize = payload.size();
  auto res = writer.write(msg);
  if (!res.ok()) {
    std::cerr << "failed to write message: " << res.message << std::endl;
    writer.close();
    return 1;
  }


  std::memcpy(payload.data() + 4, "Hello, world!", 13);

  // Write our message
  msg.channelId = chatterPublisher.id;
  msg.sequence = 1; // Optional
  msg.logTime = now(); // Required nanosecond timestamp
  msg.publishTime = msg.logTime; // Set to logTime if not available
  msg.data = payload.data();
  msg.dataSize = payload.size();
  res = writer.write(msg);
  if (!res.ok()) {
    std::cerr << "failed to write message: " << res.message << std::endl;
    writer.close();
    return 1;
  }
  // Finish writing the file
  writer.close();
  return 0;
}

int readFile()
{
    const char* inputFilename ="output.mcap";

    mcap::McapReader reader;
    {
      const auto res = reader.open(inputFilename);
      if (!res.ok()) {
        std::cerr << "Failed to open " << inputFilename << " for reading: " << res.message
                  << std::endl;
        return 1;
      }
    }

    auto messageView = reader.readMessages();

    for (const auto& msgView : messageView) {
        const mcap::Channel& channel = *msgView.channel;
        std::cout << "[" << channel.topic << "] " << ToString(msgView.message) << "\n";
      }

    reader.close();
    return 0;
  }

int main()
{
    std::cout << "mcap logger 0.1" << std::endl;
    writerFile();
    std::cout << "mcap file created." << std::endl;
    readFile();
    std::cout << "mcap file read." << std::endl;
    std::cout << "mcap logger finished." << std::endl;
    return 0;
}