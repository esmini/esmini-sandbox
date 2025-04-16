#define MCAP_IMPLEMENTATION
#include <mcap/writer.hpp>
#include <mcap/reader.hpp>
#include <iostream>
#include <chrono>
#include "vehicle_sim_generated.h"

int WriteFile()
{
  std::string filename = "sim.mcap";
  mcap::McapWriterOptions options("");
  mcap::McapWriter writer;

  const auto status = writer.open(filename, options);
  if (!status.ok())
  {
    std::cerr << "Failed to open " << filename << " for writing: " << status.message
              << std::endl;
    return 1;
  }

  mcap::Schema MySchema("vehicle_sim", "", "");
  writer.addSchema(MySchema);
  mcap::Channel SpeedChannel("speed", "", MySchema.id);
  writer.addChannel(SpeedChannel);
  mcap::Channel PosChannel("pos", "", MySchema.id);
  writer.addChannel(PosChannel);

  flatbuffers::FlatBufferBuilder builder;
  VehicleSim::VehicleStateSpeedBuilder speedBuilder(builder);
  speedBuilder.add_speed(10.0f);
  auto speedOffset = speedBuilder.Finish();
  builder.Finish(speedOffset);
  auto data = builder.GetBufferPointer();
  auto size = builder.GetSize();
  
  mcap::Message message;
  message.logTime = 1;
  message.channelId = SpeedChannel.id;
  message.data = reinterpret_cast<const std::byte*>(data);
  message.dataSize = size;
  auto res = writer.write(message);
  if (!res.ok())
  {
    std::cerr << "failed to write message: " << res.message << std::endl;
    writer.close();
    return 1;
  }

  VehicleSim::VehicleStatePosBuilder posBuilder(builder);
  posBuilder.add_x(1.0f);
  posBuilder.add_y(2.0f);
  auto posOffset = posBuilder.Finish();
  builder.Finish(posOffset);
  data = builder.GetBufferPointer();
  size = builder.GetSize();
  message.logTime = 1;
  message.channelId = PosChannel.id;
  message.data = reinterpret_cast<const std::byte*>(data);
  message.dataSize = size;
  res = writer.write(message);
  if (!res.ok())
  {
    std::cerr << "failed to write message: " << res.message << std::endl;
    writer.close();
    return 1;
  }

  // write speed message again
  VehicleSim::VehicleStateSpeedBuilder speedBuilder2(builder);
  speedBuilder2.add_speed(20.0f);
  speedOffset = speedBuilder2.Finish();
  builder.Finish(speedOffset);
  data = builder.GetBufferPointer();
  size = builder.GetSize();
  message.logTime = 2;
  message.channelId = SpeedChannel.id;
  message.data = reinterpret_cast<const std::byte*>(data);
  message.dataSize = size;
  res = writer.write(message);
  if (!res.ok())
  {
    std::cerr << "failed to write message: " << res.message << std::endl;
    writer.close();
    return 1;
  }

  // Close the writer
  writer.close();
  return 0;

}
int ReadFile()
{
  std::string filename = "sim.mcap";
  mcap::McapReader reader;
  auto status = reader.open(filename);
  if (!status.ok())
  {
    std::cerr << "Failed to open " << filename << " for reading: " << status.message
              << std::endl;
    return 1;
  }
  auto messageView = reader.readMessages(1, 2); // 3 is the log time

  for (const auto& msgView : messageView)
  {
    const mcap::Channel& channel = *msgView.channel;
    const mcap::Schema& schema = *msgView.schema;
    std::cout << "Topic on channel: " << channel.topic << std::endl;
    std::cout << "log time: " << msgView.message.logTime << std::endl;
    if (channel.id == 1)
    {
      const VehicleSim::VehicleStateSpeed* speed = flatbuffers::GetRoot<VehicleSim::VehicleStateSpeed>(msgView.message.data);
      std::cout << "Vehicle speed: " << speed->speed() << std::endl;
    }
    else if (channel.id == 2)
    {
      const VehicleSim::VehicleStatePos* pos = flatbuffers::GetRoot<VehicleSim::VehicleStatePos>(msgView.message.data);
      std::cout << "Vehicle pos: " << pos->x() << ", " << pos->y() << std::endl;
    }
  }
  return 0;
}

int main()
{
  WriteFile();
  ReadFile();
  return 0;
}