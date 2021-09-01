#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity),_first_unacceptable(capacity) {}
bool StreamReassembler::_is_coincide(block A,block B)const{
      if(B.index+B.lengh<A.index||B.index>A.index+A.lengh)return false;
      else return true;
    }
void StreamReassembler::_push(){
    auto A=*(_block.begin());
    if(A.index==_first_unassembled){
        size_t write_size=_output.write(A.data);
        _unassembled_bytes-=write_size;
        //_first_unassembled+=write_size;这个不需要，因为每次执行push_substring都会重新计算
        _block.erase(_block.begin());

        
    }
    if(_eof&&empty())_output.end_input();
    return ;
}
void StreamReassembler::_check(std::set<StreamReassembler::block>::iterator it,block &temp){
    while (it!=_block.end()&&_is_coincide(*(it),temp))
    {
        block current=*(it);
        if(temp.index+temp.lengh<=current.index+current.lengh){
            size_t temp_lengh=current.index+current.lengh-temp.index-temp.lengh;
            temp.data+=current.data.substr(current.lengh-temp_lengh,temp_lengh);
            temp.lengh=temp.data.size();
        }
        _unassembled_bytes-=current.lengh;
        it=_block.erase(it);
    }
    return ;
}
//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    //更新参数
        _first_unread = _output.bytes_read();
        _first_unassembled = _output.bytes_written();
        _first_unacceptable = _first_unread + _capacity;

    if(eof){
        _eof=true;
    }
    size_t last_data_index=index+data.size();

    // // 空串
    // if(last_data_index==index){
    //     if(_eof&&empty())_output.end_input();
    //     return ;
    // }

    // 下标超出允许范围
    if(index>=_first_unacceptable||last_data_index<_first_unassembled){
        // if(_eof&&empty())_output.end_input();
        return ;
    }
    // 将超出范围的字符截掉
    size_t data_begin=index,data_size=data.size();
    if(data_begin<_first_unassembled){
        data_size-=(_first_unassembled-data_begin);
        data_begin=_first_unassembled;
    }
    if(data_begin+data_size>_first_unacceptable){
        _eof=false;//最后的结束字符被截去了,所以要重新把 _eof 重新置false
        data_size-=(data_begin+data_size-_first_unacceptable);
    }
        

    // 将截取的字符存入块temp_data
    block temp_data;
    temp_data.index=data_begin;
    temp_data.lengh=data_size;
    temp_data.data=data.substr(data_begin-index,data_size);

    if(_block.empty()){
        _block.insert(temp_data);
        _unassembled_bytes+=temp_data.lengh;
        _push();
        return ;
    }
    auto insert_location=_block.lower_bound(temp_data);
    if(insert_location==_block.begin()){
        _check(insert_location,temp_data);
    }else {
        auto previous=(--insert_location);
        if(_is_coincide(*previous,temp_data)){
            if((*previous).index+(*previous).lengh>temp_data.index+temp_data.lengh){
                temp_data=(*previous);
            }else{
                size_t temp_lengh=temp_data.index+temp_data.lengh-(*previous).index-(*previous).lengh;
                temp_data.data=(*previous).data+ temp_data.data.substr(temp_data.lengh-temp_lengh,temp_lengh);
                temp_data.lengh=temp_data.data.size();
                temp_data.index=(*previous).index;
            }
            _unassembled_bytes-=((*previous).lengh);
            // 这里的insert_location由于已经减一，所以当前指向的是插入位置的前一位
            // 执行erase操作后，就指向原来的值，即一开始的插入位置
            insert_location=_block.erase(insert_location);
        }else ++insert_location;
        _check(insert_location,temp_data);
    }
    // 由于无法对_block已有的元素进行修改，
    // 所以这里采用的是将重叠的块都合并到temp_data中,再插入到_block中
    // 而不是先将temp_data插入,再去执行块合并
    _block.insert(temp_data);
    _unassembled_bytes+=temp_data.lengh;
    _push();

    return ;
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _unassembled_bytes==0; }
