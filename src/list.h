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
#include "game.h"

template <class T>
class List
{
	T ** items;
	int max_item,alloc_size;
public:
	List(void);
	void add(void * item);
	void add(T * item);
	T* operator[](const int index);
	int len();
};

template <class T>
List<T>::List(void)
{
	items=NULL;
	max_item=0;
	alloc_size=0;
}

template <class T>
void List<T>::add(T * item)
{	if(max_item >= alloc_size)
	{	debug("Expanding list from: "+to_str(max_item),2);
		T ** re = new T* [alloc_size?(alloc_size*2):4];
		for(int i=0;i<alloc_size; i++) re[i]=items[i];
		delete []items;
		items = re;
		alloc_size=alloc_size?(alloc_size*2):4;
		debug(" ... to: "+to_str(alloc_size),2);
	}
debug("Adding item as #"+to_str(max_item+1));
items[max_item]=item;
dappend(" - item added",1);
max_item++;
}

template <class T>
T* List<T>::operator[](const int index)
{	//assert(index>=0 && index<=max_item);
	return items[index];
}

template <class T>
int List<T>::len()
{	return max_item;
}


#endif /* LIST_H_ */
