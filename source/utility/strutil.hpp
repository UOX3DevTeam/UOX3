// Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef strutil_h
#define strutil_h
#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

//============================================================================

namespace util {
    
    //========================================================================
    // Trim utilities
    //=======================================================================
    
    //=======================================================================
    /// Trim all whitespace from the left of the string
    /// - Parameters:
    ///     - value: The string to be trimmed
    ///     - whitespace: optional value to specify the whitespace characters
    /// - Returns: Returns a new string, that has been trimmed
    inline auto ltrim(const std::string &value, const std::string &whitespace = " \t\v\f\n\r") -> std::string {
        if (!value.empty() && !whitespace.empty()) {
            auto loc = value.find_first_not_of(whitespace);
            if (loc == std::string::npos) {
                return std::string();
            }
            return value.substr(loc);
        }
        return value;
    }
    
    //=======================================================================
    /// Trim all whitespace from the right of the string
    /// - Parameters:
    ///     - value: The string to be trimmed
    ///     - whitespace: optional value to specify the whitespace characters
    /// - Returns: Returns a new string, that has been trimmed
    inline auto rtrim(const std::string &value, const std::string &whitespace = " \t\v\f\n\r") -> std::string {
        if (!value.empty() && !whitespace.empty()) {
            auto loc = value.find_last_not_of(whitespace);
            if (loc == std::string::npos) {
                return std::string();
            }
            return value.substr(0, loc + 1);
        }
        return value;
    }
    
    //=======================================================================
    /// Trim all whitespace from both sides of the string
    /// - Parameters:
    ///     - value: The string to be trimmed
    ///     - whitespace: optional value to specify the whitespace characters
    /// - Returns: Returns a new string , that has been trimmed
    inline auto trim(const std::string &value, const std::string &whitespace = " \t\v\f\n\r") -> std::string {
        return rtrim(ltrim(value, whitespace), whitespace);
    }
    
    //=======================================================================
    /// Simplify the string.
    /// Removes all whitespace from both sides of the string, and then replaces
    /// whitespace between words with a single space character
    /// - Parameters:
    ///     - value: The string to be simplified
    ///     - whitespace: optional value to specify the whitespace characters
    /// - Returns: Returns a new string, that has been simplified
    inline auto simplify(const std::string &value, const std::string &whitespace = " \t\v\f\n\r") -> std::string {
        auto rvalue = std::string();
        auto working = trim(value, whitespace);
        if (!working.empty()) {
            auto ewhitespace = working.find_first_not_of(whitespace);
            auto swhitespace = working.find_first_of(whitespace);
            if (swhitespace != std::string::npos) {
                while ((swhitespace != std::string::npos) && (swhitespace < working.size())) {
                    if (!rvalue.empty()) {
                        rvalue += std::string(" ");
                    }
                    rvalue += std::string(working.begin() + ewhitespace, working.begin() + ewhitespace + (swhitespace - ewhitespace)); // So it has now been appended
                    ewhitespace = working.find_first_not_of(whitespace, swhitespace);
                    swhitespace = working.find_first_of(whitespace, ewhitespace);
                    if (swhitespace == std::string::npos) {
                        // No whitespace found, so we take what is ever left
                        if (ewhitespace != std::string::npos) {
                            if (!rvalue.empty()) {
                                rvalue += std::string(" ");
                            }
                            rvalue += std::string(working.begin() + ewhitespace, working.end());
                        }
                    }
                }
            }
            else {
                // There was no whitespace, so just copy the whole thing
                rvalue = std::string(working.begin(), working.end());
            }
        }
        return rvalue;
    }
    
    //=========================================================
    // Case utilities
    //=========================================================
    
    //=======================================================================
    /// Upper case the string
    /// - Parameters:
    ///     - value: The string to be uppercased
    /// - Returns: Returns a new string , that has been uppercased
    inline auto upper(const std::string &value) -> std::string {
        auto rvalue = std::string();
        std::transform(value.begin(), value.end(), std::back_inserter(rvalue), [](unsigned char c) { return std::toupper(c); });
        return rvalue;
    }
    //=======================================================================
    /// lower case the string
    /// - Parameters:
    ///     - value: The string to be trimmed
    /// - Returns: Returns a new string , that has been lower cased
    inline auto lower(const std::string &value) -> std::string {
        auto rvalue = std::string();
        std::transform(value.begin(), value.end(), std::back_inserter(rvalue), [](unsigned char c) { return std::tolower(c); });
        return rvalue;
    }
    
    //=========================================================
    // String manipulation
    //=========================================================
    
    //=======================================================================
    /// Strip all text after the specified separator
    /// - Parameters:
    ///     - value: The string  to be worked on
    ///     - sep: optional, The separator to search for for removal
    ///     - whitespace: optional, if present(not empty), the data is trimmed
    /// - Returns: Returns a new string , that has all data after the separator removed (inclusive)
    inline auto strip(const std::string &value, const std::string &sep = "//", const std::string &whitespace = " \t\v\f\n\r") -> std::string {
        auto rvalue = std::string();
        auto temp = value;
        if (!sep.empty()) {
            auto loc = value.find(sep);
            if (loc != std::string::npos) {
                temp = value.substr(0, loc);
            }
        }
        temp = rtrim(temp, whitespace);
        
        return std::string(temp.begin(), temp.end());
    }
    
    //=======================================================================
    /// Split a text string into to values, based on a separator
    /// The values are trimmed based on whitespace value
    /// - Parameters:
    ///     - value: The string view to be worked on
    ///     - sep: The separator to search for to split the string on
    ///     - whitespace: optional, if present(not empty), the two values are trimmed on return
    /// - Returns: Returns a pair of string views containing the two values
    inline auto split(const std::string &value, const std::string &sep, const std::string &whitespace = " \t\v\f\n\r") -> std::pair<std::string, std::string> {
        auto vfirst = value;
        auto vsecond = std::string();
        if (!sep.empty()) {
            auto loc = value.find(sep);
            if (loc != std::string ::npos) {
                vfirst = trim(value.substr(0, loc), whitespace);
                loc = loc + sep.size();
                if (loc < value.size()) {
                    vsecond = trim(value.substr(loc), whitespace);
                }
            }
        }
        return std::make_pair(vfirst, vsecond);
    }
    
    //=======================================================================
    /// Parse a text string into to values, based on a separator
    /// The values are trimmed based on whitespace value
    /// - Parameters:
    ///     - value: The string to be worked on
    ///     - sep: The separator to search for to split the string on
    ///     - whitespace: optional, if present(not empty), the two values are trimmed on return
    /// - Returns: Returns a vector containing the values
    inline auto parse(const std::string value, const std::string &sep, const std::string whitespace = " \t\v\f\n\r") -> std::vector<std::string> {
        auto rvalue = std::vector<std::string>();
        auto current = std::string::size_type(0);
        if (!sep.empty()) {
            auto loc = value.find(sep, current);
            while (loc != std::string::npos) {
                
                rvalue.push_back(trim(value.substr(current, loc - current), whitespace));
                current = loc + sep.size();
                if (current >= value.size()) {
                    rvalue.push_back(std::string());
                }
                loc = value.find(sep, current);
            }
            if (current < value.size()) {
                rvalue.push_back(trim(value.substr(current), whitespace));
            }
        }
        else {
            rvalue.push_back(trim(value));
        }
        return rvalue;
    }
    //=======================================================================
    /// Acquire the contents of the string between two separators
    /// The values are trimmed based on whitespace value
    /// - Parameters:
    ///     - value: The string to be worked on
    ///     - sepStart: The starting separator
    ///     - sepEnd: The ending separator
    ///     - trimReturn: Bool to indicate if result should be trimmed. Defaults to true
    /// - Returns: Returns a string\_view of the contents
    /// - Throws: runtime error if separators are not found or the end is before the beginning
    inline auto contentsOf(const std::string &data, const std::string &sepStart, const std::string &sepEnd, bool trimReturn = true) -> std::string {
        auto startloc = data.find_first_of(sepStart);
        auto endloc = data.find_last_of(sepEnd);
        if ((startloc == std::string::npos) || (endloc == std::string::npos) || (startloc >= endloc)) {
            // throw std::runtime_error(std::string("Unable to retrieve contents from: ") +
            // std::string(data));
            //  Keeping with our approach of system functions, so no exceptions, we return a blank
            return std::string();
        }
        auto actualstart = startloc + sepStart.size();
        auto contents = data.substr(actualstart, endloc - actualstart);
        if (trimReturn) {
            contents = trim(contents);
        }
        return contents;
    }
    
    //==========================================================
    // String formatting
    //==========================================================
    
    //=======================================================================
    /// Format a string based on a string format statement
    ///
    /// - Parameters:
    ///     - format: string of the format statment
    ///     - args: argument list for the values in the format statement
    /// - Returns: Returns a string formatted based on the arguments/format statment
    /// - Throws: If it can not allocate the memory for conversion, throws a runtime error
    template <typename... Args>
    auto format(const std::string &format_str, Args... args) -> std::string {
        auto rvalue = std::string();
        if (!format_str.empty()) {
            // First see how much space we need?
            auto size_s = std::snprintf(nullptr, 0, format_str.c_str(), args...);
            if (size_s < 0) {
                throw std::runtime_error("Error applying format string");
            }
            if (size_s > 0) {
                // Take the space we need and add 1 for the terminating \0
                size_s += 1;
                auto size = static_cast<std::size_t>(size_s);
                // Lets create a buffer we need for the data
                auto buf = std::make_unique<char[]>(size);
                size_s = std::snprintf(buf.get(), size, format_str.c_str(), args...);
                if (size_s < 0) {
                    throw std::runtime_error("Error applying format string");
                }
                if (size_s > 0) {
                    rvalue = std::string(buf.get(), buf.get() + size_s);
                }
            }
        }
        return rvalue;
    }
    //==========================================================
    // Number/string conversions
    // These are equivalanet to std::stoi type functions, but allow
    // one to fix width, pad, etc.
    //==========================================================
    //==========================================================
    // The maximum characters in a string number for conversion sake
    // Used by ntos method
    inline constexpr auto max_characters_in_number = 50;
    
    //=======================================================================
    /// Convert a number to a string, with options on radix,prefix,size,pad
    /// - Parameters:
    ///     - value: The value one is trying to convert to a string
    ///     - radix: the radix one wants, 10,16,2,8
    ///     - prefix: Should the string have the prefix (0x,0,0b) (optional, defaults to false)
    ///     - width: The minimum width the result should be (optional, defaults to 0) ;
    ///     - pad: The pad character to be used (optional, defaults to '0'
    /// - Returns: Returns a string formatted based on the arguments/format statment
    /// - Throws: If the conversion can not be performed, throws a runtime error.
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, std::string>
    ntos(T value, int radix = 10, bool prefix = false, int size = 0, char pad = '0') {
        // first, thing we need to convert the value to a string
        std::array<char, max_characters_in_number> str;
        
        if (auto [pc, ec] = std::to_chars(str.data(), str.data() + str.size(), value, static_cast<int>(radix)); ec == std::errc()) {
            // How many characters did this number take
            auto numchars = static_cast<int>(std::distance(str.data(), pc));
            // what is larger, that is the size of our string
            auto sizeofstring = std::max(numchars, size);
            // where do we start adding the number into our string ?
            auto index = sizeofstring - numchars;
            if (prefix) {
                // We have a prefix, so we add two characters to the beginning
                sizeofstring += 2;
                index += 2;
            }
            auto rvalue = std::string(sizeofstring, pad);
            // copy the value into the string
            std::copy(str.data(), pc, rvalue.begin() + index);
            // do we need our prefix?
            if (prefix) {
                switch (static_cast<int>(radix)) {
                    case 10:
                        // We dont add anything for decimal!
                        break;
                    case 16:
                        rvalue[0] = '0';
                        rvalue[1] = 'x';
                        break;
                    case 8:
                        rvalue[0] = '0';
                        rvalue[1] = 'o';
                        break;
                    case 2:
                        rvalue[0] = '0';
                        rvalue[1] = 'b';
                        break;
                        
                    default:
                        break;
                }
            }
            return rvalue;
        }
        else {
            // The conversion was not successful, so we return an empty string
            throw std::runtime_error("Unable to convert the value: ");
            // return std::string();
        }
    }
    //=======================================================================
    /// Convert a bool to a string,
    /// - Parameters:
    ///     - value: The value one is trying to convert to a string
    /// - Returns: Returns a string (true/false);
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T> && std::is_same_v<T, bool>, std::string>
    ntos(T value) {
        // first, thing we need to convert the value to a string
        if (value) {
            
            return "true";
        }
        return "false";
    }
    //=======================================================================
    /// Convert a string to a number, with options on radix
    /// - Parameters:
    ///     - value: The value one is trying to convert to a string
    ///     - radix: the radix of the value (if no prefix) ;
    /// - Returns: the requested number
    /// - Throws: If unable to convert it, or has a format error, throws a runtime error.
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, T>
    ston(const std::string &str_value, int radix = 10) {
        auto value = T{0};
        // auto svalue = std::string(str_value);
        if (!str_value.empty()) {
            if (str_value.size() < 2) {
                // std::from_chars(str_value.data(),str_value.data() + str_value.size(), value,radix);
                std::from_chars(str_value.data(), str_value.data() + str_value.size(), value, radix);
            }
            else if (std::isalpha(static_cast<int>(static_cast<int>(str_value[1])))) {
                // This has a "radix indicator"
                switch (str_value[1]) {
                    case 'b':
                    case 'B':
                        // std::from_chars(str_value.data() + 2,str_value.data() + str_value.size(),
                        // value,2);
                        std::from_chars(str_value.data() + 2, str_value.data() + str_value.size(), value, 2);
                        break;
                    case 'x':
                    case 'X':
                        // std::from_chars(str_value.data() + 2,str_value.data() + str_value.size(),
                        // value,16);
                        std::from_chars(str_value.data() + 2, str_value.data() + str_value.size(), value, 16);
                        break;
                    case 'o':
                    case 'O':
                        // std::from_chars(str_value.data() + 2,str_value.data() + str_value.size(),
                        // value,8);
                        std::from_chars(str_value.data() + 2, str_value.data() + str_value.size(), value, 8);
                        break;
                    default:
                        // we dont do anything, we dont undertand so let value be 0
                        break;
                }
            }
            else {
                // auto [ptr,ec] = std::from_chars(str_value.data(),str_value.data() + str_value.size(),
                // value,radix);
                auto [ptr, ec] = std::from_chars(str_value.data(), str_value.data() + str_value.size(), value, radix);
                if (ec == std::errc::invalid_argument) {
                    throw std::runtime_error("Invalid argument for number conversion from string.");
                }
                else if (ec == std::errc::result_out_of_range) {
                    throw std::runtime_error("Out of range for number conversion from string.");
                }
                return value;
            }
        }
        return value;
    }
    //=======================================================================
    /// Convert a string(true/false) to a bool
    /// - Parameters:
    ///     - value: The value one is trying to convert to a string
    ///     - true_value: what text would indicate true (optional, defaults to "true"  ;
    /// - Returns: a boolean of true if the text equals the true value (or if it converts as a number to
    /// not 0)
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T> && std::is_same_v<T, bool>, T>
    ston(const std::string &str_value, const std::string &true_value = "true") {
        // If string empty, we return false
        //  we take advantege, that if in ston() we set value to 0 false, and if
        //  the from_chars fails, it doesn't touch value
        auto numvalue = ston<int>(str_value);
        if ((str_value == true_value) || (numvalue != 0)) {
            return true;
        }
        return false;
    }
    //=======================================================================
    /// Convert a string to a real
    /// - Parameters:
    ///     - value: The value one is trying to convert to a string
    /// - Returns: a boolean of true if the text equals the true value (or if it converts as a number to
    /// not 0)
    /// - Throws: If unable to convert it, or has a format error, throws a runtime error.
    template <typename T>
    typename std::enable_if_t<std::is_floating_point_v<T>, T> ston(const std::string &str_value) {
        // If string empty, we return false
        auto value = T{0.0};
        if (str_value.empty()) {
            return value;
        }
        auto [ptr, ec] = std::from_chars(str_value.data(), str_value.data() + str_value.size(), value);
        if (ec == std::errc::invalid_argument) {
            throw std::runtime_error("Invalid argument for number conversion from string.");
        }
        else if (ec == std::errc::result_out_of_range) {
            throw std::runtime_error("Out of range for number conversion from string.");
        }
    }
    
    //=======================================================================
    /// Dump a byte buffer in the standard value : alpha format
    /// - Parameters:
    ///     - output: the output (ostream)
    ///     - buffer: a pointer to the data buffer of bytes
    ///     - length: the length of the buffer (bytes)
    ///     - radix: (16,10,8,2) the radix of the output
    ///     - entries per line: the number of byes printed per line (defaults to 8)
    /// - Returns: Nothing
    inline auto dump(std::ostream &output, const std::uint8_t *buffer, std::size_t length, int radix = 16, int entries_line = 8) -> void {
        // number of characters for entry
        auto entry_size = 3; // decimal and octal
        switch (static_cast<int>(radix)) {
            case 16:
                entry_size = 2;
                break;
            case 2:
                entry_size = 8;
                break;
            default:
                break;
        }
        auto num_rows = (length / entries_line) + (((length % entries_line) == 0) ? 0 : 1);
        // what is the largest number for the address ?
        auto max_address_chars = static_cast<int>((ntos(num_rows * entries_line)).size());
        
        // first write out the header
        output << std::setw(max_address_chars + 2) << "" << std::setw(1);
        for (auto i = 0; i < entries_line; ++i) {
            output << ntos(i, 10, false, entry_size, ' ') << " ";
        }
        output << "\n";
        
        // now we write out the values for each line
        std::string text(entries_line, ' ');
        
        for (std::size_t i = 0; i < length; ++i) {
            auto row = i / entries_line;
            if (((i % static_cast<std::size_t>(entries_line) == 0) && (i >= static_cast<std::size_t>(entries_line))) ||
                (i == 0)) {
                // This is a new line!
                output << ntos(row * entries_line, 10, false, max_address_chars, ' ') << ": ";
                text = std::string(entries_line, ' ');
            }
            output << ntos(*(buffer + i), radix, false, entry_size) << " ";
            // If it is an alpha, we want to write it
            if (std::isalpha(static_cast<int>(*(buffer + i))) != 0) {
                // we want to write this to the string
                text[(i % entries_line)] = static_cast<char>(*reinterpret_cast<const char *>((buffer + i)));
            }
            else {
                text[(i % entries_line)] = '.';
            }
            if (i % entries_line == entries_line - 1) {
                output << " " << text << "\n";
            }
        }
        // what if we had a partial last line, we need to figure that out
        auto last_line_entry = length % entries_line;
        if (last_line_entry != 0) {
            // we need to put the number of leading spaces
            output << std::setw(static_cast<int>((entries_line - last_line_entry) * (entry_size + 1))) << "" << std::setw(1) << " " << text << "\n";
        }
    }
    
    //=========================================================
    // Time/String conversions
    //=========================================================
    
    //=======================================================================
    /// Converts a system clock time point to a string value
    /// - Parameters:
    ///     - t: the time point
    ///     - format: the format string (defaults to Thu Dec 30 14:13:28 2021)
    /// - Returns: string value of the time point
    inline auto sysTimeToString(const std::chrono::system_clock::time_point &t, const std::string &format = "%a %b %d %H:%M:%S %Y") -> std::string {
        std::stringstream output;
        auto time = std::chrono::system_clock::to_time_t(t);
        tm myvalue;
#if defined(_MSC_VER)
        auto status = ::localtime_s(&myvalue, &time);
#else
        ::localtime_r(&time, &myvalue);
#endif
        output << std::put_time(&myvalue, format.c_str());
        
        return output.str();
    }
    
    //=======================================================================
    /// Converts a string value of time to a system time point
    /// - Parameters:
    ///     - str: the string value for the time point
    ///     - format: the string value representing the format (Thu Dec 30 14:13:28 2021)
    /// - Returns: a system time point
    inline auto stringToSysTime(const std::string &str, const std::string &format = "%a %b %d %H:%M:%S %Y") -> std::chrono::system_clock::time_point {
        std::stringstream timbuf(str);
        tm converted;
        timbuf >> std::get_time(&converted, format.c_str());
        converted.tm_isdst = -1;
        auto ntime = mktime(&converted);
        return std::chrono::system_clock::from_time_t(ntime);
    }
    
    //=======================================================================
    /// Returns time now in a string
    /// - Parameters:
    ///     - format: the format string (defaults to Thu Dec 30 14:13:28 2021)
    /// - Returns: string value of the time point
    inline auto timeNow(const std::string &format = "%a %b %d %H:%M:%S %Y") -> std::string {
        return sysTimeToString(std::chrono::system_clock::now(), format);
    }
} // namespace util
#endif /* Header guard */
