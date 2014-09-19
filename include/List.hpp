/*
 * List.h
 *
 *  Created on: 24.06.2013
 *      Author: user
 */

#ifndef LIST_H_
#define LIST_H_

#include <string>

template <class T>
class Cnct {
	Cnct<T> *prev;
	Cnct<T> *next;
	T       *data;
public:
	Cnct(T* item = 0) {
		prev = 0;
		next = 0;
		data = item;
	}
	~Cnct() { delete data; };
	Cnct *getNext() const { return next; };
	Cnct *getPrev() const { return prev; };
	T    *getData() const { return data; };
	void  setNext(Cnct *x) { next = x; };
	void  setPrev(Cnct *x) { prev = x; };
	void  setData(T    *x) { data = new T; data = x; };
};


template <class T>
class List {
	Cnct<T> *first;
	Cnct<T> *last;
	Cnct<T> *curr;
	int 	 elements;
	bool     wrapped;
public:
	List() {
		first    = 0;
		last     = 0;
		curr     = 0;
		elements = 0;
		wrapped  = false;
	}
	void forAll(void (*f)(T* data));
	void insert		 (T *item);
	void insertHead  (T *item);
	void insertTail  (T *item);
	void insertBefore(T *item);
	void insertBehind(T *item);
	void remove();
	bool iswrapped() { return wrapped; };
	int  getLen() { return elements; };
	T   *find(std::string const& op);
	T   *find(int op);
	T   *getFirst();
	T   *getLast();
	T   *getCurr();
	T   *operator ++(int);
	T   *operator --(int);
	virtual ~List();
};

template <class T>
T *List<T>::find(std::string const& op) {
	if (!getFirst()) return NULL;
	do {
		if (*curr->getData() == op) return curr->getData();
		(*this)++;
	} while (!iswrapped());
	return NULL;
}

template <class T>
T *List<T>::find(int op) {
	if (!getFirst()) return NULL;
	do {
		if (*curr->getData() == op) return curr->getData();
		(*this)++;
	} while (!iswrapped());
	return NULL;
}

template <class T>
void List<T>::insert(T *item) {
	if (!getFirst()) insertBehind(item);
	else {
		while (*curr->getData() < *item && curr != last) (*this)++;
		if (curr == last && *item > *curr->getData()) insertBehind(item);
		else insertBefore(item);
	}
}

template <class T>
void List<T>::insertHead(T *item) {
	getFirst();
	insertBefore(item);
}

template <class T>
void List<T>::insertTail(T *item) {
	getLast();
	insertBehind(item);
}

template <class T>
void List<T>::insertBefore(T *item) {
	elements++;
	if (!curr) {
		curr  = new Cnct<T>(item);
		first = curr;
		last  = curr;
	}
	else if (curr->getPrev()) {
		(*this)--;
		elements--;
		insertBehind(item);
	} else {
		curr->setPrev(new Cnct<T>(item));
		curr->getPrev()->setNext(curr);
		(*this)--;
		first = curr;
	}
}

template <class T>
void List<T>::insertBehind(T *item) {
	elements++;
	if (!curr) {
		curr  = new Cnct<T>(item);
		first = curr;
		last  = curr;
	}
	else if (curr->getNext()) {
		Cnct<T>* tmp = new Cnct<T>(item);
		tmp->setNext(curr->getNext());
		tmp->setPrev(curr);
		curr->getNext()->setPrev(tmp);
		curr->setNext(tmp);
		(*this)++;
	} else {
		curr->setNext(new Cnct<T>(item));
		curr->getNext()->setPrev(curr);
		(*this)++;
		last = curr;
	}
}

template <class T>
void List<T>::forAll(void (*f)(T* data)) {
	getFirst();
	while(curr && !wrapped) {
		f(curr->getData());
		(*this)++;
	}
}

template <class T>
void List<T>::remove() {
	if (!curr) return;
	Cnct<T>* tmp = curr;
	if (curr->getNext()) {
		curr->getNext()->setPrev(curr->getPrev());
	} else {
		last = curr->getPrev();
		tmp = last;
	}
	if (curr->getPrev()) {
		curr->getPrev()->setNext(curr->getNext());
		tmp = curr->getPrev();
	} else {
		first = curr->getNext();
		tmp = first;
	}
	delete(curr);
	curr = tmp;
	elements--;
}

template <class T>
T* List<T>::getCurr() {
	wrapped = false;
	return curr ? curr->getData() : NULL;
}

template <class T>
T* List<T>::getFirst() {
	curr = first;
	return getCurr();
}

template <class T>
T* List<T>::getLast() {
	curr = last;
	return getCurr();
}

template <class T>
T* List<T>::operator ++(int x) {
	if (!curr) { wrapped = true; return NULL; };
	if (curr->getNext()) {
		curr = curr->getNext();
		wrapped = false;
	} else {
		curr = first;
		wrapped = true;
	}
	return curr->getData();
}

template <class T>
T* List<T>::operator --(int x) {
	if (curr->getPrev()) {
		curr = curr->getPrev();
		wrapped = false;
	} else {
		curr = last;
		wrapped = true;
	}
	return curr->getData();
}

template <class T>
List<T>::~List() {
	while (curr) remove();
}

#endif /* LIST_H_ */
