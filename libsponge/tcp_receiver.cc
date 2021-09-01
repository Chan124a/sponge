#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.


using namespace std;

void TCPReceiver::segment_received(const TCPSegment& seg) {
    TCPHeader header = seg.header();
    Buffer payload = seg.payload();
    if (!_syn && header.syn) {
        _syn = true;
        _isn = header.seqno;
        _ackno = _isn + 1;
    }

    //如果连接未建立，则退出
    if (!_syn)return;

    //下面是连接建立后进行的操作
    if (_syn && header.fin) {
        _fin = true;
        _fin_seqno = header.seqno + seg.length_in_sequence_space() - 1;
    }
    if (payload.size() > 0) {
        uint64_t index;
        if (header.syn)index = unwrap(header.seqno + 1, _isn, _reassembler.first_unassembled());
        else index = unwrap(header.seqno, _isn, _reassembler.first_unassembled());
        _reassembler.push_substring(payload.copy(), index - 1, _fin);
        uint64_t temp = _reassembler.first_unassembled();
        _ackno = wrap(temp, _isn) + 1;//加1是因为字符串下标和报文下标相差1,即加上syn占用的一个序号

    }
    if (_fin && _ackno == _fin_seqno) {
        _syn = false;
        _fin = false;
        uint64_t temp = _reassembler.first_unassembled();
        _ackno = wrap(temp, _isn) + 2;//加2是因为syn和fin标志都要占用一个序号
        _reassembler.stream_out().end_input();
    }
    return;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    //这里要将从未建立连接、建立连接但没数据、建立连接且有数据三种情况区分开来
    //return _isn? _ackno:std::optional<WrappingInt32>{};
    //上面这种代码是不可行的。如果报文同时携带syn和fin标志，那么处理完报文后，_isn为false，无法输出正确的确认号

    return _ackno == _isn ? std::optional<WrappingInt32>{} : _ackno;
}

size_t TCPReceiver::window_size() const {
    return _capacity - stream_out().buffer_size();
}
