#define MCAP_IMPLEMENTATION
#include <mcap/writer.hpp>
#include <mcap/reader.hpp>
#include <iostream>


int WriteFile()
{
  std::string filename = "sim.mcap";
  mcap::McapWriterOptions options("");
  mcap::McapWriter writer;

  const auto status = writer.open(filename, options);

  mcap::Schema MySchema("vehicle_sim", "", "");
  writer.addSchema(MySchema);
  mcap::Channel SpeedChannel("speed", "", MySchema.id);
  writer.addChannel(SpeedChannel);
  mcap::Channel PosChannel("pos", "", MySchema.id);
  writer.addChannel(PosChannel);

  mcap::Message message;
  double speed = 5.0;
  const std::byte* speedData = reinterpret_cast<const std::byte*>(&speed);
  message.logTime = 1;
  message.channelId = SpeedChannel.id;
  message.data = speedData;
  message.dataSize = sizeof(speed);
  auto res = writer.write(message);

  struct
  {
    double x;
    double y;
  } posData = {1.0, 2.0};
  const std::byte* posDataPtr = reinterpret_cast<const std::byte*>(&posData);

  message.logTime = 1;
  message.channelId = PosChannel.id;
  message.data = posDataPtr;
  message.dataSize = sizeof(posData);
  res = writer.write(message);


  // write speed message again
  speed = 1000.0;
  const std::byte* data = reinterpret_cast<const std::byte*>(&speed);
  message.logTime = 2;
  message.channelId = SpeedChannel.id;
  message.data = data;
  message.dataSize = sizeof(speed);
  res = writer.write(message);

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
  auto messageView = reader.readMessages(); // 3 is the log time

  for (const auto& msgView : messageView)
  {
    const mcap::Channel& channel = *msgView.channel;
    const mcap::Schema& schema = *msgView.schema;
    std::cout << "Topic on channel: " << channel.topic << std::endl;
    std::cout << "log time: " << msgView.message.logTime << std::endl;
    if (channel.id == 1)
    {
      double speed = *reinterpret_cast<const double*>(msgView.message.data);
      std::cout << "Vehicle speed: " << speed << std::endl;
    }
    else if (channel.id == 2)
    {
      struct
      {
        double x;
        double y;
      } posData;
      posData = *reinterpret_cast<const decltype(posData)*>(msgView.message.data);
      std::cout << "Vehicle pos: " << posData.x << ", " << posData.y << std::endl;

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