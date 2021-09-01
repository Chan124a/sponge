#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

using namespace std;

ByteStream::ByteStream(const size_t capacity)  :_capacity(capacity){ }

size_t ByteStream::write(const string &data) {
    size_t size=data.size();
    if(size<=_capacity-_buff.size()){
        for(auto c:data){
            _buff.push_back(c);
        }
        _bytes_written+=size;
        return size;
    }else{
        int write_size=_capacity-_buff.size();
        for (int i = 0; i < write_size; i++)
        {
            _buff.push_back(data[i]);
        }
        _bytes_written+=write_size;
        return write_size;
    }
    
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    int peek_len=len;
    if(len>_buff.size()){
        peek_len=_buff.size();
    }
    return string().assign(_buff.begin(),_buff.begin()+peek_len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    int pop_size=len;
    if(len>_buff.size()){
        pop_size=_buff.size();
    }
    _buff.erase(_buff.begin(),_buff.begin()+pop_size);
    _bytes_read+=pop_size;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const string read_string=peek_output(len);
    pop_output(len);
    return read_string;
}

void ByteStream::end_input() {_end_input=true;}

bool ByteStream::input_ended() const { return _end_input; }

size_t ByteStream::buffer_size() const { return _buff.size(); }

bool ByteStream::buffer_empty() const { return _buff.empty(); }

bool ByteStream::eof() const { return _end_input&&buffer_empty(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity-_buff.size(); }
