//

#ifndef buffer_hpp
#define buffer_hpp
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace util {
    //======================================================================
    class BufferOffsetError:public std::out_of_range{
    public:
        std::int64_t offset ;
        std::int64_t size ;
        
        BufferOffsetError(std::int64_t offset,std::int64_t size);
    };
    //======================================================================
    class BufferRangeError:public std::out_of_range{
    public:
        std::int64_t offset;
        int length;
        std::int64_t buffersize;
        BufferRangeError(std::int64_t offset, int length, std::int64_t buffersize);

    };
    //=================================================================================
    /** General purpose buffer class.  This allows for reading/writing intergral types from a
     position in the buffer.
     **/
    //======================================================================
    class Buffer {
    protected:
        std::vector<std::uint8_t> data ;
        mutable std::int64_t offset ;
        bool extend ;
        auto checkSize(std::int64_t offset,int length) const ->bool ;
        auto extendSize(std::int64_t offset, int length) -> void ;
        auto checkExtend(std::int64_t offset, int length,bool extend) ->bool ;
    public:
        Buffer() ;
        //=================================================================================
        /// Constructor for buffer
        /// - Parameters:
        ///     - size: the size of the data
        ///     - autoExtend: Does the buffer auto extend on writes if end of buffer is detected
        Buffer(std::int64_t size,bool autoExtend = false) ;
        
        //=================================================================================
        // Size/position related
        //=================================================================================

        //=================================================================================
        /// Determines the size of the buffer (bytes)
        /// - Returns: The size of the buffer
        auto size() const -> std::int64_t ;
        //=================================================================================
        /// Resizes the buffer to the specified size.
        /// If the current position is greater then the new size, the position is set to the end
        /// of the buffer
        /// - Parameters:
        ///     - size: The desired size of the buffer
        auto resize(std::int64_t value) -> void ;
        //=================================================================================
        /// Determines the remaining size of the buffer from the current position
        /// - Returns: The remaining size of the buffer
        auto remaining() const ->std::int64_t ;

        //=================================================================================
        /// Determines the current position in the buffer
        /// - Returns: The current position
        auto at() const -> std::int64_t ;
        //=================================================================================
        /// Sets the current position in the buffer
        /// - Parameters:
        ///     - position: The desired position in the buffer
        /// - Throws: If the position is greater then the size, a runtime error is thrown
        auto setOffset(std::int64_t offset) -> void ;
        //=================================================================================
        /// Determines if the buffer will automatically expand as writes are processed
        /// - Returns: true if the buffer will expand
        auto doesExtend() const ->bool ;
        //=================================================================================
        /// Sets if the buffer will automatically expand as writes are processed
        /// - Parameters:
        ///     - expand: if true, the buffer will expand on writes
        auto setExtend(bool expand) ->void ;

        //=================================================================================
        // General utility
        //=================================================================================

        //=================================================================================
        /// Determines the pointer to the data
        /// - Returns: pointer to data
        auto raw() const -> const std::uint8_t * ;
        //=================================================================================
        /// Determines the pointer to the data
        /// - Returns: pointer to data
        auto raw()  ->  std::uint8_t * ;
        //=================================================================================
        /// Dumps the data into a formatted output
        auto dump(std::ostream &output) ->void ;
        
        //=================================================================================
        // Read/write strings
        //=================================================================================

        //=================================================================================
        /// Reads a string type from the buffer at the specified position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - length: the amount of characters (NOT BYTES) to read.  but will set the current position as if all characters are read
        ///     - offset: optional, defaults to internal offset. The offset in the buffer to read from. If -1, will use the internal offset
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - includeNulls: If false(default), will not include any null characters it reads, and stop reading on that. But will set the current position as if all characters are read
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: The value requested
        /// - Throws: If the read would exceed the buffer, a run time error is thrown.
        template <typename T>
        inline typename std::enable_if<std::is_same_v<T,std::string> ||std::is_same_v<T,std::u16string> || std::is_same_v<T,std::wstring> || std::is_same_v<T,std::u32string>,T>::type read(int length,std::int64_t offset = -1,bool reverse = false ,bool includeNulls=false,std::uint32_t null_value=0) const {
            auto charsize = static_cast<int>(sizeof( typename T::value_type)) ;
            auto bytesize = length * charsize ;
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }
            if (!checkSize(position, bytesize)){
                throw BufferRangeError(position, bytesize, static_cast<std::int64_t>(data.size()));
            }
            T value ;
            typename T::value_type dummy = static_cast<typename T::value_type>(null_value) ;
            typename T::value_type single = 0 ;
            for (int i = 0 ; i < length ; i++){
                std::copy(static_cast<const std::uint8_t *>(data.data())+ position + (i*charsize),static_cast<const std::uint8_t *>(data.data())+ position + ((i+1)*charsize),reinterpret_cast<std::uint8_t*>(&single ));
                // Should we reverse it?
                if (reverse){
                    std::reverse(reinterpret_cast<char*>(&single), reinterpret_cast<char*>(&single)+charsize);
                }
                if (!includeNulls && (single==dummy)){
                    break;
                }
                value.push_back(single);
            }
            if (offset <0) {
                this->offset += bytesize ;
            }
            return value ;
        }
        //=================================================================================
        /// writes a string type to the buffer
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string view to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - offset: optional, defaults to -1. The offset in the buffer, if -1, will use the internal offset
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Throws: If tthe write would exceed the buffer and is not expandable a run time error is thrown.
        template <typename T>
        inline typename std::enable_if<std::is_same_v<T,std::string>|| std::is_same_v<T,std::u16string>|| std::is_same_v<T,std::wstring>|| std::is_same_v<T,std::u32string>,void>::type
        write(const T &value,int length,std::int64_t offset=-1, bool reverse = false,std::uint32_t null_value = 0){
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }

            auto charsize = static_cast<int>(sizeof( typename T::value_type)) ;
            auto bytesize = length * charsize ;
            if (!checkExtend(position, bytesize, this->extend)){
                throw BufferRangeError(position, bytesize, static_cast<std::int64_t>(data.size()));
            }
            auto iter = value.begin() ;
            typename T::value_type write_value ;
            auto count = std::size_t(0) ;
            while ((iter != value.end()) && (length != 0)){
                write_value = *iter ;
                if (reverse){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&write_value),reinterpret_cast<std::uint8_t*>(&write_value)+charsize);
                }
                std::copy(reinterpret_cast<char*>(&write_value),reinterpret_cast<char*>(&write_value)+charsize,reinterpret_cast<char*>(data.data())+position+(count*charsize));
                count++;
                length-- ;
                iter++;
            }
            if (length >0){
                // we have to be here because we ran out of characters
                write_value = static_cast<typename T::value_type>(null_value) ;
                if (reverse){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&write_value),reinterpret_cast<std::uint8_t*>(&write_value)+charsize);
                }
                while (length >0) {
                    std::copy(reinterpret_cast<char*>(&write_value),reinterpret_cast<char*>(&write_value)+charsize,reinterpret_cast<char*>(data.data())+position+(count*charsize));
                    count++;
                    length-- ;
                }
                if (offset <0) {
                    this->offset += bytesize ;
                }
            }
            
        }

        //=================================================================================
        // Read/write integrals
        //=================================================================================

        //=================================================================================
        /// Read an integral
        ///     - offset: optional, defaults to -1. If nonnegative, the offset to read from. Otherwise will use the internal offset
        ///     - reverse: optional, defaults to true. Reverse the order of the bytes
        template<typename T>
        inline typename std::enable_if<std::is_integral_v<T>,T>::type read(std::int64_t offset = -1,bool reverse = true) const {
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }
            auto amount = static_cast<int>(sizeof(T)) ;
            if (!checkSize(position, amount)){
                throw BufferRangeError(position, amount, data.size());
            }
            T rvalue ;
            if (reverse) {
                auto iter = data.rbegin() +  (data.size() - (position + amount) ) ;
                std::copy(iter,iter+amount,reinterpret_cast<std::uint8_t*>(&rvalue));
            }
            else {
                auto iter = data.begin() + position ;
                std::copy(iter,iter+amount,reinterpret_cast<std::uint8_t*>(&rvalue));
            }
            // Reposition the index if we need to
            if (offset <0){
                this->offset += amount ;
            }
            return rvalue ;
        }
        //=================================================================================
        /// Wriate an integral
        ///     - value: the value to be written
        ///     - offset: optional, defaults to -1. If nonnegative, the offset to read from. Otherwise will use the internal offset
        ///     - reverse: optional, defaults to true. Reverse the order of the bytes
        template<typename T>
        inline typename std::enable_if<std::is_integral_v<T>,void>::type write(T value, std::int64_t offset = -1,bool reverse = true)  {
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }
            auto amount = static_cast<int>(sizeof(T)) ;
            if (!checkExtend(position, amount, this->extend)){
                throw BufferRangeError(position, amount, data.size());
            }
            if (reverse){
                auto temp = std::vector<std::uint8_t>(amount,0) ;
                std::copy(reinterpret_cast<const std::uint8_t*>(&value),reinterpret_cast<const std::uint8_t*>(&value)+amount,temp.begin());
                std::copy(temp.rbegin(),temp.rend(),data.data()+position);
            }
            else {
                std::copy(reinterpret_cast<const std::uint8_t*>(&value),reinterpret_cast<const std::uint8_t*>(&value)+amount,data.begin()+position);
            }
            
            // Reposition the index if we need to
            if (offset <0){
                this->offset += amount ;
            }
        }

        //=================================================================================
        // Read/write vector of integrals
        //=================================================================================
        //=================================================================================
        /// Read a vector of integrals
        ///     - length: the amount of objects to read
        ///     - offset: optional, defaults to -1.  The offset in the buffer. If -1, defaults to the internal offset
        ///     - reverse: optional, defaults to true. If true, reverse the byte order of the integral
        ///     - returns: A vector of the integral type
        template<typename T>
        inline typename std::enable_if<std::is_integral_v<T>,std::vector<T>>::type readVector(int length, std::int64_t offset = -1,bool reverse = true) const {
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }
            auto bytesize = static_cast<int>(sizeof(T));
            auto amount = bytesize * length ;
            if (!checkSize(position, amount)){
                throw BufferRangeError(position, amount, static_cast<int>(data.size()));
            }
            std::vector<T> rvalue ;
            T value ;
            for (auto i = 0 ; i < length;i++){
                std::copy(data.data()+position + (i*bytesize), data.data()+position + ((i+1)*bytesize),reinterpret_cast<std::uint8_t*>(&value));
                if (reverse){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&value),reinterpret_cast<std::uint8_t*>(&value)+bytesize) ;
                }
                rvalue.push_back(value) ;
            }
            if (offset <0) {
                this->offset += amount ;
            }
            return rvalue ;
        }
        //=================================================================================
        /// Write a vector of integrals
        ///     - value: the vector of objects to write
        ///     - offset: optional, defaults to -1.  The offset in the buffer. If -1, defaults to the internal offset
        ///     - reverse: optional, defaults to true. If true, reverse the byte order of the integral
        template<typename T>
        inline typename std::enable_if<std::is_integral_v<T>,void>::type writeVector(const std::vector<T> &value, std::int64_t offset = -1,bool reverse = true)  {
            auto position = offset ;
            if (position <0){
                position = this->offset ;
            }
            auto bytesize = static_cast<int>(sizeof(T));
            auto amount = static_cast<int>(value.size()) * bytesize ;
            if (!checkExtend(position, amount, this->extend)){
                throw BufferRangeError(position, amount, static_cast<int>(data.size()));
            }
            if (reverse) {
                for (auto entry:value){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&entry),reinterpret_cast<std::uint8_t*>(&entry)+bytesize);
                    std::copy(reinterpret_cast<std::uint8_t*>(&entry),reinterpret_cast<std::uint8_t*>(&entry)+bytesize,data.data()+position) ;
                    position+=bytesize ;
                }
            }
            else {
                std::copy(reinterpret_cast<const std::uint8_t*>(value.data()),reinterpret_cast<const std::uint8_t*>(value.data()+amount),data.data()+position);
            }
            if (offset <0) {
                this->offset += amount ;
            }

        }
        //=================================================================================
        // Read/write a pointer to data
        //=================================================================================
        /// Write a pointer to data
        ///     - value: the pointer to the data
        ///     - length: the number of bytes to write
        ///     - offset: optional, defaults to -1.  The offset in the buffer. If -1, defaults to the internal offset
        auto write(const std::uint8_t *value,int length,std::int64_t offset = -1) ->void ;
        //=================================================================================
        /// Read to a pointer to data
        ///     - value: the pointer to the data
        ///     - length: the number of bytes to write
        ///     - offset: optional, defaults to -1.  The offset in the buffer. If -1, defaults to the internal offset
        auto read(std::uint8_t *value,int length,std::int64_t offset = -1) const ->void ;

   };
}
#endif /* buffer_hpp */
