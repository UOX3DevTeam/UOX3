//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef timer_hpp
#define timer_hpp

#include <cstdint>
#include <string>
#include <chrono>
//=========================================================================================
namespace util {
    //=================================================================================
    /// Class that allows one to time events, in terms of milliseconds
    class timer_t{
        std::chrono::time_point<std::chrono::steady_clock> start_time ;
        std::int64_t time_duration ;
    public:
        //=================================================================================
        /// Provides the current time
        /// - Returns: A string value of the current time
        static auto now() ->std::string ;

        //=================================================================================
        /// Provides the current time point for the steady clock
        /// - Returns: Steady clock time point
        static auto current() ->std::chrono::system_clock::time_point;
        
        //=================================================================================
        /// Provides the number of millisconds from a timepoint
        /// - Parameters:
        ///     - time: The time point to delta from
        /// - Returns: millisconds from the time point
        static auto delta(const std::chrono::steady_clock::time_point &time) ->std::int64_t;

        //=================================================================================
        /// Default constructor for the timer
        /// - Returns:Nothing
        timer_t();

        //=================================================================================
        /// Constructor for the timer
        /// - Parameters:
        ///     - milliseconds: The number of millisonds it should time
        ///     - block: If true, will block until time has elapsed
        /// - Returns:Nothing
        timer_t(std::int64_t milliseconds,bool block);

        //=================================================================================
        /// Sets the time to measure from
        /// - Parameters:
        ///     - milliseconds: The number of millisonds it should time
        ///     - block: If true, will block until time has elapsed
        /// - Returns:Nothing
        auto time(std::int64_t milliseconds,bool block)->void;

        //=================================================================================
        /// Starts the timer
        /// - Returns:Nothing
        auto start() ->void ;

        //=================================================================================
        /// Calculates the number of millisconds since started
        /// - Returns:Returns the number of milliseconds that have elapsed since started
        auto elapsed() const ->std::int64_t ;

        //=================================================================================
        /// Determines if the time has expired
        /// - Returns:Returns true if the number of milliseconds has expired
        auto expired() const ->bool;

        //=================================================================================
        /// Determines the remaining milliseconds 
        /// - Returns:Returns the number of milliseconds that remain
        auto remaining() const -> std::int64_t ;
    };
}
#endif /* timer_hpp */
