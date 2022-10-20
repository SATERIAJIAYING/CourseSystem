#pragma once

template<class T> struct ChainNode {
	unsigned key;
	ChainNode<T>* link;
	T data;	
	ChainNode<T>() : key(0), link(NULL) {}
};

template<class T> class HashTable {
private:
	int divisor, currentSize, tableSize;
	ChainNode<T>** bucket;
	int Hash(unsigned k, ChainNode<T>*& p);
public:
	HashTable<T>(int divisor, int sz);
	~HashTable<T>() { delete[]bucket; }
	//bool Insert(unsigned k, ChainNode<T>& el);
	bool Insert(unsigned k, const T& el);
	bool Search(unsigned k); 
	bool Remove(unsigned k);
	ChainNode<T>& Find(unsigned k);
	void PrintHashTable();
};

class Course {
private:
	unsigned* studentList;
	int num, maxSize;
	std::string name, place, time;
public:
	Course() : studentList(NULL), num(0), maxSize(0), name(""), place(""), time("") {}
	Course& operator=(const Course& c);
	Course(std::string name, std::string place, std::string time, int maxSize);
	~Course();
	bool AddStudent(unsigned studentId);
	bool RemStudent(unsigned studentId);
	int Search(unsigned studentId);
	//bool SetSize(int sz);  TODO: 测试完后要添加修改课容量的功能

	friend std::ostream& operator <<(std::ostream& out, Course &course);
};

int Divisor(int bucket);
void test1();