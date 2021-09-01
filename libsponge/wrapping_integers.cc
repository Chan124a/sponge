#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.
using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn + static_cast<uint32_t>(n);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    //不加static_cast<uint32_t>，直接转为uint64_t时，对于负数，默认在高位补1，这并不是我要的结果，我要的是高位补0
    uint64_t ans = static_cast<uint32_t>(n - isn);//这里n-isn为int32_t类型，直接强制转为uint32_t，不用管正负
    //ans和checkpoint在同一个2^32区间里
    ans += (checkpoint & 0xffffffff00000000);
    //ans2是checkpoint附近的另一个可能答案，check2表示ans2和checkpoint的距离
    uint64_t ans2, check1, check2;
    if (ans >= checkpoint) {
        check1 = ans - checkpoint;
        if (ans >= (1ul << 32))ans2 = ans - (1ul << 32);//ans必须大于1ul << 32才能减，否则会溢出。对于溢出的情况，直接令ans2=ans即可
        else ans2 = ans;
        check2 = checkpoint - ans2;
    }
    else
    {
        if (ans < 0xfffffffeffffffff)ans2 = ans + (1ul << 32);
        else ans2 = ans;
        check1 = checkpoint - ans;
        check2 = ans2 - checkpoint;
    }
    if (check2 < check1&&ans2!=ans)ans = ans2;//当ans2=ans时，表示溢出的情况
    return ans;
}
