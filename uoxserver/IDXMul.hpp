// 
// Created on:  4/28/21

#ifndef IDXMul_hpp
#define IDXMul_hpp

#include <cstdint>
#include <string>
#include <vector>


//===========================================================================
class IDXMul {
public:
	struct entry {
		int offset ;
		int length ;
		int extra ;
		bool valid() const {
			return !((length ==0) || (offset ==-1) || (offset == -2));
		}
	};
private:
	std::vector<entry> _entries ;
	
public:
	IDXMul() ;
	IDXMul(const std::string& filepath);
	void load(const std::string &filepath);
	std::size_t size() const ;
	const entry& operator[](std::size_t index) const;
};


#endif /* IDXMul_hpp */
