#define MCAP_IMPLEMENTATION
#include <mcap/writer.hpp>
#include <mcap/reader.hpp>
#include <iostream>
#include <chrono>

mcap::Timestamp now() {
  return mcap::Timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count());
}

std::string leveltoString(mcap::CompressionLevel level) {
  switch (level) {
    case mcap::CompressionLevel::Fastest: return "Fastest";
    case mcap::CompressionLevel::Fast: return "Fast";
    case mcap::CompressionLevel::Default: return "Default";
    case mcap::CompressionLevel::Slow: return "Slow";
    case mcap::CompressionLevel::Slowest: return "Slowest";
    default: return "Unknown";
  }
}
int WriteFile(bool use_compression, mcap::CompressionLevel compression_level)
  // Start the timer
{
    auto start = std::chrono::high_resolution_clock::now(); // Start timer
    std::string filename = use_compression ? "vehicle_compressed.mcap" : "vehicle_uncompressed.mcap";
    std::cout << "\n=== " << filename << " ===\n";
    // Initialize an MCAP writer with the "ros1" profile and write the file header
    mcap::McapWriter writer;
    mcap::McapWriterOptions options("ros2");
    options.compression = use_compression ? mcap::Compression::Zstd : mcap::Compression::None;
    options.compressionLevel = compression_level;
    std::cout << "Compression level: " << leveltoString(compression_level) << "\n";
    options.noChunkCRC = true;
    options.noSummaryCRC = true;
    options.noChunking = false;
    auto status = writer.open(filename, options);
    if (!status.ok()) {
      std::cerr << "Failed to open MCAP file for writing: " << status.message << "\n";
      return 1;
    }

    // Register a Schema for double value
    mcap::Schema stdMsgsDouble("std_msgs/Double", "ros1msg", "float64 data");
    writer.addSchema(stdMsgsDouble);

    // Register a Channel
    mcap::Channel chatterPublisher("/chatter", "ros1", stdMsgsDouble.id);
    writer.addChannel(chatterPublisher);

    // Create a message payload. This would typically be done by your own
    // serialization library. In this example, we manually create ROS1 binary data

    for (int i = 0; i < 200; i++)
    {
      double speed = 50.0 + (i * 0.01); // Example speed value
      std::array<std::byte, sizeof(double)> payload;
      std::memcpy(payload.data(), &speed, sizeof(double));
      // Write our message
      mcap::Message msg;
      msg.channelId = chatterPublisher.id;
      msg.sequence = i + 1; // Optional, increment sequence
      msg.logTime = now(); // Required nanosecond timestamp
      msg.publishTime = msg.logTime; // Set to logTime if not available
      msg.data = payload.data();
      msg.dataSize = static_cast<uint32_t>(payload.size());
      const auto res = writer.write(msg);
      if (!res.ok()) {
      std::cerr << "Failed to write message: " << res.message << "\n";
      writer.terminate();
      std::ignore = std::remove("output.mcap");
      return 1;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();   // End timer
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Wrote to " << filename << " in " << duration.count() << " ms\n";
    // Finish writing the file
    writer.close();
  return 0;
}

void AnalyzeFile(const std::string& filename, bool expect_compression) {
  // 1. Get accurate file size
  std::ifstream in(filename, std::ios::binary | std::ios::ate);
  size_t file_size = in.tellg();
  in.close();

  // 2. Read MCAP file properly
  mcap::McapReader reader;
  auto status = reader.open(filename);
  if (!status.ok()) {
    std::cerr << "Failed to open file\n";
    return;
  }

  // 3. Calculate raw message sizes
  size_t total_message_size = 0;
  size_t message_count = 0;
  std::map<size_t, size_t> size_distribution;

  for (const auto& msg : reader.readMessages()) {
    total_message_size += msg.message.dataSize;
    size_distribution[msg.message.dataSize]++;
    message_count++;
  }

  // 4. Print accurate comparison

  std::cout << "File size: " << file_size << " bytes\n";
  std::cout << "Total messages: " << message_count << "\n";
  std::cout << "Sum of message sizes: " << total_message_size << " bytes\n";

  // Compare compressed and uncompressed file sizes
  if (expect_compression) {
    std::ifstream uncompressed("vehicle_uncompressed.mcap", std::ios::binary | std::ios::ate);
    size_t uncompressed_size = uncompressed.tellg();
    uncompressed.close();

    double compression_ratio = static_cast<double>(file_size) / uncompressed_size * 100.0;

    std::cout << "Uncompressed file size: " << uncompressed_size << " bytes\n";
    std::cout << "Compression ratio: " << compression_ratio << "%\n";
  }
  reader.close();
}

int main() {
  std::cout << "MCAP Compression Demonstration\n";

  // Write and analyze uncompressed version for comparison
  if (WriteFile(false, mcap::CompressionLevel::Default) != 0) return 1;
  AnalyzeFile("vehicle_uncompressed.mcap", false);
  mcap::CompressionLevel levels[] = {
      mcap::CompressionLevel::Fastest,
      mcap::CompressionLevel::Fast,
      mcap::CompressionLevel::Default,
      mcap::CompressionLevel::Slow,
      mcap::CompressionLevel::Slowest
  };
  for (auto level : levels)
  {
    // Write and analyze compressed version
    if (WriteFile(true, level) != 0) return 1;
    AnalyzeFile("vehicle_compressed.mcap", true);
  }

  return 0;
}