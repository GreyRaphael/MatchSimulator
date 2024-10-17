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

struct BaseSimulator {
    virtual ~BaseSimulator() = default;
    virtual Status on_order(uint32_t id, uint64_t volume) = 0;
    virtual Status on_cancel(uint32_t id) = 0;
    virtual uint64_t get_pending_volume(uint32_t id) const = 0;
};

struct Simulator : public BaseSimulator {
    std::unordered_map<uint32_t, uint64_t> orders;
    Status on_order(uint32_t id, uint64_t volume) override {
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
    Status on_cancel(uint32_t id) override {
        return orders.erase(id) ? Status::Canceled : Status::Rejected;
    }

    uint64_t get_pending_volume(uint32_t id) const override {
        auto it = orders.find(id);
        return it != orders.end() ? it->second : 0;
        ;
    }
};