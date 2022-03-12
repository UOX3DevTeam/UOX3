//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef netexcept_hpp
#define netexcept_hpp
#include "framework.h"
#include <cstdint>
#include <stdexcept>
#include <string>
//=========================================================
//=========================================================
// socket_exp
//=========================================================
struct socket_exp :public std::runtime_error {
	int error ;
	std::string errormsg ;
	socket_exp(int errornum,const std::string &message);
};

//=========================================================
// socket_close_create
//=========================================================
struct socket_close_create :public std::runtime_error {
	int error ; // 0 = connect/listen failed, 1 = socket closed
	std::string errormsg ;
	socket_close_create(int errornum,const std::string &message);
};
#endif /* netexcept_hpp */
