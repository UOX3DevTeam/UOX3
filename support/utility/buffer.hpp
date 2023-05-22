//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef buffer_hpp
#define buffer_hpp

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstddef>

//=========================================================================================
namespace util {
    //=================================================================================
    /** General purpose buffer class.  This allows for reading/writing intergral types from a
     position in the buffer.
     **/
    class buffer_t{
        const void *read_data ; // Ptr to the data we read
        void *write_data ; // Ptr to the data we write
        mutable std::size_t current_position ;
        std::size_t length ; // Length of our buffer ;
        bool owning ; // Do we own this data (if so, we can resize it)
        std::vector<std::uint8_t> data ; // If we own the data, we store it here
        bool is_expandable ; // Is this data expandable ?
        
    public:
        //=================================================================================
        // Constructors
        //=================================================================================
        
        //=================================================================================
        /// Base constructor for buffer
        buffer_t() ;
        
        //=================================================================================
        /// Constructor for buffer
        /// - Parameters:
        ///     - ptr: The pointer to the data
        ///     - size: the size of the data
        ///     - consume: (optional, defaults to false). If true, the buffer takes ownership
        ///     - Throws: Runtime error if initialized with a nullptr or 0 size
        buffer_t(void *ptr,std::size_t size, bool consume=false) ;
        
        //=================================================================================
        /// Constructor for buffer
        /// - Parameters:
        ///     - ptr: The pointer to the data
        ///     - size: the size of the data
        ///     - consume: (optional, defaults to false). If true, the buffer takes ownership
        ///     - Throws: Runtime error if initialized with a nullptr or 0 size
        buffer_t(const void *ptr,std::size_t size, bool consume=false) ;
        
        //=================================================================================
        /// Constructor for buffer
        /// - Parameters:
        ///     - size: the size of the data
        buffer_t(std::size_t size) ;
        
        
        //=================================================================================
        // Size/position related
        //=================================================================================

        //=================================================================================
        /// Determines the size of the buffer (bytes)
        /// - Returns: The size of the buffer
        auto size() const ->std::size_t ;

        //=================================================================================
        /// Determines the remaining size of the buffer from the current position
        /// - Returns: The remaining size of the buffer
        auto remaining() const ->std::size_t ;

        //=================================================================================
        /// Determines the current position in the buffer
        /// - Returns: The current position
        auto at() const ->std::size_t;
        
        
        //=================================================================================
        /// Sets the current position in the buffer
        /// - Parameters:
        ///     - position: The desired position in the buffer
        /// - Returns: Reference to the buffer
        /// - Throws: If the position is greater then the size, a runtime error is thrown
        [[maybe_unused]] auto at(std::size_t position)  -> buffer_t&;

        //=================================================================================
        /// Resizes the buffer to the specified size.
        /// If the current position is greater then the new size, the position is set to the end
        /// of the buffer
        /// - Parameters:
        ///     - size: The desired size of the buffer
        /// - Returns: Reference to the buffer
        /// - Throws: If the buffer is not owned, throws a run time error
        [[maybe_unused]] auto resize(std::size_t size) ->buffer_t& ;
        
        //=================================================================================
        /// Sets if the buffer will automatically expand as writes are processed
        /// - Parameters:
        ///     - expand: if true, the buffer will expand on writes
        /// - Returns: Reference to the buffer
        [[maybe_unused]] auto expandable(bool expand) ->buffer_t ;

        //=================================================================================
        /// Determines if the buffer will automatically expand as writes are processed
        /// - Returns: true if the buffer will expand
        auto expandable() const ->bool ;

        //=================================================================================
        // Access related
        //=================================================================================

        //=================================================================================
        /// Determines the pointer to the data
        /// - Returns: pointer to data
        auto raw() ->void* ;
 
        //=================================================================================
        /// Determines the pointer to the data
        /// - Returns: pointer to data
        auto raw() const ->const void* ;

        //=================================================================================
        // read
        //=================================================================================

        //=================================================================================
        /// Reads an integral type from the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        /// - Returns: The value requested
        /// - Throws: If the buffer is null, or the read would exceed the buffer, a run time error is thrown.
        template <typename T>
        inline typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T,bool>,T>::type
        read(bool reverse=false){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto bytesize = sizeof(T) ;
            if (current_position+bytesize >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            T value ;
            std::copy(static_cast<const std::uint8_t*>(read_data)+current_position,static_cast<const std::uint8_t*>(read_data)+current_position+bytesize,reinterpret_cast<std::uint8_t*>(&value));
            current_position+= bytesize;
            if (reverse) {
                std::reverse(reinterpret_cast<std::uint8_t*>(&value), reinterpret_cast<std::uint8_t*>(&value)+bytesize);
            }
            return value ;
        }

        //=================================================================================
        /// Reads a string type from the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - size: the amount of characters (NOT BYTES) to read.  but will set the current position as if all characters are read
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - stop-on-null: If true(default), will not include any null characters it reads, and stop reading on that. But will set the current position as if all characters are read
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: The value requested
        /// - Throws: If the buffer is null, or the read would exceed the buffer, a run time error is thrown.
        template <typename T>
        inline typename std::enable_if<std::is_same_v<T,std::string> ||std::is_same_v<T,std::u16string> || std::is_same_v<T,std::wstring> || std::is_same_v<T,std::u32string>,T>::type
        read(std::size_t amount, bool reverse = false,bool stop_on_null = true,std::uint32_t null_value=0){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            auto charsize = sizeof( typename T::value_type) ;
            auto bytesize = amount * charsize ;
            if (current_position+bytesize >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            T value ;
            typename T::value_type dummy = static_cast<typename T::value_type>(null_value) ;
            typename T::value_type single = 0 ;
          
            for (std::size_t i = 0 ; i < amount ; i++){
                std::copy(static_cast<const std::uint8_t *>(read_data)+ current_position + (i*charsize),static_cast<const std::uint8_t *>(read_data)+ current_position + ((i+1)*charsize),reinterpret_cast<std::uint8_t*>(&single ));
                // Should we reverse it?
                if (reverse){
                    std::reverse(reinterpret_cast<char*>(&single), reinterpret_cast<char*>(&single)+charsize);
                }
                if (stop_on_null && (single==dummy)){
                    break;
                }
                value.push_back(single);
            }
            current_position+=bytesize;
            return value ;
        }
 
        //=================================================================================
        /// Reads a fixed amount from the buffer  at the current position
        /// - Parameters:
        ///     - ptr: const void ptr to the store data
        ///     - size: the amount of byre to read.
        /// - Returns: a reference to the bufer
        /// - Throws: If the buffer is null,ptr is null, or the read would exceed the buffer, a run time error is thrown.
        [[maybe_unused]] inline auto read(void *ptr, std::size_t amount) -> buffer_t& {
            auto bytesize = amount ;
            if ((read_data==nullptr) || (ptr==nullptr) ){
                throw std::runtime_error("Buffer/Ptr/Amount is empty");
            }
            if (current_position+bytesize >=length){
                throw std::out_of_range("Read would exceed buffer");
            }
            if (amount>0) {
                std::copy(static_cast<const char*>(read_data)+current_position,static_cast<const char*>(read_data)+current_position+amount,static_cast<char*>(ptr));
                current_position+=amount ;
            }
            return *this;
        }
        
        //=================================================================================
        /// Reads a fixed amount from the buffer  at the current position
        /// - Parameters:
        ///     - array: array to the data to read
        ///     - size: the amount of items to read (not bytes)
        /// - Returns: a reference to the bufer
        /// - Throws: If the buffer is null,ptr is null, or the read would exceed the buffer, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_array_v<T>,buffer_t&>::type
        read(T &value, size_t amount){
            if (read_data==nullptr){
                throw std::runtime_error("Buffer is empty");
            }
            
            auto charsize = sizeof(std::remove_all_extents_t<T>) ;
            auto bytesize = amount *charsize ;
            if ((current_position+bytesize) >= length){
                throw std::out_of_range("Read would exceed buffer");
            }
            
            std::copy(static_cast<const char*>(read_data)+current_position,static_cast<const char*>(read_data)+current_position+(bytesize*amount),reinterpret_cast<char*>(value));
            current_position+=bytesize;
            return *this ;
        }

        //=================================================================================
        // write
        //=================================================================================

        //=================================================================================
        /// Writes the data to buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// If the buffer is too small, and is expandable and is owning, then will auto expand the buffer
        /// - Parameters:
        ///     - value: the data to write
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is not writeable, or can not be expanded a runtime error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_integral_v<T>,buffer_t&>::type
        write(T value,bool reverse=false){
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto bytesize = sizeof(T) ;
            if (current_position+bytesize >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            if (reverse) {
                std::reverse(reinterpret_cast<std::uint8_t*>(&value), reinterpret_cast<std::uint8_t*>(&value)+bytesize);
            }
            
            std::copy(reinterpret_cast<std::uint8_t*>(&value),reinterpret_cast<std::uint8_t*>(&value)+bytesize,static_cast<std::uint8_t*>(write_data)+current_position);
            current_position+= bytesize;
            return *this ;
        }
        
        //=================================================================================
        /// writes a string view type to the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string view to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::string_view>|| std::is_same_v<T,std::u16string_view>|| std::is_same_v<T,std::wstring_view>|| std::is_same_v<T,std::u32string_view>,buffer_t&>::type
        write(const T value,std::size_t amount,bool reverse = false,std::uint32_t null_value = 0){
            if (write_data==nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto charsize = sizeof( typename T::value_type) ;
            auto bytesize = amount * charsize ;
            if (current_position+bytesize >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            auto iter = value.begin() ;
            typename T::value_type write_value ;
            auto count = std::size_t(0) ;
            while ((iter != value.end()) && (amount != 0)){
                write_value = *iter ;
                if (reverse){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&write_value),reinterpret_cast<std::uint8_t*>(&write_value)+charsize);
                }
                std::copy(reinterpret_cast<char*>(&write_value),reinterpret_cast<char*>(&write_value)+charsize,static_cast<char*>(write_data)+current_position+(count*charsize));
                count++;
                amount-- ;
                iter++;
            }
            if (amount >0){
                // we have to be here because we ran out of characters
                write_value = static_cast<typename T::value_type>(null_value) ;
                if (reverse){
                    std::reverse(reinterpret_cast<std::uint8_t*>(&write_value),reinterpret_cast<std::uint8_t*>(&write_value)+charsize);
                }
                while (amount >0) {
                    std::copy(reinterpret_cast<char*>(&write_value),reinterpret_cast<char*>(&write_value)+charsize,static_cast<char*>(write_data)+current_position+(count*charsize));
                    count++;
                    amount-- ;
                }
            }
            current_position+= bytesize;
            return *this ;
        }
        
        //=================================================================================
        /// writes a buffer data to the buffer at the current position
        /// - Parameters:
        ///     - ptr: The ptr to the data to write
        ///     - size: the amount of bytes to write.
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer, or the ptr is null and is not expandable/owning, a run time error is thrown.
        [[maybe_unused]] inline auto write(const void *ptr, std::size_t amount) ->buffer_t& {
            if ((ptr == nullptr) || (write_data==nullptr)){
                throw std::runtime_error("Ptr has no data/Buffer is not writeable");
            }
            auto bytesize = amount ;
            if (current_position+bytesize >=length){
                if (owning && is_expandable) {
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            if (amount >0){
                std::copy(static_cast<const char*>(ptr),static_cast<const char*>(ptr)+bytesize,static_cast<char*>(write_data));
                current_position+=bytesize;
            }
            return *this ;
        }

        //=================================================================================
        /// writes a string type to the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string  to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::string>,buffer_t&>::type
        write(const T &value,std::size_t amount,bool reverse = false,std::uint32_t null_value = 0){
            return this->write(static_cast<std::string_view>(value),amount,reverse,null_value);
        }
        //=================================================================================
        /// writes a u16string type to the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string  to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::u16string>,buffer_t&>::type
        write(const T &value,std::size_t amount,bool reverse = false,std::uint32_t null_value = 0){
            return this->write(static_cast<std::u16string_view>(value),amount,reverse,null_value);
        }
        //=================================================================================
        /// writes a u32string type to the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string  to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::u32string>,buffer_t&>::type
        write(const T &value,std::size_t amount,bool reverse = false,std::uint32_t null_value = 0){
            return this->write(static_cast<std::u32string_view>(value),amount,reverse,null_value);
        }
        //=================================================================================
        /// writes a wstring type to the buffer at the current position
        /// Will reverse the byte order (endian) based on the status of reverse
        /// - Parameters:
        ///     - value: The string  to be written
        ///     - size: the amount of characters (NOT BYTES) to write.  If the string view does not have sufficient characters, then the remaining is written the supplied null character
        ///     - reverse: optional, defaults to false. If true, will reverse the byte order
        ///     - null-value: The value for null. Defaults to 0
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_same_v<T,std::wstring>,buffer_t&>::type
        write(const T &value,std::size_t amount,bool reverse = false,std::uint32_t null_value = 0){
            return this->write(static_cast<std::wstring_view>(value),amount,reverse,null_value);
        }
        
        //=================================================================================
        /// writes an array to the buffer at the current position
        /// - Parameters:
        ///     - value: The string  to be written
        ///     - size: the amount of entries (NOT BYTES) to write from the array.
        /// - Returns: A reference to the buffer
        /// - Throws: If the buffer is null, or the write would exceed the buffer and is not expandable/owning, a run time error/out of range error is thrown.
        template <typename T>
        [[maybe_unused]] inline typename std::enable_if<std::is_array_v<T>,buffer_t&>::type write (const T &value,std::size_t amount){
            if (write_data == nullptr){
                throw std::runtime_error("Buffer is not writeable");
            }
            auto charsize = sizeof(std::remove_all_extents_t<T>);
            auto bytesize = charsize *amount ;
            if (current_position+bytesize >= length) {
                if (owning && is_expandable){
                    this->resize(current_position+bytesize);
                }
                else {
                    throw std::out_of_range("Write would exceed buffer");
                }
            }
            std::copy(reinterpret_cast<const char*>(value),reinterpret_cast<const char*>(value)+bytesize,static_cast<char*>(write_data)+current_position);
            current_position+=bytesize;
            return *this ;
        }
    };
}
#endif /* buffer_hpp */
