#include "simu_matcher.hpp"

#include <cassert>
#include <cstdio>
#include <memory>

std::string repr(Status status) {
    switch (status) {
        case Status::Canceled:
            return "Canceled";
        case Status::Filled:
            return "Filled";
        case Status::PartialFilled:
            return "PartialFilled";
        case Status::Pending:
            return "Pending";
        case Status::Rejected:
            return "Rejected";
        default:
            return "Unknown";
    }
}

int main() {
    std::shared_ptr<BaseSimulator> ptr = std::make_shared<Simulator>();
    Status status;

    int reqId = 0;
    status = ptr->on_order(1, reqId++, 100);
    assert(status == Status::Filled);
    status = ptr->on_order(1, reqId++, 200);
    assert(status == Status::Filled);

    status = ptr->on_order(1, reqId++, 400);
    assert(status == Status::PartialFilled);
    status = ptr->on_order(1, reqId++, 800);
    assert(status == Status::PartialFilled);

    status = ptr->on_order(1, reqId++, 1000);
    assert(status == Status::Rejected);
    status = ptr->on_order(1, reqId++, 1500);
    assert(status == Status::Rejected);

    status = ptr->on_order(1, reqId++, 2000);
    assert(status == Status::Pending);
    status = ptr->on_order(1, reqId++, 10000);
    assert(status == Status::Pending);

    status = ptr->on_order(1, reqId++, 50);
    assert(status == Status::Rejected);
    status = ptr->on_cancel(1, 2);
    assert(status == Status::Canceled);

    int id = 2;
    for (int i = 0; i < 3; ++i) {
        int vol = i * 200 + 100;
        status = ptr->on_order(2, i, vol);
        printf("send id %d: available vol=%lu/%d, status=%s\n", id, ptr->get_pending_volume(id, i), vol, repr(status).c_str());
        status = ptr->on_cancel(2, i);
        printf("cancel id %d: available vol=%lu/%d, status=%s\n", id, ptr->get_pending_volume(id, i), vol, repr(status).c_str());
    }
}