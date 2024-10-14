#include <flatbuffers/flatbuffers.h>
#include <hv/WebSocketServer.h>

#include <cstdio>
#include <format>

#include "message_generated.h"

int main(int argc, char** argv) {
    hv::WebSocketService ws;
    ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
        std::cout << std::format("client connect to {}\n", req->Path().c_str());
    };
    ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
        auto msg_ptr = Messages::GetMessage(msg.data());
        flatbuffers::FlatBufferBuilder builder;
        switch (msg_ptr->type()) {
            case Messages::MessageType::ReqOrderInsertField: {
                auto req_order = msg_ptr->payload_as_ReqOrderInsertField();
                auto matched_order = Messages::CreateOrderFieldDirect(
                    builder,
                    100,
                    100,
                    req_order->market(),
                    req_order->symbol()->c_str(),
                    req_order->side(),
                    req_order->price_type(),
                    req_order->price(),
                    req_order->volume(),
                    req_order->volume() * 0.5,
                    1);
                auto msg = Messages::CreateMessage(
                    builder,
                    Messages::MessageType::OrderField,
                    Messages::Payload::OrderField,
                    matched_order.Union());
                builder.Finish(msg);
                channel->send(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
                builder.Clear();
                break;
            }
            case Messages::MessageType::ReqOrderActionField: {
                auto cancel_order = msg_ptr->payload_as_ReqOrderActionField();
                auto canceled_order = Messages::CreateRspOrderActionField(
                    builder,
                    cancel_order->session_id(),
                    cancel_order->order_ref());
                auto msg = Messages::CreateMessage(
                    builder,
                    Messages::MessageType::RspOrderActionField,
                    Messages::Payload::RspOrderActionField,
                    canceled_order.Union());
                builder.Finish(msg);
                channel->send(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
                builder.Clear();
                break;
            }
            default:
                std::cout << "not match" << '\n';
                break;
        }
    };
    ws.onclose = [](const WebSocketChannelPtr& channel) {
        printf("onclose\n");
    };

    hv::WebSocketServer server{&ws};
    server.setHost("localhost");
    server.setPort(8888);
    std::cout << std::format("Listening on {}:{}...\n", server.host, server.port);
    server.start();
    getchar();
}
