
#ifndef GenericList_h
#define GenericList_h
#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <queue>

//===========================================================
// GenericList
//===========================================================

template <typename T>
class GenericList
{
private:
	std::list<T> objData;
	std::queue<T> stack ;
	mutable T current = nullptr ;
	
	auto currentIterator() -> typename std::list<T>::iterator {
		return std::find_if(objData.begin(),objData.end(),[this](T &value){
			return (value) == this->current;
		});
	}
	
public:
	//===========================================================
	GenericList():current(nullptr) {} ;
	//===========================================================
	~GenericList() = default ;
	//=========================================================
	auto storage() ->std::list<T>& {return objData;}
	auto storage() const ->const std::list<T>& {return objData;}
	//===========================================================
	auto GetCurrent() ->T {
		auto iter = currentIterator();
		if (iter != objData.end()){
			return *iter;
		}
		return nullptr ;
	}
	//===========================================================
	auto Push() ->void {
		stack.push(current) ;
	}
	//===========================================================
	auto Pop() ->void {
		current = nullptr ;
		if (!stack.empty()){
			auto possible = current ;
			
			do{
				possible = stack.front() ;
				stack.pop() ;
				auto iter = std::find_if(objData.begin(),objData.end(),[possible](T &value){
					return value = possible ;
				});
				if (iter != objData.end()){
					current = possible;
					break;
				}
			}while (!stack.empty());
		}
	}
	//===========================================================
	auto First(bool setcurrent = true) ->T{
		auto iter = objData.begin() ;
		if (iter != objData.end()){
			if (setcurrent) {
				current = *iter ;
			}
			return *iter ;
		}
		return nullptr ;
	}
	
	//===========================================================
	auto Next(bool setcurrent = true ) ->T{
		if (current == nullptr){
			return First(setcurrent) ;
		}
		auto iter = currentIterator() ;
		if (iter != objData.end()){
			// It is valid, lets increment it
			iter++ ;
			current = nullptr ;
			if (iter != objData.end()){
				if (setcurrent){
					current = *iter ;
				}
				return *iter ;
			}
		}
		current = nullptr;
		return nullptr ;
	}
	
	//===========================================================
	auto Finished() ->bool{
		return current == nullptr;
	}
	
	//===========================================================
	auto Num() const ->size_t {
		return objData.size() ;
	}
	
	//===========================================================
	auto Clear() ->void {
		objData.clear() ;
	}
	
	//===========================================================
	auto Add( T toAdd, bool checkForExisting = true ) ->bool {
		auto rvalue = true ;
		// we always check for existing, or we break
		// we have the method for compatability
		
		auto iter = std::find_if(objData.begin(),objData.end(),[toAdd](T &value){
			return (value) == toAdd;
		});
		if (iter != objData.end() ){
			rvalue = false ;
		}
		
		if (rvalue) {
			objData.push_back(toAdd) ;
		}
		return rvalue ;
		
	}
	//===========================================================
	auto AddInFront( T toAdd, bool checkForExisting = true ) ->bool{
		auto rvalue = true ;
		//if (checkForExisting){
		auto iter = std::find_if(objData.begin(),objData.end(),[toAdd](T &value){
			return (value) == toAdd;
		});
		if (iter != objData.end() ){
			rvalue = false ;
		}
		//}
		if (rvalue) {
			objData.push_front(toAdd) ;
		}
		return rvalue ;
	}
	
	//===========================================================
	auto Remove( T toRemove, bool handleAlloc = false ) ->bool {
		auto rvalue = false ;
		auto iter = std::find_if(objData.begin(),objData.end(),[toRemove](T &value){
			return (value) == toRemove;
		});
		if (iter !=  objData.end()){
			rvalue = true ;
			
			auto temp = objData.erase(iter) ;
			if (toRemove == current ) {
				auto front = --temp ;
				if (front != objData.end()){
					current = *front ;
				}
				else {
					current = nullptr ;
				}
			}
			if (handleAlloc) {
				delete toRemove ;
			}
			
		}
		return rvalue ;
	}
	//===========================================================
	auto Reverse()  ->void {
		objData.reverse();
	}
	
	//===========================================================
	auto Sort() ->void  {
		objData.sort();
	}
	
	//===========================================================
	auto Sort( bool Comparer( T one, T two )) ->void {
		objData.sort(Comparer);
	}
};



//===========================================================
// RegionSerialList
//===========================================================
// I think someone was very confused about templates.  I believe they used
// the typename as SERIAL (it orginally wasnt T, but SERIAL, I modified it to be more obvious.
// If always type SERIAL, doedn't need to be a template.
// Doesnt' look like in the code a lot to fix, probably should be done.
template <typename T>
class RegionSerialList {
private:
	std::unordered_set<T> objSerials;
public:
	RegionSerialList()=default;
	
	~RegionSerialList() = default ;
	
	//===========================================================
	auto Add( T toAdd )->bool{
		auto insertResult = objSerials.insert( toAdd );
		return insertResult.second;
	}
	
	//===========================================================
	auto Remove(T toRemove) ->size_t {
		return objSerials.erase( toRemove );
	}
};

#endif /* GenericList_h */
