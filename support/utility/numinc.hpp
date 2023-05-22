//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef numinc_h
#define numinc_h
#include <cstdint>
#include <string>
#include <limits>
#include <algorithm>

//============================================================================
namespace util{
    //=================================================================================
    /// Provides a one up number generator.
    template <typename T>
    class numinc_t {
        static_assert(std::is_integral_v<T>,
                      "numic_t requires integral types");
        T current_value ;
        T limit ;
        
    public:
        //=================================================================================
        /// Provides the next value and increments the internal value
        /// - Returns: Returns the next value
        /// - Throws: If the value would exceed the limit, will throw a std::runtome_error
        auto next() ->T {
            if ((current_value+1) == limit){
                throw std::runtime_error(std::string("Numeric limit has been reached: ") + std::to_string(limit));
            }
            return ++current_value;
            
        }
        
        //=================================================================================
        /// Informs the generator that the specified value is used.  If the current value is less then that
        /// then the current value is set to the provided value
        /// - Parameters:
        ///     - value: The value that has been used.
        /// - Returns: A reference to the generator
        /// - Throws: If the value would exceed the limit, will throw a std::runtome_error
        [[maybe_unused]] auto used(T value) ->numinc_t&{
            current_value=std::max(current_value,value);
        }
        //=================================================================================
        /// Default constructor for the generator.
        /// The limit is to the maximum numeric limit for the number type.
        /// - Parameters:
        ///     - initial: the initial starting value for the number generator
        /// - Returns: Nothing
         numinc_t(T initial=0):current_value(initial),limit(std::numeric_limits<T>::max()){};
        //=================================================================================
        /// Constructor for the generator.
        /// - Parameters:
        ///     - initial: the initial starting value for the number generator
        ///     - max value: the maximum value (limit) for the number generator
        /// - Returns: Nothing
        numinc_t(T initial, T max_value):current_value(initial),limit(max_value){};
        
    };
}
#endif /* Header guard */
