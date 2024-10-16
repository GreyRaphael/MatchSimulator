#include <flatbuffers/flatbuffers.h>
#include <hv/WebSocketClient.h>

#include <cstdint>
#include <vector>

#include "message_generated.h"

std::vector<uint8_t> serialize_order(flatbuffers::FlatBufferBuilder& builder,
                                     uint8_t mkt,
                                     const char* symbol,
                                     uint8_t side,
                                     double price,
                                     double vol) {
    builder.Clear();
    auto symbol_offset = builder.CreateString(symbol);  // must above order_builder to avoid nested state
    Messages::ReqOrderInsertFieldBuilder order_builder(builder);
    order_builder.add_market(mkt);
    order_builder.add_symbol(symbol_offset);
    order_builder.add_side(side);
    order_builder.add_price(price);
    order_builder.add_volume(vol);
    // can use default price_type without pass argument.
    auto order = order_builder.Finish();
    auto msg = Messages::CreateMessage(builder, Messages::Payload::ReqOrderInsertField, order.Union());
    builder.Finish(msg);
    auto sp = builder.GetBufferSpan();
    return {sp.begin(), sp.end()};
}

std::vector<uint8_t> serialize_cancel(flatbuffers::FlatBufferBuilder& builder,
                                      uint32_t session_id,
                                      uint32_t order_ref) {
    builder.Clear();
    auto cancel = Messages::CreateReqOrderActionField(builder, session_id, order_ref);
    auto msg = Messages::CreateMessage(builder, Messages::Payload::ReqOrderActionField, cancel.Union());
    builder.Finish(msg);
    auto sp = builder.GetBufferSpan();
    return {sp.begin(), sp.end()};
}

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
        switch (msg_ptr->payload_type()) {
            case Messages::Payload::OrderField: {
                auto order_ptr = msg_ptr->payload_as_OrderField();
                std::cout << std::format("symbol={}, vol={}, trade_vol={}\n", order_ptr->symbol()->c_str(), order_ptr->volume(), order_ptr->trade_volume());
                break;
            }
            case Messages::Payload::RspOrderActionField: {
                auto cancel_ptr = msg_ptr->payload_as_RspOrderActionField();
                std::cout << std::format("cancel session_id={}, order_ref={}\n", cancel_ptr->orig_session_id(), cancel_ptr->orig_order_ref());
                break;
            }
            default:
                break;
        }
    };

    client.open("localhost:8888");

    flatbuffers::FlatBufferBuilder builder;
    for (size_t i = 0; i < 5; ++i) {
        auto order_data = serialize_order(builder, 1, "600000", 1, 10.1 * i, 100 * i + 100);
        client.send(reinterpret_cast<const char*>(order_data.data()), order_data.size());
    }
    for (size_t i = 0; i < 3; ++i) {
        auto cancel_data = serialize_cancel(builder, 100, i);
        client.send(reinterpret_cast<const char*>(cancel_data.data()), cancel_data.size());
    }

    getchar();
}