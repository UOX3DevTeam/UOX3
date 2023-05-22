//Copyright © 2023 Charles Kerr. All rights reserved.

#include "filemap.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstring>

#if !defined(_WIN32)
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#else
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
using namespace std::string_literals;
namespace util {
    //=================================================================================
    //=================================================================================
    filemap_t::~filemap_t() {
        if (ptr != nullptr){
            if (!unmap(true)){
                std::cerr <<"Unable to unmap: "s + path.string()<<std::endl;
            }
        }
    }
    //=================================================================================
    filemap_t::filemap_t(const std::filesystem::path &filepath):filemap_t(){
        if(!filepath.empty()){
            map(filepath);
        }
    }
    //=================================================================================
    auto filemap_t::map(const std::filesystem::path &filepath) ->void* {
        if (ptr != nullptr){
            unmap();
        }
        
        path = filepath;
        length = std::filesystem::file_size(filepath);
#if !defined(_WIN32)
        auto fd  = open(filepath.string().c_str(), O_RDONLY);
        if (fd == -1){
            throw std::runtime_error("Unable to open: "s + filepath.string());
        }
        auto temp = mmap(0, length, PROT_READ, MAP_FILE|MAP_SHARED, fd, 0);
        if (temp == MAP_FAILED){
            close(fd) ;
            
            throw std::runtime_error("Error mapping file: "s+ std::string(std::strerror(errno))+". File: "s + filepath.string());
        }
        // according to man pages, we can close the file
        close(fd) ;
        ptr = temp;
#else
        HANDLE hFile = ::CreateFileA(
                                     filepath.string().c_str(),
                                     GENERIC_READ,
                                     FILE_SHARE_READ,
                                     nullptr,
                                     OPEN_EXISTING,
                                     FILE_FLAG_SEQUENTIAL_SCAN,
                                     nullptr
                                     );
        
        if( hFile == INVALID_HANDLE_VALUE ){
            throw std::runtime_error("Unable to open: "s + filepath.string());
        }
        
        // Store the size of the file, it's used to construct
        //  the end iterator
        length = ::GetFileSize( hFile, nullptr );
        
        HANDLE hMap = ::CreateFileMapping( hFile, nullptr, PAGE_READONLY, 0, 0, nullptr );
        
        if( hMap == nullptr ){
            ::CloseHandle( hFile );
            throw std::runtime_error("Error mapping file: "s + filepath.string());
        }
        
        ptr = ::MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0 );
        
        // We hold both the file handle and the memory pointer.
        // We can close the hMap handle now because Windows holds internally
        //  a reference to it since there is a view mapped.
        ::CloseHandle( hMap );
        
        // It seems like we can close the file handle as well (because
        //  a reference is hold by the filemap object).
        ::CloseHandle( hFile );
        
#endif
        return ptr ;
    }
    //=================================================================================
    auto filemap_t::unmap(bool nothrow)->bool {
        auto status = true ;
        if (ptr != nullptr){
#if !defined(_WIN32)
            if (munmap(reinterpret_cast<void*>(ptr),length ) ==-1){
                if (!nothrow){
                    throw std::runtime_error("Unable to unmap: "s + path.string());
                }
                else {
                    status = false ;
                }
                
            }
#else
            UnmapViewOfFile(reinterpret_cast<void*>(ptr) );
#endif
            if (status){
                ptr = nullptr;
                length=0;
                path.clear();
            }
        }
        return status ;
    }
    
}
