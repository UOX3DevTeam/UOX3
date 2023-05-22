//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef console_hpp
#define console_hpp

#include <cstdint>
#include <string>
#include <array>
//=========================================================================================
namespace util{
    namespace console {
        enum class direction_t {up,down,forward,backward};
        enum class axis_t {vertical,horizontal};
        enum class position_t {beginning,end,entire};
        enum class scope_t{display,line};
        enum class shape_t {user,block,underline,bar} ;
        enum class attr_t{normal,bold,underline,inverse};
        enum class cursor_t{normal,blink};
        enum class color_t{black=30,red,green,yellow,blue,magenta,cyan,white,def=39};
        enum class layer_t{foreground,background};
        enum class buffer_t{primary,secondary};
        
        auto moveCursorBy(direction_t direction,int amount) ->std::string;
        auto setCursorTo(int row, int col) ->std::string ;
        auto setCursorTo(axis_t axis, int position) ->std::string ;
        auto saveCursorPosition() ->std::string ;
        auto restoreCursorPosition()->std::string;
        auto enableBlinking(bool state)->std::string;
        auto hideCusor(bool state)->std::string;
        auto setCursorShape(shape_t shape,bool blinking) ->std::string ;
        auto scrollUp(int amount) ->std::string ;
        auto scrollDown(int amount) ->std::string ;
        
        auto insertCharacter(int amount ) ->std::string ;
        auto deleteCharacter(int amount ) ->std::string ;
        auto eraseCharacter(int amount ) ->std::string ;
        auto insertLine(int amount ) ->std::string ;
        auto deleteLine(int amount ) ->std::string ;
        
        auto erase(scope_t scope,position_t position) -> std::string ;
        
        auto setAttribute(attr_t attr,bool enable) ->std::string ;
        auto setColor(color_t color,bool bright,layer_t layer) ->std::string ;
        
        auto setScrollMargins(int top,int bottom) ->std::string ;
        auto setWindowTitle(const std::string &title) ->std::string;
        auto setBuffer(buffer_t buffer) ->std::string;
        auto resetConsole() ->std::string  ;
        auto setVirtualTerminal() ->void ;
    }
}
#endif /* console_hpp */
