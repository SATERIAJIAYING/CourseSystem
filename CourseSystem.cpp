#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>
#include "CourseSystem.h"

#define CLS (system("cls"))
#define PAUSE (system("pause"))
#define REM_ENTER (std::cin.ignore(1000, '\n')) 

// 参数设置
// 课程哈希表的bucket容量，应小于65535(unsigned short的范围)，对实际存储的课程数量没有上限
#define COURSE_BUCKET 15000
// 学生哈希表的bucket容量，容量不限
#define STUDENT_BUCKET 800000
// 保存和读取的文件的名字
#define COURSE_FILE_NAME ("COURSE_DATA.csv")
#define STUDENT_FILE_NAME ("STUDENT_DATA.csv")
#define READ_LOG_NAME ("Log.txt")
#define PRINT_FILE_NAME ("Print.txt")

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

template<class T> void HashTable<T>::PrintHashTable(int n, std::ostream& out)
{
	int print_count = 0;
	out << "//////////////////////////////////////////////\n共有" << currentSize << "个数据。\n";
	if (n)
		out << "第" << n << "页结果：\n\n";
	else
		out << "信息一览：\n\n";
	for (int i = 0; i < tableSize; i++)
	{
		if (bucket[i])
		{
			for (ChainNode<T>* p = bucket[i]; p; p = p->link)
			{ 
				print_count++;
				if ((!n) || (n && print_count > (n - 1) * 5 && print_count <= n * 5))
					out << "Key:\t" << p->key << '\n' << p->data << "\n\n";
				if (n && print_count > n * 5)
					break;
			}
		}
		if (n && print_count > n * 5)
			break;
	}
	out << "\n//////////////////////////////////////////////" << std::endl;
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
	out << "学生名字： " << student.name << "  ";
	out << "学号： " << student.NO << '\n';
	//out << "Bucket数组: " << student.num << " / " << student.maxSize << '\n';
	return out;
}


//////////////////////////AnalyzedTime类///////////////////////////////////////////////////////////////////////////////
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

//////////////////////////CourseSystem/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CourseSystem::RemStudentFromCourses(unsigned studentKey)
{
	const Student& studentToDel = studentList.Find(studentKey).data;
	unsigned short courseBucketIdx;
	for (int i = 0; i < studentToDel.num; i++)
	{
		courseBucketIdx = studentToDel.courseList[i]; //学生存储的选课信息不是课程的Key，而是课程的Bucket索引
		for (ChainNode<Course>* p = courseList.bucket[courseBucketIdx]; p; p = p->link)
		{
			p->data.RemStudent(studentKey);
		}
	}
}

void CourseSystem::RemCourseFromStudents(unsigned courseKey)
{
	ChainNode<Course>* pCourseToDel;
	unsigned short courseBucketIdx = courseList.Hash(courseKey, pCourseToDel);
	for (int i = 0; i < pCourseToDel->data.num; i++)
	{
		// 先找在课程哈希表中的相同bucket是否有待处理的学生选的课
		bool otherCourseInSameBucket = false;
		for (ChainNode<Course>* p = courseList.bucket[courseBucketIdx]; p; p = p->link)
		{
			if (pCourseToDel == p)
				continue;
			if (p->data.Search(pCourseToDel->data.studentList[i]) != -1)
			{
				otherCourseInSameBucket = true;
				break;
			}
		}
		// 如果没有的话就删除该bucket的索引
		if (otherCourseInSameBucket == false)
			studentList.Find(pCourseToDel->data.studentList[i]).data.RemCourseBucket(courseBucketIdx);
	}
}

AnalyzedTime CourseSystem::TotalTime(unsigned studentKey)
{
	const Student& studentToSearch = studentList.Find(studentKey).data;
	AnalyzedTime totalTime;
	unsigned short courseBucketIdx;
	for (int i = 0; i < studentToSearch.num; i++)
	{
		courseBucketIdx = studentToSearch.courseList[i];
		for (ChainNode<Course>* p = courseList.bucket[courseBucketIdx]; p; p = p->link)
		{
			if (p->data.Search(studentKey) != -1)
				totalTime.SetTime(p->data.time);
		}
	}
	return totalTime;
}

bool CourseSystem::PickCourseInTable(unsigned studentKey, unsigned courseKey)
{
	if (!courseList.Find(courseKey).data.AddStudent(studentKey))
		return false;
	ChainNode<Course>* temp;
	unsigned short courseBucketIdx = courseList.Hash(courseKey, temp);
	studentList.Find(studentKey).data.AddCourseBucket(courseBucketIdx);
	return true;
}

bool CourseSystem::ExitCourseInTable(unsigned studentKey, unsigned courseKey)
{
	if (!courseList.Find(courseKey).data.RemStudent(studentKey))
		return false;
	ChainNode<Course>* courseNode;
	unsigned short courseBucketIdx = courseList.Hash(courseKey, courseNode);
	// 先找在课程哈希表中的相同bucket是否有待处理的学生选的课
	bool otherCourseInSameBucket = false;
	for (ChainNode<Course>* p = courseList.bucket[courseBucketIdx]; p; p = p->link)
	{
		if (courseNode == p)
			continue;
		if (p->data.Search(studentKey) != -1)
		{
			otherCourseInSameBucket = true;
			break;
		}
	}
	// 如果没有的话就删除该bucket的索引
	if (otherCourseInSameBucket == false)
		studentList.Find(studentKey).data.RemCourseBucket(courseBucketIdx);
	return true;
}

void CourseSystem::PrintPickedCourse(unsigned studentKey)
{
	Student& student = studentList.Find(studentKey).data;
	unsigned short courseBucketIdx;
	int count = 0;
	for (int i = 0; i < student.num; i++)
	{
		courseBucketIdx = student.courseList[i];
		for (ChainNode<Course>* p = courseList.bucket[courseBucketIdx]; p; p = p->link)
		{
			if (p->data.Search(studentKey) != -1)
			{
				std::cout << "\nKey： " << p->key << '\n';
				std::cout << p->data;
				count++;
			}
		}
	}
	std::cout << "\n一共有" << count << "个课程。" << std::endl;
}

// .csv文件保存，无列名行
// 课程数据：key, name, place, time, maxSize, studentKey *
// 学生数据：key, name, NO

void CourseSystem::ReadFromFile()
{
	clock_t startRead = clock(), endRead;
	std::ifstream inFileStudent, inFileCourse;
	std::ofstream outLog;
	outLog.open(READ_LOG_NAME, std::ios::out | std::ios::trunc);
	// 读取学生数据
	inFileStudent.open(STUDENT_FILE_NAME, std::ios::in);
	if (!inFileStudent.is_open())
	{ 
		std::cout << "学生数据文件" << STUDENT_FILE_NAME << "读取失败！" << std::endl;
		outLog << "学生数据文件" << STUDENT_FILE_NAME << "读取失败！" << std::endl;
	}
	else
	{
		unsigned key;
		std::string name, NO, line;
		while (!inFileStudent.eof())
		{
			getline(inFileStudent, line);
			std::stringstream lineStream(line);
			if (lineStream >> key)
			{
				lineStream.get();
				if (!getline(lineStream, name, ','))
				{
					std::cout << "Key：" << key << " 读取学生名字失败！\n";
					outLog << "Key：" << key << " 读取学生名字失败！\n";
					continue;
				}
				else if (!getline(lineStream, NO))
				{
					std::cout << "Key：" << key << " 读取学生学号失败！\n";
					outLog << "Key：" << key << " 读取学生学号失败！\n";
					continue;
				}
				else
				{
					if (!studentList.Insert(key, Student(name, NO)))
					{
						std::cout << "Key：" << key << " 添加学生失败，已有相同key的学生信息！\n";
						outLog << "Key：" << key << " 添加学生失败，已有相同key的学生信息！\n";
						continue;
					}
				}
			}
			else
			{
				continue;
			}
		}
	}
	inFileStudent.close();
	// 读取课程数据
	inFileCourse.open(COURSE_FILE_NAME, std::ios::in);
	if (!inFileCourse.is_open())
	{
		std::cout << "课程数据文件" << COURSE_FILE_NAME << "读取失败！" << std::endl;
		outLog << "课程数据文件" << COURSE_FILE_NAME << "读取失败！" << std::endl;
	}
	else
	{
		unsigned courseKey, studentKey;
		int maxSize;
		std::string name, place, time, line;
		AnalyzedTime tempT;
		while (!inFileCourse.eof())
		{
			getline(inFileCourse, line);
			std::stringstream lineStream(line);
			if (lineStream >> courseKey)
			{
				lineStream.get();
				if (!getline(lineStream, name, ','))
				{
					std::cout << "Key：" << courseKey << " 读取课程名字失败！\n";
					outLog << "Key：" << courseKey << " 读取课程名字失败！\n";
					continue;
				}
				else if (!getline(lineStream, place, ','))
				{
					std::cout << "Key：" << courseKey << " 读取授课地点失败！\n";
					outLog << "Key：" << courseKey << " 读取授课地点失败！\n";
					continue;
				}
				else if (!(getline(lineStream, time, ',') && tempT.SetTime(time)))
				{
					std::cout << "Key：" << courseKey << " 读取课程时间失败！\n";
					outLog << "Key：" << courseKey << " 读取课程时间失败！\n";
					continue;
				}
				else if (!(lineStream >> maxSize))
				{
					std::cout << "Key：" << courseKey << " 读取课程容量失败！\n";
					outLog << "Key：" << courseKey << " 读取课程容量失败！\n";
					continue;
				}
				else
				{
					if (!courseList.Insert(courseKey, Course(name, place, time, maxSize)))
					{
						std::cout << "Key：" << courseKey << " 添加课程失败，已有相同key的课程信息！\n";
						outLog << "Key：" << courseKey << " 添加课程失败，已有相同key的课程信息！\n";
						continue;
					}
					lineStream.get();
					int count = 0;
					while (lineStream >> studentKey)
					{
						lineStream.get();
						if (count >= maxSize)
						{
							std::cout << "CourseKey：" << courseKey << " StudentKey：" << studentKey << " 添加选课失败，课程容量已满！\n";
							outLog << "CourseKey：" << courseKey << " StudentKey：" << studentKey << " 添加选课失败，课程容量已满！\n";
							continue;
						}
						if (!studentList.Search(studentKey))
						{
							std::cout << "CourseKey：" << courseKey << " StudentKey：" << studentKey << " 添加选课失败，学生信息不存在！\n";
							outLog << "CourseKey：" << courseKey << " StudentKey：" << studentKey << " 添加选课失败，学生信息不存在！\n";
							continue;
						}
						if (PickCourseInTable(studentKey, courseKey))
							count++;
					}
				}
			}
			else
			{
				continue;
			}
		}
	}
	inFileCourse.close();
	endRead = clock();
	std::cout << "数据读取完成，所用时间：" << (float)(endRead - startRead) / CLOCKS_PER_SEC << 's' << std::endl;
	outLog << "数据读取完成，所用时间：" << (float)(endRead - startRead) / CLOCKS_PER_SEC << 's' << std::endl;
	outLog.close();
	PAUSE;
	return;
}

void CourseSystem::WriteInFile()
{
	clock_t startWrite = clock(), endWrite;
	std::ofstream outFileStudent, outFileCourse;
	outFileStudent.open(STUDENT_FILE_NAME, std::ios::out | std::ios::trunc);
	if (!outFileStudent.is_open())
	{
		std::cout << "写入文件失败！" << std::endl;
		PAUSE;
		return;
	}
	for (int i = 0; i < studentList.tableSize; i++)
	{
		if (studentList.bucket[i])
		{
			for (ChainNode<Student>* p = studentList.bucket[i]; p; p = p->link)
			{
				outFileStudent << p->key << ',' << p->data.name << ',' << p->data.NO << '\n';
			}
		}
	}
	outFileStudent.close();
	outFileCourse.open(COURSE_FILE_NAME, std::ios::out | std::ios::trunc);
	if (!outFileCourse.is_open())
	{
		std::cout << "写入文件失败！" << std::endl;
		PAUSE;
		return;
	}
	for (int i = 0; i < courseList.tableSize; i++)
	{
		if (courseList.bucket[i])
		{
			for (ChainNode<Course>* p = courseList.bucket[i]; p; p = p->link)
			{
				outFileCourse << p->key << ',' << p->data.name << ',' << p->data.place << ','
					<< p->data.time << ',' << p->data.maxSize;
				for (int j = 0; j < p->data.num; j++)
				{
					outFileCourse << ',' << p->data.studentList[j];
				}
				outFileCourse << '\n';
			}
		}
	}
	outFileCourse.close();
	endWrite = clock();
	std::cout << "数据保存成功：" << STUDENT_FILE_NAME << ' ' << COURSE_FILE_NAME 
		<< " 所用时间：" << (float)(endWrite - startWrite) / CLOCKS_PER_SEC << 's' << std::endl;
	PAUSE;
	return;
}

void CourseSystem::PrintInfo(bool isCourse)
{
	int n;
	std::ofstream outFile;
	bool printToFile = false;
	ResetIStrm();
	CLS;
	if (isCourse)
		std::cout << "显示课程信息：\n请选择显示所有课程(0)还是显示第n页(n)：";
	else
		std::cout << "显示学生信息：\n请选择显示所有学生(0)还是显示第n页(n)：";
	if (std::cin >> n)
	{
		// 当全部输出到屏幕且n过大，采用文件输出方式
		if (n == 0 && (isCourse ? courseList.currentSize : studentList.currentSize) >= 20)
		{
			printToFile = true;
			outFile.open(PRINT_FILE_NAME, std::ios::out | std::ios::trunc);
		}
		if (isCourse)
			courseList.PrintHashTable(n, printToFile ? outFile : std::cout);
		else
			studentList.PrintHashTable(n, printToFile ? outFile : std::cout);
		if (printToFile)
		{
			outFile.close();
			std::cout << "信息已输出至文件：" << PRINT_FILE_NAME << std::endl;
		}
		PAUSE;
		return;
	}
	else
	{
		std::cout << "输入错误！" << std::endl;
		PAUSE;
		return;
	}
}

void CourseSystem::AddCourse()
{
	unsigned key;
	int maxSize;
	std::string name, place, time;
	AnalyzedTime t;
	CLS;
	ResetIStrm();
	std::cout << "在系统中添加课程：\n请依次输入课程的Key号（最多9位）、课程名称、授课地点和课程的最大容量：" << std::endl;
	if (std::cin >> key >> name >> place >> maxSize)
	{
		if (courseList.Search(key))
		{
			std::cout << "已经存在相同Key的课程！" << std::endl;
			PAUSE;
			return;
		}
		std::cout << "请输入课程时间：\n" << "\t输入示例：\n" <<
			"\t\tWeeks1-16 Thu Classes1-3\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13 Weeks4-8 Sat Classes2-3" << std::endl;
		REM_ENTER;
		if (getline(std::cin, time))
		{
			if (t.SetTime(time, false)) //判断输入时间是否合法
			{
				if (courseList.Insert(key, Course(name, place, time, maxSize)))
				{
					std::cout << "添加课程成功！" << std::endl;
					PAUSE;
					return;
				}
				else
				{
					std::cout << "添加课程失败！" << std::endl;
					PAUSE;
					return;
				}
			}
			else
			{
				std::cout << "输入时间非法！" << std::endl;
				PAUSE;
				return;
			}
		}
		std::cout << "输入错误！" << std::endl;
		PAUSE;
		return;
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::AddStudent()
{
	unsigned key;
	std::string name, NO;
	CLS;
	ResetIStrm();
	std::cout << "在系统中添加学生：\n请依次输入学生的Key号（最多9位）、学生姓名和学号：" << std::endl;
	if (std::cin >> key >> name >> NO)
	{
		if (studentList.Search(key))
		{
			std::cout << "已经存在相同Key的学生！" << std::endl;
			PAUSE;
			return;
		}
		if (studentList.Insert(key, Student(name, NO)))
		{
			std::cout << "添加学生成功！" << std::endl;
			PAUSE;
			return;
		}
		std::cout << "添加学生失败！" << std::endl;
		PAUSE;
		return;
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::SearchCourse() 
{
	unsigned key;
	CLS;
	ResetIStrm();
	std::cout << "查询课程信息：\n请输入要查询的课程的Key：";
	if (std::cin >> key)
	{
		if (courseList.Search(key))
		{
			Course& CourseToSearch = courseList.Find(key).data;
			std::cout << "查询结果：\n\n" << CourseToSearch << "\n已选该课程的学生：\n\n";
			for (int i = 0; i < CourseToSearch.num; i++)
			{
				std::cout << "Key： " << studentList.Find(CourseToSearch.studentList[i]).key << '\n';
				std::cout << studentList.Find(CourseToSearch.studentList[i]).data <<'\n';
			}
			PAUSE;
			return;
		}
		else
		{
			std::cout << "课程不存在！" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::RemInfo(bool isCourse) 
{
	unsigned key;
	CLS;
	ResetIStrm();
	const std::string object = isCourse ? "课程" : "学生";
	std::cout << "从系统中删除" << object << "信息：\n请输入要删除的" << object << "的Key：";
	if (std::cin >> key)
	{
		bool searchResult;
		if (isCourse)
			searchResult = courseList.Search(key);
		else
			searchResult = studentList.Search(key);
		if (searchResult)
		{
			if (isCourse)
				std::cout << "查询结果：\n" << courseList.Find(key).data << std::endl;
			else
				std::cout << "查询结果：\n" << studentList.Find(key).data << std::endl;
			std::cout << "是(1)否(0)删除该" << object << "：";
			bool del;
			if (std::cin >> del)
			{
				if (del)
				{
					if (isCourse)
					{
						// 先删除选了这门课的学生对这门课的选课关系，再删除这门课的数据
						RemCourseFromStudents(key);
						courseList.Remove(key);
					}
					else
					{
						// 先删除该学生选的所有课的选课关系，再删除该学生的数据
						RemStudentFromCourses(key);
						studentList.Remove(key);
					}
					std::cout << "已从系统中移除该" << object << "！" << std::endl;
					PAUSE;
					return;
				}
				else
				{
					PAUSE;
					return;
				}
			}
			else
			{
				std::cout << "输入错误！" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << object << "不存在！" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::PickCourse(unsigned studentKey)
{
	unsigned courseKey;
	CLS;
	ResetIStrm();
	std::cout << "学生选课：\n请输入要选择的课程的Key：";
	if (std::cin >> courseKey)
	{
		if (courseList.Search(courseKey))
		{
			Course& CourseToSearch = courseList.Find(courseKey).data;
			std::cout << "查询结果：\n\n" << CourseToSearch << '\n';
			if (CourseToSearch.maxSize == CourseToSearch.num)
			{
				std::cout << "该课程已满，无法选课！" << std::endl;
				PAUSE;
				return;
			}
			if (CourseToSearch.Search(studentKey) != -1)
			{
				std::cout << "该课程已经选择，无法重复选课！" << std::endl;
				PAUSE;
				return;
			}
			AnalyzedTime totalTime = TotalTime(studentKey), courseTime;
			courseTime.SetTime(CourseToSearch.time);
			if (totalTime.IsConflict(courseTime))
			{
				std::cout << "时间冲突，无法选课！" << std::endl;
				PAUSE;
				return;
			}
			bool pick;
			std::cout << "是(1)否(0)选择该课程：";
			if (std::cin >> pick)
			{
				if (pick)
				{
					if (PickCourseInTable(studentKey, courseKey))
					{
						std::cout << "选课成功！" << std::endl;
						PAUSE;
						return;
					}
					else
					{
						std::cout << "选课失败！" << std::endl;
						PAUSE;
						return;
					}
				}
				PAUSE;
				return;
			}
			else
			{
				std::cout << "输入错误！" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "课程不存在！" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::ExitCourse(unsigned studentKey)
{
	unsigned courseKey;
	CLS;
	ResetIStrm();
	std::cout << "学生退选课程：\n请输入要退选的课程的Key：";
	if (std::cin >> courseKey)
	{
		if (courseList.Search(courseKey))
		{
			Course& CourseToSearch = courseList.Find(courseKey).data;
			std::cout << "查询结果：\n\n" << CourseToSearch << std::endl;
			if (CourseToSearch.Search(studentKey) == -1)
			{
				std::cout << "该课程未选择！" << std::endl;
				PAUSE;
				return;
			}
			bool isExit;
			std::cout << "是(1)否(0)退选该课程：";
			if (std::cin >> isExit)
			{
				if (isExit)
				{
					if (ExitCourseInTable(studentKey, courseKey))
					{
						std::cout << "退选成功！" << std::endl;
						PAUSE;
						return;
					}
					else
					{
						std::cout << "退选失败！" << std::endl;
						PAUSE;
						return;
					}
				}
				PAUSE;
				return;
			}
			else
			{
				std::cout << "输入错误！" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "课程不存在！" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::StudentLoop()
{
	CLS;
	ResetIStrm();
	std::cout << "用户组：学生\n请输入学生的Key：";
	unsigned studentKey;
	if (std::cin >> studentKey)
	{
		if (studentList.Search(studentKey))
		{
			while (1)
			{
				CLS;
				ResetIStrm();
				std::cout << "学生信息：\n";
				std::cout << "Key： " << studentKey << '\t' << studentList.Find(studentKey).data;
				std::cout << "\n请选择服务：\n查看已选课程(1)、选课(2)、退选(3)或者返回上级菜单(0)\n";
				int command;
				if (std::cin >> command)
				{
					if (command == 1)
					{
						std::cout << "已选课程一览：\n";
						PrintPickedCourse(studentKey);
						std::cout << '\n';
						PAUSE;
					}
					else if (command == 2)
					{
						PickCourse(studentKey);
					}
					else if (command == 3)
					{
						ExitCourse(studentKey);
					}
					else
					{
						PAUSE;
						return;
					}
				}
				else
				{
					std::cout << "输入错误！" << std::endl;
					PAUSE;
					return;
				}
			}
		}
		else
		{
			std::cout << "未查找到该学生！" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "输入错误！" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::AdminLoop()
{
	while (1)
	{
		CLS;
		ResetIStrm();
		std::cout << "用户组：管理员\n请选择服务：\n添加课程信息(1)、删除课程信息(2)、添加学生信息(3)、删除学生信息(4)、读取数据(5)、保存数据(6)或者返回上级菜单(0)\n";
		int command;
		if (std::cin >> command)
		{
			if (command == 1)
			{
				AddCourse();
			}
			else if (command == 2)
			{
				RemInfo();
			}
			else if (command == 3)
			{
				AddStudent();
			}
			else if (command == 4)
			{
				RemInfo(false);
			}
			else if (command == 5)
			{
				ReadFromFile();
			}
			else if (command == 6)
			{
				WriteInFile();
			}
			else
			{
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "输入错误！" << std::endl;
			PAUSE;
			return;
		}
	}
}

void CourseSystem::MainLoop()
{
	while (1)
	{
		CLS;
		ResetIStrm();
		std::cout << "学生选课系统\n请选择服务：\n显示学生信息(1)、显示课程信息(2)、查询课程选课情况(3)、"
			<< "学生组操作(4)、管理员组操作(5)或者退出程序(0)\n";
		int command;
		if (std::cin >> command)
		{
			if (command == 1)
			{
				PrintInfo(false);
			}
			else if (command == 2)
			{
				PrintInfo();
			}
			else if (command == 3)
			{
				SearchCourse();
			}
			else if (command == 4)
			{
				StudentLoop();
			}
			else if (command == 5)
			{
				AdminLoop();
			}
			else
			{
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "输入错误！" << std::endl;
			PAUSE;
			return;
		}
	}
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
	return 0;
}

void ResetIStrm()
{
	if (std::cin.rdstate())
	{
		std::cin.clear();
	}
	std::cout << "(如果阻塞，请按Enter键继续)\n" << std::endl;
	std::cin.ignore(1000, '\n');
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
	TEST(clist.Insert(11111111, Course("数据结构", "东南院204", "1-16周 周一 4-5节", 100)), false);
	TEST(clist.Insert(36323543, Course("数据分析", "东南院104", "1-16周 周二 3-5节", 25)), false);
	clist.PrintHashTable(1);
	clist.PrintHashTable(2);
	clist.PrintHashTable(3);
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

void test4()
{
	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	cSys.AddCourse();
	cSys.AddCourse();
	cSys.PrintInfo();
	cSys.SearchCourse();
	cSys.RemInfo();
	cSys.PrintInfo();
	cSys.AddStudent();
	cSys.AddStudent();
	cSys.PrintInfo(false);
	cSys.RemInfo(false);
	cSys.PrintInfo(false);
}

void test5()
{
	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	cSys.courseList.Insert(1, Course("人体解剖", "中山院503", "Weeks1-16 Mon Classes1-3", 35));
	cSys.courseList.Insert(2, Course("化学信息", "中山院507", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(3, Course("数据分析", "东南院104", "Weeks1-16 Wed Classes1-3", 25));
	cSys.courseList.Insert(4, Course("Python", "中山院403", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(5, Course("数据结构", "东南院204", "Weeks1-16 Thu Classes1-2", 100));
	cSys.courseList.Insert(6, Course("形势与政策", "礼东102", "Weeks1-16 Fri Classes1-2", 200));
	cSys.studentList.Insert(1, Student("王朝兴", "11320111"));
	cSys.studentList.Insert(2, Student("张三", "11320154"));
	cSys.studentList.Insert(3, Student("李四", "TJ210104"));
	cSys.studentList.Insert(4, Student("王五", "D1120524"));
	cSys.studentList.Insert(5, Student("赵六", "S1120213"));
	cSys.studentList.Insert(6, Student("古尔丹", "11119123"));
	while (1)
	{
		cSys.StudentLoop();
		cSys.SearchCourse();
		cSys.PrintInfo();
	}
}

void test6()
{
	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	cSys.courseList.Insert(1, Course("人体解剖", "中山院503", "Weeks1-16 Mon Classes1-3", 35));
	cSys.courseList.Insert(2, Course("化学信息", "中山院507", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(3, Course("数据分析", "东南院104", "Weeks1-16 Wed Classes1-3", 25));
	cSys.courseList.Insert(4, Course("Python", "中山院403", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(5, Course("数据结构", "东南院204", "Weeks1-16 Thu Classes1-2", 100));
	cSys.courseList.Insert(6, Course("形势与政策", "礼东102", "Weeks1-16 Fri Classes1-2", 200));
	cSys.studentList.Insert(1, Student("王朝兴", "11320111"));
	cSys.studentList.Insert(2, Student("张三", "11320154"));
	cSys.studentList.Insert(3, Student("李四", "TJ210104"));
	cSys.studentList.Insert(4, Student("王五", "D1120524"));
	cSys.studentList.Insert(5, Student("赵六", "S1120213"));
	cSys.studentList.Insert(6, Student("古尔丹", "11119123"));
	cSys.MainLoop();
}

int main()
{
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	//test6();

	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	cSys.MainLoop();
 
	//_CrtDumpMemoryLeaks();  // 检测内存是否泄漏
	return 0;
}