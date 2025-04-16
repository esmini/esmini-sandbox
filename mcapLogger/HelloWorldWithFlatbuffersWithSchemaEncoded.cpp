#define MCAP_IMPLEMENTATION
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>


#include <mcap/writer.hpp>
#include <mcap/reader.hpp>

#include "flatbuffers/flatbuffers.h"
#include "vehicle_state_generated.h" // Generated header
#include "flatbuffers/reflection.h" // Include FlatBuffers reflection header

using namespace mcap;
using namespace std::chrono;

// Function to read the binary FlatBuffer schema (.bfbs) file
std::vector<uint8_t> readBinaryFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return {};
    }
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Error reading file: " << filePath << std::endl;
        return {};
    }
    file.close();
    return buffer;
}

int main() {
    const std::string schemaFilePath = "mcapLogger/vehicle_state.bfbs"; // Path to your generated .bfbs file

    // --- Writing to MCAP file ---
    std::string filename = "vehicle_data.mcap";
    mcap::McapWriter writer;
    mcap::McapWriterOptions options("");
    const auto status = writer.open(filename, options);


    // Read the binary FlatBuffer schema
    std::vector<uint8_t> schemaData = readBinaryFile(schemaFilePath);
    if (schemaData.empty()) {
        std::cerr << "Error reading FlatBuffer schema file." << std::endl;
        return 1;
    }

    // Create and register the MCAP schema
    Schema flatbufferSchema(
        "VehicleState", // Schema name
        "application/x-flatbuffers", // Schema encoding type
        std::string(schemaData.begin(), schemaData.end()) // Schema data as a string
    );
    writer.addSchema(flatbufferSchema);


    // Create and register the MCAP channel
    Channel vehicleChannel("vehicle_state", "flatbuffer", flatbufferSchema.id);
    writer.addChannel(vehicleChannel);

    // Write some messages
    flatbuffers::FlatBufferBuilder builder;

    for (int i = 0; i < 5; ++i) {
        VehicleSim::VehicleStateBuilder stateBuilder(builder);
        stateBuilder.add_speed(static_cast<float>(i * 5.0));
        if(i != 1) // skip the pos for time 1
        {
            stateBuilder.add_x(static_cast<float>(i * 2.0));
            stateBuilder.add_y(static_cast<float>(i * 2.0 + 1.0));
        }
        auto state = stateBuilder.Finish();
        builder.Finish(state);
        mcap::Message message;
        message.logTime = i;
        message.channelId = vehicleChannel.id;
        message.data = reinterpret_cast<const std::byte*>(builder.GetBufferPointer());
        message.dataSize = builder.GetSize();
        Status res = writer.write(message);
        builder.Clear();
    }

    std::cout << "Successfully wrote to vehicle_data.mcap" << std::endl;
    writer.close();

    // --- Reading from MCAP file ---
    std::ifstream infile("vehicle_data.mcap", std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error opening vehicle_data.mcap for reading" << std::endl;
        return 1;
    }

    mcap::McapReader reader;
    Status r_status = reader.open(infile);
    if (!r_status.ok()) {
        std::cerr << "Error opening reader: " << r_status.message << std::endl;
        return 1;
    }

    std::cout << "\n--- Reading messages from vehicle_data.mcap ---" << std::endl;

    auto messageView = reader.readMessages();

    for (const auto& msgView : messageView)
    {
        const mcap::Channel& channel = *msgView.channel;
        const mcap::Schema& schema = *msgView.schema;
        // this code uses reflection to decode the schema encoded in schema data
        const reflection::Schema* fbSchema = reflection::GetSchema(reinterpret_cast<const uint8_t*>(schema.data.data()));

        if (fbSchema) {
            const reflection::Object* rootTable = fbSchema->root_table();
            if (rootTable) {
                const uint8_t* messageData = reinterpret_cast<const uint8_t*>(msgView.message.data);
                const flatbuffers::Table* root = flatbuffers::GetRoot<flatbuffers::Table>(messageData); // Retrieve the root table

                for (size_t i = 0; i < rootTable->fields()->size(); ++i) {
                    const reflection::Field* field = rootTable->fields()->Get(i);
                    if (field && field->name()) {
                        if (field->name()) {
                            std::cout << "  Field: " << field->name()->str() << " (Type: " << reflection::EnumNamesBaseType()[field->type()->base_type()] << ")";
                        } else {
                            std::cerr << "  Field name is null." << std::endl;
                        }
                        if (root->CheckField(field->offset())) {
                            std::cout << ", Value: " << root->GetField<float>(field->offset(), 0.0f);
                        } else {
                            std::cout << ", Value: [Field not present]";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "Error loading FlatBuffer schema for reflection." << std::endl;
        }
    }

    for (const auto& msgView : messageView)
    {
        const mcap::Channel& channel = *msgView.channel;
        const mcap::Schema& schema = *msgView.schema;
        // below code shall use generated flatbuffers header code to decode the message(did not used schema encoded in schema data)
        std::cout << "using generated flatbuffer code to decode:" << std::endl;
        std::cout << "Topic on channel: " << channel.topic << std::endl;
        std::cout << "log time: " << msgView.message.logTime << std::endl;
        if (schema.name == "VehicleState" && channel.topic == "vehicle_state")
        {
          const VehicleSim::VehicleState* state =
          flatbuffers::GetRoot<VehicleSim::VehicleState>(
              reinterpret_cast<const uint8_t*>(msgView.message.data)
          );
          std::cout << ", Speed: " << state->speed()
                << ", X: " << state->x()
                << ", Y: " << state->y() << std::endl;
        }
    }
    infile.close();

    return 0;
}