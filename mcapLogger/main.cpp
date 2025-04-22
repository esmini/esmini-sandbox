#define MCAP_IMPLEMENTATION
#include <mcap/writer.hpp>
#include <mcap/reader.hpp>
#include <iostream>
#include <chrono>
#include "vehicle_sim_generated.h"

int WriteFile(bool use_compression) {
  // MCAP writer with configurable compression
  mcap::McapWriterOptions options("ros2");
  if (use_compression) {
    options.compression = mcap::Compression::Zstd;
    options.compressionLevel = mcap::CompressionLevel::Fast;
  } else {
    options.compression = mcap::Compression::None;
  }
  options.noChunkCRC = true;
  options.noSummaryCRC = true;
  options.noChunking = false;

  std::string filename = use_compression ? "vehicle_compressed.mcap" : "vehicle_uncompressed.mcap";

  mcap::McapWriter writer;
  auto status = writer.open(filename, options);
  if (!status.ok()) {
    std::cerr << "Failed to open " << filename << ": " << status.message << "\n";
    return 1;
  }

  // Register schema
  mcap::Schema schema("VehicleState", "flatbuffer", "binary");
  writer.addSchema(schema);
  mcap::Channel channel("vehicle_updates", "flatbuffer", schema.id);
  writer.addChannel(channel);

  flatbuffers::FlatBufferBuilder builder;

  // Simulate vehicle updates
  for (int i = 0; i < 100; i++) {
    builder.Clear();

    auto name = builder.CreateString("Vehicle_" + std::to_string(i % 5));
    auto vehicle = VehicleSim::CreateVehicleState(
      builder,
      i % 3,  // 3 vehicle IDs
      std::chrono::nanoseconds(
        std::chrono::system_clock::now().time_since_epoch()).count(),
      10.0f + (i * 0.1f),  // Changing position
      20.0f + (i * 0.1f),
      5.0f + (i % 2 ? 0.1f : -0.1f),  // Oscillating speed
      1.0f,  // Constant acceleration
      name
    );
    builder.Finish(vehicle);

    mcap::Message msg;
    msg.channelId = channel.id;
    msg.sequence = i;
    msg.publishTime = std::chrono::nanoseconds(
      std::chrono::system_clock::now().time_since_epoch()).count();
    msg.data = reinterpret_cast<const std::byte*>(builder.GetBufferPointer());
    msg.dataSize = builder.GetSize();

    auto res = writer.write(msg);
    if (!res.ok()) {
      std::cerr << "Failed to write message: " << res.message << "\n";
      writer.close();
      return 1;
    }
  }

  writer.close();
  std::cout << "Successfully wrote " << filename << " with "
            << (use_compression ? "Zstd compression" : "no compression") << "\n";
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
  std::cout << "\n=== " << filename << " ===\n";
  std::cout << "File size: " << file_size << " bytes\n";
  std::cout << "Total messages: " << message_count << "\n";
  std::cout << "Sum of message sizes: " << total_message_size << " bytes\n";

  if (expect_compression) {
    double ratio = (double)total_message_size / file_size;
    std::cout << "Compression ratio: " << ratio << ":1\n";
    std::cout << "Space savings: " << (1 - (file_size/(double)total_message_size))*100 << "%\n";
  }

  reader.close();
}

int main() {
  std::cout << "MCAP Compression Demonstration\n";

  // Write and analyze compressed version
  if (WriteFile(true) != 0) return 1;
  AnalyzeFile("vehicle_compressed.mcap", true);

  // Write and analyze uncompressed version for comparison
  if (WriteFile(false) != 0) return 1;
  AnalyzeFile("vehicle_uncompressed.mcap", false);

  return 0;
}