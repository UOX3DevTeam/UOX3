//Copyright © 2022 Charles Kerr. All rights reserved.

#include "netexcept.hpp"

#include <iostream>

//=========================================================
//=========================================================
// socket_exp
//=========================================================
socket_exp::socket_exp(int errornum,const std::string &message): std::runtime_error(message),error(errornum),errormsg(message){
}
//=========================================================
// socket_close_create
//=========================================================
socket_close_create::socket_close_create(int errornum,const std::string &message): std::runtime_error(message),error(errornum),errormsg(message){
}

