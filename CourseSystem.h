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

	// ����key���ض�Ӧ��bucket�����ź�ƥ��Ԫ�صĵ�ַ�����û���ҵ���ӦԪ��p���ؿ�ָ��
	int Hash(unsigned k, ChainNode<T>*& p);
public:
	HashTable<T>(int divisor, int sz);
	~HashTable<T>() { delete[]bucket; }

	// ����key�ڱ����ҵ���Ӧ��Ԫ�أ�����ҵ��˷���false���Ҳ���������ҷ���true
	bool Insert(unsigned k, const T& el);
	bool Search(unsigned k); 

	// ����key�ڱ����ҵ���Ӧ��Ԫ�أ�����Ҳ�������false���ҵ���ɾ���ҷ���true
	bool Remove(unsigned k);

	// ���øú���ǰ�����Search(k)����֤��ӦԪ�ش���
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

	// ����ѧ��������Ԫ�ص����������Ҳ�������-1
	int Search(unsigned studentId);

	// ����ѧ�����������֮ǰ�Ѿ�ѡ�ÿε�ѧ���������µĿ�������ѧ������������Ĳ�����false
	bool SetSize(int sz);

	friend std::ostream& operator <<(std::ostream& out, Course &course);
};

// ���ز�����bucket������������������ϣ����������
int Divisor(int bucket);

// �Ϳγ̹�ϣ���йصĲ���
void test1();