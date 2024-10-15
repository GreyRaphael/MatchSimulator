# MatchSimulator

websocket based Match Engine Simulator

```cpp
enum Market: uint8{
    Unknown = 0,
    SSE,
    SZSE
}

enum Side: uint8{
    Buy = 0,
    Sell
}

enum PriceType: uint8{
    Limit = 0,
    Market
}

enum Status: uint8{
    Unknown = 0,
    Accepted,
    Rejected,
    PartialFilled,
    Filled
}
```