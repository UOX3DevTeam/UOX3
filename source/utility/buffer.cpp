//

#include "buffer.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "strutil.hpp"

using namespace std::string_literals ;
namespace util {
    
    //======================================================================
    BufferOffsetError::BufferOffsetError(std::int64_t offset,std::int64_t size):std::out_of_range(util::format("Offset %i would exceed size %i",offset,size)){
        this->offset = offset ;
        this->size = size ;
    }
    //======================================================================
    BufferRangeError::BufferRangeError(std::int64_t offset, int length, std::int64_t buffersize):std::out_of_range(util::format("Offset: %i Size: %i would exceed buffer size %i",offset,length,buffersize)){
        this->offset = offset;
        this->length = length ;
        this->buffersize = buffersize ;
    }

    
    //======================================================================
    auto Buffer::checkSize(std::int64_t offset, int length) const ->bool {
        if (offset + length <= data.size()){
            return true ;
        }
        return false ;
    }

    //======================================================================
    auto Buffer::extendSize(std::int64_t offset, int length) ->void {
        auto amount = static_cast<std::int64_t>(data.size()) - (offset+static_cast<std::int64_t>(length)) ;
        if (amount < 0){
            data.resize(data.size() + std::abs(amount),0);
        }
    }
    //======================================================================
    auto Buffer::checkExtend(std::int64_t offset, int length,bool extend) ->bool {
        auto rvalue = checkSize(offset, length);
        if (extend && !rvalue){
            extendSize(offset, length);
            rvalue = true ;
        }
        return rvalue ;
    }

    //======================================================================
    Buffer::Buffer():offset(0),extend(false) {
    }
    //======================================================================
    Buffer::Buffer(std::int64_t size,bool autoExtend):Buffer() {
        data = std::vector<std::uint8_t>(size,0) ;
        extend = autoExtend ;
    }
    //======================================================================
    auto Buffer::size() const -> std::int64_t{
        return static_cast<std::int64_t>(data.size()) ;
    }
    //======================================================================
    auto Buffer::resize(std::int64_t value)->void {
        data.resize(value,0) ;
        if (offset < data.size()){
            offset = std::max(static_cast<std::int64_t>(0),static_cast<std::int64_t>(data.size())-1);
        }
    }
    //======================================================================
    auto Buffer::remaining() const ->std::int64_t {
        return static_cast<std::int64_t>(data.size() - offset) ;
    }

    //======================================================================
    auto Buffer::at() const ->std::int64_t {
        return offset ;
    }
    //======================================================================
    auto Buffer::setOffset(std::int64_t offset) ->void {
        if (offset >= data.size() && !extend){
            throw BufferOffsetError(offset, data.size());
        }
        if (offset == std::numeric_limits<std::int64_t>::max()){
            throw BufferOffsetError(offset, data.size());
        }
        if (offset>= data.size()){
            // we need to extend this
            data.resize(offset+1, 0);
        }
        this->offset = offset ;
    }
    //======================================================================
    auto Buffer::doesExtend() const ->bool {
        return extend ;
    }
    //======================================================================
    auto Buffer::setExtend(bool state) ->void {
        extend = state ;
    }
    //======================================================================
    auto Buffer::raw() const -> const std::uint8_t * {
        return data.data() ;
    }
    //======================================================================
    auto Buffer::raw()  ->  std::uint8_t * {
        return data.data();
    }
    //======================================================================
    auto Buffer::dump(std::ostream &output) ->void {
        ::util::dump(output, data.data(), data.size()) ;
    }
    //======================================================================
    auto Buffer::write(const std::uint8_t *value,int length,std::int64_t offset ) ->void {
        auto position = offset ;
        if (position <0){
            position = this->offset ;
        }
        auto amount = length ;
        if (!checkExtend(position, amount,this->extend)){
            throw BufferRangeError(position, amount, static_cast<int>(data.size()));
        }
        std::copy(value,value+amount,data.data()+position);
        if (offset < 0) {
            this->offset += amount ;
        }

    }
    //=================================================================================
    //======================================================================
    auto Buffer::read(std::uint8_t *value,int length,std::int64_t offset ) const ->void {
        auto position = offset ;
        if (position <0){
            position = this->offset ;
        }
        auto amount = length ;
        if (!checkSize(position, amount)){
            throw BufferRangeError(position, amount, static_cast<int>(data.size()));
        }
        std::copy(data.data()+position,data.data()+position+amount,value);
        if (offset < 0) {
            this->offset += amount ;
        }

    }

 }
