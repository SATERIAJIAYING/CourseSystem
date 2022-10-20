#include <iostream>
#include <string>
#include "CourseSystem.h"
#include <ctime>

// ��������
#define COURSE_BUCKET 2000

// ����ڴ��Ƿ�й©
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

// ���Եĺ궨��
static int main_ret = 0, test_pass = 0, test_count = 0, test_NO = 0;
static clock_t startTime, endTime;
#define TEST(actual, expect)\
	do{\
		auto temp = (actual);\
		test_count++;\
		if((expect) == temp)\
			test_pass++;\
		else{\
			std::cout << __FILE__ << " Line: " << __LINE__ << ": Expect:" << (expect) << " Actual:" << temp << std::endl;\
			main_ret = 1;\
		}\
	}while(0)

#define START_TEST()\
	do{\
		test_pass = test_count = 0;\
		test_NO++;\
		std::cout << "#### START_TEST " << test_NO << " ####\n�������ƣ�" << __FUNCTION__ << "\n\n";\
		startTime = clock();\
	}while(0)

#define END_TEST()\
	do{\
		endTime = clock();\
		std::cout << "\n#### END_TEST " << test_NO << " ####\n������ʱ�� " <<(float)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";\
		std::cout << test_pass << "/" << test_count << " ��" << test_pass * 100.0 / test_count << "%�� Pass" << "\n\n";\
	}while(0)


//////////////////////////HashTable///Node/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> HashTable<T>::HashTable(int divisor, int sz)
: divisor(divisor), currentSize(0), tableSize(sz)
{
	bucket = new ChainNode<T> *[sz];
	if (bucket == NULL) {
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
		exit(1);
	}
	for (int i = 0; i < tableSize; i++)
	{
		bucket[i] = NULL;
	}
}

template<class T> int HashTable<T>::Hash(unsigned k, ChainNode<T> *&p) 
{
	unsigned i = k % divisor;
	p = bucket[i];
	while (p != NULL && p->key != k) {
		p = p->link;
	}
	return i;
}

template<class T> bool HashTable<T>::Insert(unsigned k, const T& el)
{
	ChainNode<T>* p, * s;
	int i = Hash(k, p);
	if (p != NULL) {
		return false;
	}
	else {
		s = new ChainNode<T>;
		s->data = el;
		s->key = k;
		s->link = bucket[i];
		bucket[i] = s;
		currentSize++;
		return true;
	}
}

template<class T> bool HashTable<T>::Search(unsigned k) 
{
	ChainNode<T> *p;
	Hash(k, p);
	return p != NULL;
}

template<class T> bool HashTable<T>::Remove(unsigned k) 
{
	ChainNode<T>* p, * s;
	int i = Hash(k, p);
	if (p == NULL) {
		return false;
	}
	else {
		s = bucket[i];
		if (s == p) { // ������ֻ��һ���ڵ�
			delete s;
			bucket[i] = NULL;
		}
		else { // �������ж���ڵ�
			while (s->link != p) // p �� s->link
				s = s->link;
			s->link = p->link;
			delete p;
		}
		currentSize--;
		return true;
	}
}

template<class T> ChainNode<T>& HashTable<T>::Find(unsigned k) 
{
	ChainNode<T>* p;
	Hash(k, p);
	if (p == NULL)
		exit(1);
	return *p;
}

template<class T> void HashTable<T>::PrintHashTable()
{
	int print_count = 0;
	std::cout << "//////////////////////////////////////////////\n�γ���Ϣһ����\n";
	for (int i = 0; i < tableSize; i++)
	{
		if (bucket[i])
		{
			for (ChainNode<T>* p = bucket[i]; p; p = p->link)
			{ 
				std::cout << "Key:\t" << p->key << '\n' << p->data << "\n\n";	
				print_count++;
			}
		}
	}
	std::cout << "�ܹ���" << print_count << "���γ���Ϣ��\n//////////////////////////////////////////////\n";
}


//////////////////////////Course///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Course& Course::operator=(const Course& c)
{
	name = c.name; 
	place = c.place;
	time = c.time;
	num = c.num;
	maxSize = c.maxSize;
	studentList = new unsigned[c.maxSize];
	if (studentList == NULL)
	{
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
		exit(1);
	}
	int i;
	for (i = 0; i < c.num; i++)
	{
		studentList[i] = c.studentList[i];
	}
	for (i; i < c.maxSize; i++)
	{
		studentList[i] = 0;
	}
	return *this;
}

Course::Course(std::string name, std::string place, std::string time, int maxSize)
	:name(name), place(place), time(time), num(0), maxSize(maxSize) 
{
	studentList = new unsigned[maxSize];
	if (studentList == NULL)
	{
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
		exit(1);
	}
	for (int i = 0; i < maxSize; i++)
	{
		studentList[i] = 0;
	}
}

Course::~Course()
{
	if (studentList)
	{
		delete[]studentList;
		studentList = NULL;
	}
}

bool Course::AddStudent(unsigned studentId)
{
	if (maxSize == num)
		return false;
	int i;
	for (i = 0; i < num; i++) // �������
	{
		if (studentList[i] == studentId)
			return false;
		if (studentList[i] > studentId)
			break;
	}
	for (int j = num; j > i; j--)
		studentList[j] = studentList[j - 1];
	studentList[i] = studentId;
	num++;
	return true;
}

bool Course::RemStudent(unsigned studentId)
{
	int i = Search(studentId);
	if (i < 0)
		return false;
	for (int j = i; j < num - 1; j++)
		studentList[j] = studentList[j + 1];
	num--;
	return true;
}

int Course::Search(unsigned studentId)
{ 
	if (num == 0)
		return -1;
	int low = 0, high = num - 1, mid;
	while (low <= high) {   //�۰����
		mid = (low + high) / 2;
		if (studentId == studentList[mid]) {
			return mid;
		}
		else if (studentId < studentList[mid]) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return -1;
}

bool Course::SetSize(int sz)
{
	if (sz < num || sz == maxSize)
	{
		return false;
	}
	unsigned* temp = studentList;
	studentList = NULL;
	studentList = new unsigned[sz];
	if (studentList == NULL)
	{
		std::cerr << "�ڴ�������\n";
		exit(1);
	}
	for (int i = 0; i < num; i++)
	{
		studentList[i] = temp[i];
	}
	maxSize = sz;
	delete[]temp;
	return true;
}

std::ostream& operator <<(std::ostream& out, Course &course)
{
	out << "�γ�����: " << course.name << '\n';
	out << "�γ�ʱ��: " << course.time << '\n';
	out << "�ڿεص�: " << course.place << '\n';
	out << "��ѡ����: " << course.num << " / " << course.maxSize << '\n';
	return out;
}


int Divisor(int bucket)
{
	for (int i = bucket; i >= 2; i--)
	{
		bool isPrime = true;
		for (int j = 2; j <= sqrt(i); j++) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}
		if (isPrime)
			return i;
	}
}


//////////////////////////Test///Main//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void test1()
{
	START_TEST();
	// ���챣��γ���Ϣ�Ĺ�ϣ��
	HashTable<Course> clist(Divisor(COURSE_BUCKET), COURSE_BUCKET);
	// ��ӿγ���Ϣ
	// Course���캯���� Course(std::string name, std::string place, std::string time, int maxSize);
	TEST(clist.Insert(15564546, Course("�������", "��ɽԺ503", "1-16�� ��һ 1-3��", 35)), true);
	TEST(clist.Insert(13124121, Course("��ѧ��Ϣ", "��ɽԺ507", "1-16�� �ܶ� 1-2��", 30)), true);
	TEST(clist.Insert(36323543, Course("���ݷ���", "����Ժ104", "1-16�� �ܶ� 3-5��", 25)), true);
	TEST(clist.Insert(11111509, Course("Python", "��ɽԺ403", "1-16�� �ܶ� 6-7��", 30)), true);
	TEST(clist.Insert(11111111, Course("���ݽṹ", "����Ժ204", "1-16�� ��һ 4-5��", 100)), true);
	TEST(clist.Insert(11111310, Course("����������", "��102", "1-16�� �ܶ� 11-12��", 200)), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	
	TEST(clist.Search(11111111), true);
	for (int i = 0; i < 80; i++)
	{  // Ϊĳ���γ����ѧ��
		TEST(clist.Find(11111111).data.AddStudent(i + 1000000), true);
	}
	// ����ĳ���γ̲���ʾ��Ϣ
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	for (int i = 60; i < 80; i++)
	{ // Ϊĳ���γ�ɾ��ѧ��
		TEST(clist.Find(11111111).data.RemStudent(i + 1000000), true);
	}
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	// ����ĳ���γ̲���ʾ��Ϣ
	TEST(clist.Search(1111111), false);
	TEST(clist.Search(11111111), true);
	std::cout << clist.Find(11111111).data << std::endl;
	// �Ƴ�ĳ���γ�
	TEST(clist.Remove(11111509), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	// �޸Ŀγ̵�ѧ������
	TEST(clist.Find(11111111).data.SetSize(40), false);
	std::cout << clist.Find(11111111).data << std::endl;
	TEST(clist.Find(11111111).data.SetSize(90), true);
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();
}


int main()
{
	test1();
	_CrtDumpMemoryLeaks();
	return 0;
}