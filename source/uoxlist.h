//------------------------------------------------------------------------
//  uoxlist.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
#ifndef __LIST_H
#define __LIST_H
#define NDEBUG
#include <assert.h>
template <class T> struct Node 
{
    T *data;
    Node *next;
};

// A linked list. Internal iterator only!
// Usage is List<Object, 1|0> - the list will store Object*
// If own is 1, the list will call delete on all objects when destroyed

template <class T, char own>
class List
{
    
public:
    
    List() : first(NULL), next_node(NULL) {}
    ~List();

    void flush();
    void insert (T* x);
    void remove (T* x);
    inline T* rewind();
    inline T* next();
    
    T* operator[] (int no);
    
    int count() const;
    
private:
    Node <T> *first;
    Node <T> *next_node;
};

template <class T, char own> List<T,own>::~List() {
    flush(); // flush() is not virtual so its safe to call
}

template <class T, char own> void List<T,own>::flush() {
    Node <T> *n_next;
    for (Node<T> *n = first; n; n = n_next) {
        n_next = n->next;
        if (own)
            delete n->data;
        delete n;
    }
	first = next_node = NULL;
}

template <class T, char own> int List<T,own>::count() const {
    int i = 0;
    for (Node<T> *n = first; n; n = n->next)
        i++;
    
    return i;
}

template<class T, char own> T* List<T,own>::operator[] (int no) {
    int i = 0;
    
    for (Node<T> *n = first; n; n = n->next, i++)
        if (i == no)
            return n->data;
    
    return NULL;
}

template<class T, char own> T* List<T,own>::next() {
    Node<T> *p = next_node;
    
    if (p)
        next_node = next_node->next;
    
    return p ? p->data : 0;
}

template<class T, char own> void List<T,own>::remove(T *x) {
    Node<T> *p;

    assert(first != NULL);
    
    if (first->data == x)
    {
        p = first;
        first = first->next;
    }
    else
    {
        Node<T> *prev;
        for (prev = first; prev && prev->next->data != x; prev = prev->next)
            ;
        
        assert(prev);
        assert(prev->next->data == x);
        
        p = prev->next;
        prev->next = prev->next->next;
    }
    
    if (next_node && next_node->data == x)
        next_node = p->next;
    
    delete p;
}

template<class T, char own> T* List<T,own>::rewind() {
    
    if (first)
        next_node = first->next;
    else
        next_node = 0;
    
    return first ? first->data : 0;
}

template <class T, char own> void List<T,own>::insert (T* x) {
    Node<T> *node = new Node<T>;
    Node<T> *prev;
    
    node->data = x;
    
    for (prev = first; prev && prev->next;prev = prev->next)	// this line craps out on a reload of npc.scp, Access Violation
        ;
    
    if (!prev)
    {
        node->next = first;
        first = node;
    }
    else
    {
        prev->next = node;
        node->next = 0;
    }
    
}
#endif

