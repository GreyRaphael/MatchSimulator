#include <flatbuffers/flatbuffers.h>
#include <hv/WebSocketClient.h>

#include "message_generated.h"

int main() {
    hv::WebSocketClient client;
    client.setPingInterval(0);
    client.onopen = [] {
        std::cout << "onopen\n";
    };
    client.onclose = [] {
        std::cout << "onclose\n";
    };
    client.onmessage = [](std::string const& msg) {
        auto msg_ptr = Messages::GetMessage(msg.data());
        std::cout << "get message back\n";
        switch (msg_ptr->type()) {
            case Messages::MessageType::OrderField: {
                auto order_ptr = msg_ptr->payload_as_OrderField();
                std::cout << std::format("symbol={}, vol={}, trade_vol={}", order_ptr->symbol()->c_str(), order_ptr->volume(), order_ptr->trade_volume());
            }
            default:
                break;
        }
    };

    client.open("localhost:8888");

    flatbuffers::FlatBufferBuilder builder;
    auto symbol = builder.CreateString("600000");
    auto order = Messages::CreateReqOrderInsertField(
        builder,
        0,
        symbol,
        0,
        1,
        12.3,
        10000);
    auto msg = Messages::CreateMessage(
        builder,
        Messages::MessageType::ReqOrderInsertField,
        Messages::Payload::ReqOrderInsertField,
        order.Union());
    builder.Finish(msg);
    client.send(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());

    getchar();
}