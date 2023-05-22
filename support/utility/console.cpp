//Copyright © 2023 Charles Kerr. All rights reserved.

#include "console.hpp"

#include <iostream>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#endif
#include "strutil.hpp"


using namespace std::string_literals ;

//=========================================================================================
namespace util{
    namespace console {
        //=========================================================================================
        auto moveCursorBy(direction_t direction,int amount) ->std::string{
            const auto command = "\x1b[%i%s"s ;
            auto cmd = "A"s;
            switch (direction) {
                case direction_t::up:{
                    cmd = "A"s;
                    break;
                }
                case direction_t::down:{
                    cmd = "B"s;
                    break;
                }
                case direction_t::forward:{
                    cmd = "C"s;
                    break;
                }
                case direction_t::backward:{
                    cmd = "D"s;
                    break;
                }
            }
            return util::format(command,amount,cmd.c_str());
        }
        //=========================================================================================
        auto setCursorTo(int row, int col) ->std::string {
            const auto command = "\x1b[%i;%iH"s ;
            return util::format(command,row,col);
        }
        //=========================================================================================
        auto setCursorTo(axis_t axis, int position) ->std::string {
            const auto command = "\x1b[%i%s"s;
            auto cmd = "G"s ;
            if (axis == axis_t::vertical){
                cmd = "d"s;
            }
            return util::format(command,position,cmd.c_str());
        }
        //=========================================================================================
        auto saveCursorPosition() ->std::string {
            return "\x1b[s"s ;
        }
        //=========================================================================================
        auto restoreCursorPosition()->std::string{
            return "\x1b[u"s ;
        }
        //=========================================================================================
        auto enableBlinking(bool state)->std::string{
            auto cmd = "\x1b[?12h"s;
            if (!state){
                cmd = "\x1b[?12l"s;
            }
            return cmd ;
        }
        //=========================================================================================
        auto hideCusor(bool state)->std::string{
            auto cmd = "\x1b[?25h"s;
            if (!state){
                cmd = "\x1b[?25l"s;
            }
            return cmd ;
        }
        //=========================================================================================
        auto setCursorShape(shape_t shape,bool blinking) ->std::string{
            auto cmd = std::string();
            switch(shape){
                case shape_t::user:{
                    cmd = "\0x1b[0 q"s;
                    break;
                }
                case shape_t::block:{
                    cmd = "\0x1b[2 q"s;
                    if (blinking){
                        cmd = "\0x1b[1 q"s;
                    }
                    break;
                }
                case shape_t::underline:{
                    cmd = "\0x1b[4 q"s;
                    if (blinking){
                        cmd = "\0x1b[3 q"s;
                    }
                    break;
                }
                case shape_t::bar:{
                    cmd = "\0x1b[6 q"s;
                    if (blinking){
                        cmd = "\0x1b[5 q"s;
                    }
                    break;
                }
            }
            return cmd ;
        }
        //=========================================================================================
        auto scrollUp(int amount) ->std::string {
            const auto cmd = "\x1b[%iS"s;
            return util::format(cmd, amount);
        }
        //=========================================================================================
        auto scrollDown(int amount) ->std::string {
            const auto cmd = "\x1b[%iT"s;
            return util::format(cmd, amount);
        }
        
        //=========================================================================================
        auto insertCharacter(int amount ) ->std::string {
            const auto  cmd = "\x1b[%i@"s ;
            return  util::format(cmd,amount) ;
        }
        //=========================================================================================
        auto deleteCharacter(int amount ) ->std::string {
            const auto  cmd = "\x1b[%iP"s ;
            return  util::format(cmd,amount) ;
        }
        //=========================================================================================
        auto eraseCharacter(int amount ) ->std::string {
            const auto  cmd = "\x1b[%iX"s ;
            return  util::format(cmd,amount) ;
        }
        //=========================================================================================
        auto insertLine(int amount ) ->std::string {
            const auto  cmd = "\x1b[%iL"s ;
            return  util::format(cmd,amount) ;
        }
        //=========================================================================================
        auto deleteLine(int amount ) ->std::string {
            const auto  cmd = "\x1b[%iM"s ;
            return  util::format(cmd,amount) ;
        }
        
        //=========================================================================================
        auto erase(scope_t scope,position_t position) -> std::string{
            auto cmd = "\x1b[%i"s;
            if (scope == scope_t::line){
                cmd +="K"s;
            }
            else {
                cmd +="J"s;
            }
            auto range = 0 ;
            switch (position){
                case position_t::beginning:{
                    range = 1;
                    break;
                }
                case position_t::end:{
                    range = 0 ;
                    break;
                }
                case position_t::entire:{
                    range = 2;
                    break;
                }
            }
            return util::format(cmd,range);
        }
        
        //=========================================================================================
        auto setAttribute(attr_t attr,bool enable) ->std::string {
            const auto cmd = "\x1b[%im"s ;
            auto value = 0 ;
            switch(attr) {
                case attr_t::normal:{
                    value = 0 ;
                    break;
                }
                case attr_t::bold:{
                    value = 22 ;
                    if (!enable){
                        value = 1 ;
                    }
                    break;
                }
                case attr_t::underline:{
                    value = 24 ;
                    if (!enable){
                        value = 4 ;
                    }
                    break;
                }
                case attr_t::inverse:{
                    value = 7 ;
                    if (!enable){
                        value = 27 ;
                    }
                    break;
                }
            }
            return util::format(cmd,value) ;
        }
        //=========================================================================================
        auto setColor(color_t color,bool bright,layer_t layer) ->std::string {
            const auto cmd = "\x1b[%im"s ;
            auto value = static_cast<int>(color) ;
            if (layer == layer_t::background){
                value += 10;
            }
            if (bright) {
                value +=60 ;
            }
            return util::format(cmd,value);
        }
        //=========================================================================================
        auto setScrollMargins(int top,int bottom) ->std::string {
            const auto cmd ="\x1b[%i;%ir"s;
            return util::format(cmd,top,bottom);
        }
        //=========================================================================================
        auto setWindowTitle(const std::string &title) ->std::string {
            const auto cmd ="\x1b]0;%s\x07"s;
            return util::format(cmd,title.c_str());
        }
        //=========================================================================================
        auto setBuffer(buffer_t buffer) ->std::string {
            auto cmd = "\x1b[?1049l"s;
            if (buffer == buffer_t::secondary){
                cmd = "\x1b[?1049h"s ;
            }
            return cmd;
        }
        
        //=========================================================================================
        auto resetConsole() ->std::string {
            return "\x1b[!p"s ;
        }
        //=========================================================================================
        auto setVirtualTerminal() ->void {
#if defined(_WIN32)
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE){
                throw std::runtime_error("Unable to obtain handle to set virtual terminal");
            }
            
            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)){
                throw std::runtime_error("Unable to obtain current mode to set virtual terminal");
            }
            
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode)){
                throw std::runtime_error("Unable to set virtual terminal");
            }
            
#endif
        }
    }
}
