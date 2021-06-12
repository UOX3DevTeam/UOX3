// 
// Created on:  6/8/21

#ifndef IP4Address_hpp
#define IP4Address_hpp

#include <cstdint>
#include <string>
#include <vector>
class IP4Address {
private:
	std::vector<std::string> _components ;

	static std::vector<IP4Address> _myIPs ;

	static std::vector<IP4Address> available() ;
	static IP4Address _externalIP ;
	static const std::vector<IP4Address> _privateIPs ;
	static const IP4Address _localIP;
	static const IP4Address _APIPA ;
	
	// Break an ip into sections
	std::vector<std::string> parseIP(const std::string &ip);

	// Get an ip text into an unsigned int
	unsigned int createIP(const std::string &ip);
	unsigned int convert(int start, int increment) const;
	static const IP4Address& bestMatch(const IP4Address &address);
public:
	static void loadIPs() ;
	enum typeIP { wan,lan,local,apipa,mine};
	typeIP type(bool notmine = true ) const ;
	IP4Address() ;
	IP4Address(unsigned int address);
	IP4Address(const std::string &address);
	IP4Address& operator=(const std::string &address);
	IP4Address& operator=(const unsigned int &address);
	bool operator==(const IP4Address &address) const ;
	bool operator!=(const IP4Address &address) const ;
	
	bool match(const IP4Address &address, int level) const ;
	unsigned int littleEndian() const ;
	unsigned int bigEndian() const;
	std::string string() const ;
	bool valid() const ;
	static void setExternal(const std::string &address) ;
	static IP4Address lookup(const std::string& address);
	static IP4Address respond(IP4Address &address);
	static std::vector<IP4Address> deviceIPs() ;
	static std::string externalIP()  ;
	
	
};

#endif /* IP4Address_hpp */
