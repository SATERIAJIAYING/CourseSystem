#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include "CourseSystem.h"

// 参数设置
// 课程哈希表的bucket容量，应小于65535(unsigned short的范围)
#define COURSE_BUCKET 2000
// 学生哈希表的bucket容量，容量不限
#define STUDENT_BUCKET 20000

// 检查内存是否泄漏
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

// 单元测试的宏定义
static int main_ret = 0, test_pass = 0, test_count = 0, test_NO = 0;
static clock_t startTime, endTime;
#define TEST(actual, expect)\
	do{\
		auto temp = (actual);\
		test_count++;\
		if((expect) == temp)\
			test_pass++;\
		else{\
			std::cerr << __FILE__ << " Line: " << __LINE__ << ": Expect:" << (expect) << " Actual:" << temp << std::endl;\
			main_ret = 1;\
		}\
	}while(0)

#define START_TEST()\
	do{\
		test_pass = test_count = 0;\
		test_NO++;\
		std::cout << "#### START_TEST " << test_NO << " ####\n测试名称：" << __FUNCTION__ << "\n\n";\
		startTime = clock();\
	}while(0)

#define END_TEST()\
	do{\
		endTime = clock();\
		std::cout << "\n#### END_TEST " << test_NO << " ####\n测试用时： " <<(float)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";\
		std::cout << test_pass << "/" << test_count << " （" << test_pass * 100.0 / test_count << "%） Pass" << "\n\n";\
	}while(0)


//////////////////////////HashTable///Node/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> HashTable<T>::HashTable(int divisor, int sz)
: divisor(divisor), currentSize(0), tableSize(sz)
{
	bucket = new ChainNode<T> *[sz];
	if (bucket == NULL) {
		std::cerr << "分配内存失败！\n";
		exit(1);
	}
	for (int i = 0; i < tableSize; i++)
	{
		bucket[i] = NULL;
	}
}

template<class T> HashTable<T>::~HashTable<T>()
{
	for (int i = 0; i < tableSize; i++)
	{
		if (bucket[i])
			for (ChainNode<T>* p = bucket[i]; p; p = bucket[i])
			{
				bucket[i] = p->link;
				delete p;
			}
	}
	delete[]bucket; 
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
		if (s == p) { // 链表中只有一个节点
			delete s;
			bucket[i] = NULL;
		}
		else { // 链表中有多个节点
			while (s->link != p) // p 是 s->link
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
	std::cout << "//////////////////////////////////////////////\n信息一览：\n\n";
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
	std::cout << "总共有" << print_count << "个信息。\n//////////////////////////////////////////////\n";
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
		std::cerr << "分配内存失败！\n";
		exit(1);
	}
	int i;
	for (i = 0; i < c.num; i++)
	{
		studentList[i] = c.studentList[i];
	}
	return *this;
}

Course::Course(std::string name, std::string place, std::string time, int maxSize)
	:name(name), place(place), time(time), num(0), maxSize(maxSize) 
{
	studentList = new unsigned[maxSize];
	if (studentList == NULL)
	{
		std::cerr << "分配内存失败！\n";
		exit(1);
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
	for (i = 0; i < num; i++) // 升序插入
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
	while (low <= high) {   //折半查找
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
		std::cerr << "内存分配错误！\n";
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
	out << "课程名称: " << course.name << '\n';
	out << "课程时间: " << course.time << '\n';
	out << "授课地点: " << course.place << '\n';
	out << "已选人数: " << course.num << " / " << course.maxSize << '\n';
	return out;
}


//////////////////////////Student//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Student& Student::operator=(const Student& s)
{
	name = s.name;
	NO = s.NO;
	num = 0;
	maxSize = 4;
	courseList = new unsigned short[maxSize];
	if (courseList == NULL)
	{
		std::cerr << "分配内存失败！\n";
		exit(1);
	}
	return *this;
}

Student::~Student()
{
	if (courseList)
	{
		delete[]courseList;
		courseList = NULL;
	}
}

bool Student::SetSize(bool broaden)
{
	if (broaden == false && maxSize - 4 < num)
		return false;
	maxSize += broaden ? 4 : (-4);
	unsigned short *temp = courseList;
	courseList = NULL;
	courseList = new unsigned short[maxSize];
	if (courseList == NULL)
	{
		std::cerr << "内存分配错误！\n";
		exit(1);
	}
	for (int i = 0; i < num; i++)
	{
		courseList[i] = temp[i];
	}
	delete[]temp;
	return true;
}

bool Student::AddCourseBucket(unsigned short courseBucketIndex)
{
	if (maxSize == num)
		SetSize(true);
	int i;
	for (i = 0; i < num; i++) // 升序插入
	{
		if (courseList[i] == courseBucketIndex)
			return false;
		if (courseList[i] > courseBucketIndex)
			break;
	}
	for (int j = num; j > i; j--)
		courseList[j] = courseList[j - 1];
	courseList[i] = courseBucketIndex;
	num++;
	return true;
}

bool Student::RemCourseBucket(unsigned short courseBucketIndex)
{
	int i = Search(courseBucketIndex);
	if (i < 0)
		return false;
	if (num < maxSize - 5)
		SetSize(false);
	for (int j = i; j < num - 1; j++)
		courseList[j] = courseList[j + 1];
	num--;
	return true;
}


int Student::Search(unsigned short courseBucketIndex)
{
	if (num == 0)
		return -1;
	int low = 0, high = num - 1, mid;
	while (low <= high) {   //折半查找
		mid = (low + high) / 2;
		if (courseBucketIndex == courseList[mid]) {
			return mid;
		}
		else if (courseBucketIndex < courseList[mid]) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return -1;
}

std::ostream& operator <<(std::ostream& out, Student& student)
{
	out << "学生名字: " << student.name << '\n';
	out << "学号: " << student.NO << '\n';
	out << "Bucket数组: " << student.num << " / " << student.maxSize << '\n';
	return out;
}


//////////////////////////CourseSystem和其他相关的类//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AnalyzedTime::AnalyzedTime()
{
	ClearTime();
}

AnalyzedTime& AnalyzedTime::operator =(const AnalyzedTime& t)
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				time[i][j][k] = t.time[i][j][k];
	return *this;
}

bool AnalyzedTime::operator ==(const AnalyzedTime& t)
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				if (time[i][j][k] != t.time[i][j][k])
					return false;
	return true;
}

AnalyzedTime& AnalyzedTime::operator +=(const AnalyzedTime& t)
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				if (t.time[i][j][k])
					time[i][j][k] = true;
	return *this;
}

AnalyzedTime& AnalyzedTime::operator -=(const AnalyzedTime& t)
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				if (t.time[i][j][k])
					time[i][j][k] = false;
	return *this;
}

bool AnalyzedTime::IsConflict(const AnalyzedTime& t)
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				if (time[i][j][k] && t.time[i][j][k])
					return true;
	return false;
}

void AnalyzedTime::ClearTime()
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 7; j++)
			for (int k = 0; k < 13; k++)
				time[i][j][k] = false;
}

bool AnalyzedTime::SetTime(const std::string strTime, bool add)
{
	std::string temp;
	std::stringstream strStream(strTime);
	int weekBegin = 0, day = 0, weekEnd, classBegin, classEnd;
	char sep;
	while (strStream >> temp)
	{
		if (temp.size() > 7 && temp.size() <= 12 && temp.substr(0, 7) == "Classes" 
				&& weekBegin > 0 && day > 0) // temp中保存的是第几节课的信息，且之前已经得知了周次和星期几
		{
			temp = temp.substr(7, temp.length());
			std::stringstream subStream(temp);
			if (subStream >> classBegin)
			{
				if (classBegin < 1 || classBegin > 13)
					return false;
				if (subStream >> sep)
				{
					if (sep == '-')
					{
						if (subStream >> classEnd)  //  "Classess<int1>-<int2>"
						{
							if (!(1 <= classBegin && classBegin <= classEnd && classEnd <= 16))
								return false; // <int2> 的合法性检查，条件不为true说明合法
						}
						else  // "Classess<int>-"  合法输入：第<int>到13节课
							classEnd = 13;
					}
					else // "Classess<int>!" 非法输入
						return false;
				}
				else   // "Classess<int>"  合法输入：第<int>节课
					classEnd = classBegin;

				// 在bool[][][]中修改课时
				for (int week = weekBegin; week <= weekEnd; week++)
					for (int clasS = classBegin; clasS <= classEnd; clasS++)
						time[week - 1][day - 1][clasS - 1] = add;
			}
			else
				return false;
		}
		else if (temp.size() > 5 && temp.size() <= 10 && temp.substr(0, 5) == "Weeks")
			// temp中保存的是第几周的信息
		{
			temp = temp.substr(5, temp.length());
			std::stringstream subStream(temp);
			if (subStream >> weekBegin)
			{
				if (weekBegin < 1 || weekBegin > 16)
					return false;
				if (subStream >> sep)
				{
					if (sep == '-')
					{
						if (subStream >> weekEnd)  //  "Week<int1>-<int2>"
						{
							if (!(1 <= weekBegin && weekBegin <= weekEnd && weekEnd <= 16))
								return false; // <int2> 的合法性检查，条件不为true说明合法
						}
						else  // "Week<int>-"  合法输入：第<int>到16周
							weekEnd = 16;
					}
					else // "Week<int>!" 非法输入
						return false;
				}
				else   // "Weeks<int>"  合法输入：第<int>周
					weekEnd = weekBegin;
			}
			else
				return false;
		}
		else if (temp.size() == 3) // temp中保存的是星期几的信息
		{
			if (temp == "Mon")
				day = 1;
			else if (temp == "Tue")
				day = 2;
			else if (temp == "Wed")
				day = 3;
			else if (temp == "Thu")
				day = 4;
			else if (temp == "Fri")
				day = 5;
			else if (temp == "Sat")
				day = 6;
			else if (temp == "Sun")
				day = 7;
			else
				return false;
		}
		else
			return false;
	}
	return true;
}

//////////////////////////Test///Other/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Divisor(int bucket)
{
	for (int i = bucket; i >= 2; i--)
	{
		bool isPrime = true;
		for (int j = 2; j <= sqrt(i); j++) 
		{
			if (i % j == 0) 
			{
				isPrime = false;
				break;
			}
		}
		if (isPrime)
			return i;
	}
}


void test1()
{
	START_TEST();
	// 构造保存课程信息的哈希表
	HashTable<Course> clist(Divisor(COURSE_BUCKET), COURSE_BUCKET);
	// 添加课程信息
	// Course构造函数： Course(std::string name, std::string place, std::string time, int maxSize);
	TEST(clist.Insert(15564546, Course("人体解剖", "中山院503", "1-16周 周一 1-3节", 35)), true);
	TEST(clist.Insert(13124121, Course("化学信息", "中山院507", "1-16周 周二 1-2节", 30)), true);
	TEST(clist.Insert(36323543, Course("数据分析", "东南院104", "1-16周 周二 3-5节", 25)), true);
	TEST(clist.Insert(11111509, Course("Python", "中山院403", "1-16周 周二 6-7节", 30)), true);
	TEST(clist.Insert(11111111, Course("数据结构", "东南院204", "1-16周 周一 4-5节", 100)), true);
	TEST(clist.Insert(11111310, Course("形势与政策", "礼东102", "1-16周 周二 11-12节", 200)), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	// 搜索某个课程，查看是否存在
	TEST(clist.Search(11111111), true);
	for (int i = 0; i < 80; i++)
	{  // 为某个课程添加学生
		TEST(clist.Find(11111111).data.AddStudent(i + 1000000), true);
	}
	// 查找某个课程并显示信息
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	for (int i = 60; i < 80; i++)
	{ // 为某个课程删除学生
		TEST(clist.Find(11111111).data.RemStudent(i + 1000000), true);
	}
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	// 查找某个课程并显示信息
	TEST(clist.Search(1111111), false);
	TEST(clist.Search(11111111), true);
	std::cout << clist.Find(11111111).data << std::endl;
	// 移除某个课程
	TEST(clist.Remove(11111509), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	// 修改课程的学生容量
	TEST(clist.Find(11111111).data.SetSize(40), false);
	std::cout << clist.Find(11111111).data << std::endl;
	TEST(clist.Find(11111111).data.SetSize(90), true);
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();
}

void test2()
{
	START_TEST();
	// 构造保存学生信息的哈希表
	HashTable<Student> clist(Divisor(STUDENT_BUCKET), STUDENT_BUCKET);
	// 添加学生信息
	// Student构造函数： Student(std::string name, std::string NO);
	TEST(clist.Insert(15564546, Student("王朝兴", "11320111")), true);
	TEST(clist.Insert(13124121, Student("张三", "11320154")), true);
	TEST(clist.Insert(36323543, Student("李四", "TJ210104")), true);
	TEST(clist.Insert(11111509, Student("王五", "D1120524")), true);
	TEST(clist.Insert(11111111, Student("赵六", "S1120213")), true);
	TEST(clist.Insert(11111310, Student("古尔丹", "11119123")), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	// 搜索某个学生，查看是否存在
	TEST(clist.Search(11111111), true);
	for (int i = 0; i < 80; i++)
	{  // 为某个学生添加课程的bucket索引
		TEST(clist.Find(11111111).data.AddCourseBucket(i + 100), true);
	}
	// 查找某个学生并显示信息
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	for (int i = 60; i < 80; i++)
	{ // 为某个学生删除课程的bucket索引
		TEST(clist.Find(11111111).data.RemCourseBucket(i + 100), true);
	}
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	// 查找某个学生并显示信息
	TEST(clist.Search(1111111), false);
	TEST(clist.Search(11111111), true);
	std::cout << clist.Find(11111111).data << std::endl;
	// 移除某个学生
	TEST(clist.Remove(11111509), true);
	clist.PrintHashTable();
	END_TEST();
}

void test3()
{
	START_TEST();
	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	AnalyzedTime t1, t2;
	// 测试是否能判断字符串是否合法
	TEST(t1.SetTime("Weeks1-16 Mon Classes1-3"), true);
	TEST(t1.SetTime("Weeks1-16 Mon Classes1-3 Sun Classes1-5 Tue Classes3"), true);
	TEST(t1.SetTime("Weeks1-16 Mon Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13"), true);
	TEST(t1.SetTime("Weeks1-16 Mon Classes1-3"), true);
	TEST(t1.SetTime("Classes1-3 Weeks1-16 Mon"), false);
	TEST(t1.SetTime("Weeks16-1 Mon Classes1-3 "), false);
	TEST(t1.SetTime(" Weeks1-16 Mon Classes3-"), true);
	TEST(t1.SetTime("Mon Weeks1-16 Weeks2-8  Classes1-3"), true);
	TEST(t1.SetTime(" Weeks1-16 Weeks2-8  Classes1-3 Mon"), false);
	t1.ClearTime();
	// 测试时间解析功能是否正常
	TEST(t1.SetTime("Weeks1-16 Mon Classes4-5"), true);
	TEST(t1.SetTime("Weeks1-16 Wed Classes2-3"), true);
	TEST(t2.SetTime("Weeks1-16 Mon Classes4-5 Wed Classes2-3"), true);
	TEST(t1 == t2, true);
	AnalyzedTime t3, t4;
	TEST(t3.SetTime("Weeks2-3 Mon Classes6-8"), true);
	TEST(t3.SetTime("Weeks4-16 Fri Classes1-2"), true);
	TEST(t3.SetTime("Weeks1-16 Tue Classes3-5"), true);
	TEST(t4.SetTime("Weeks1-16 Tue Classes3-5 Weeks4-16 Fri Classes1-2 Weeks2-3 Mon Classes6-8"), true);
	TEST(t3 == t4, true);
	TEST(t3.IsConflict(t4), true);
	TEST(t1.IsConflict(t2), true);
	TEST(t1.IsConflict(t3), false);
	END_TEST();
}

int main()
{
	//test1();
	//test2();
	test3();
	_CrtDumpMemoryLeaks();  // 检测内存是否泄漏
	return 0;
}