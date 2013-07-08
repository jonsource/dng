/*
 * list.h
 *
 *  Created on: 10.3.2013
 *      Author: tzeentch
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include <cassert>
#include <string>
//#include "game_lib.h"

template <class T>
class List
{
    private:
        T ** items;
        int length,alloc_size;
    public:
        List(void);
        int add(T * item);
        T* pop();
        void clear_all();
        int remove(const int i);
        int remove(const T * item);
        T* operator[](const int index);
        int len();
        std::string serialize();
        std::string save_string();
};

template <class T>
std::string List<T>::serialize()
{   std::string s="";
    for(int i=0; i<this->len(); i++)
    {
        s+=this->items[i]->serialize()+"\n";
    }
    return s;
}

template <class T>
std::string List<T>::save_string()
{   std::string s="";
    for(int i=0; i<this->len(); i++)
    {
        s+=this->items[i]->save_string()+"\n";
    }
    return s;
}

template <class T>
List<T>::List(void)
{
	items=NULL;
	length=0;
	alloc_size=0;
}

template <class T>
int List<T>::add(T * item)
{	if(length >= alloc_size)
	{	//debug("Expanding list from: "+to_str(max_item),2);
		T ** re = new T* [alloc_size ? (alloc_size*2) : 4];
		for(int i=0;i<alloc_size; i++) re[i]=items[i];
		delete []items;
		items = re;
		alloc_size = alloc_size ? (alloc_size*2) : 4;
		//debug(" ... to: "+to_str(alloc_size),2);
	}
//debug("Adding item as #"+to_str(max_item+1),2);
    items[length]=item;
//dappend(" - item added",1);
    return ++length;
}

template <class T>
T* List<T>::pop()
{	T* ret;
    if(length>0)
    {   ret = items[length-1];
        items[length-1]=NULL;
        length--;
    }
    else ret = NULL;
    return ret;
}

template <class T>
T* List<T>::operator[](const int index)
{	//assert(index>=0 && index<=max_item);
	return items[index];
}

template <class T>
int List<T>::len()
{	return length;
}

template <class T>
void List<T>::clear_all()
{	//debug("Clearing list :"+to_str(max_item)+" items!",2);
    for(int i=0; i<this->len(); i++)
    {
        delete this->items[i];
    }
    delete []items;
    items=NULL;
	length=0;
	alloc_size=0;
}

template <class T>
int List<T>::remove(int i)
{	if(i<0 || i >= this->length) return -1;
    if(this->length > 1 && i < length-1)
    {   this->items[i]=this->items[length-1];
    }
    this->items[length-1]=NULL;
    this->length--;
    return length;
}

template <class T>
int List<T>::remove(const T * item)
{	if(item == NULL) return -1;
    int i;
    for(i=0; i<length; i++)
    {   if(items[i]==item) break;
    }
    if(i>=length) return -1;
    return remove(i);
}

typedef List<std::string> STR_LIST;

#endif /* LIST_H_ */
