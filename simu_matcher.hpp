#pragma once

#include <cstdint>
#include <unordered_map>

enum class Status {
    Filled = 0,
    PartialFilled,
    Rejected,
    Pending,
    Canceled
};

inline uint64_t generate_unique_id(uint32_t x, uint32_t y) {
    return (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(y);
}

struct BaseSimulator {
    virtual ~BaseSimulator() = default;
    virtual Status on_order(uint32_t session_id, uint32_t order_ref, uint64_t volume) = 0;
    virtual Status on_cancel(uint32_t session_id, uint32_t order_ref) = 0;
    virtual uint64_t get_pending_volume(uint32_t session_id, uint32_t order_ref) const = 0;
};

struct Simulator : public BaseSimulator {
    std::unordered_map<uint32_t, uint64_t> orders;
    Status on_order(uint32_t session_id, uint32_t order_ref, uint64_t volume) override {
        auto id = generate_unique_id(session_id, order_ref);
        if (100 <= volume && volume < 400) {
            return Status::Filled;
        } else if (400 <= volume && volume < 1000) {
            orders[id] = volume / 4;
            return Status::PartialFilled;
        } else if (1000 <= volume && volume < 2000) {
            return Status::Rejected;
        } else if (2000 <= volume) {
            orders[id] = volume;
            return Status::Pending;
        } else {
            return Status::Rejected;
        }
    }
    Status on_cancel(uint32_t session_id, uint32_t order_ref) override {
        auto id = generate_unique_id(session_id, order_ref);
        return orders.erase(id) ? Status::Canceled : Status::Rejected;
    }

    uint64_t get_pending_volume(uint32_t session_id, uint32_t order_ref) const override {
        auto id = generate_unique_id(session_id, order_ref);
        auto it = orders.find(id);
        return it != orders.end() ? it->second : 0;
        ;
    }
};