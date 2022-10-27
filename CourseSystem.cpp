#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>
#include "CourseSystem.h"

#define CLS (system("cls"))
#define PAUSE (system("pause"))
#define REM_ENTER (std::cin.ignore(1000, '\n')) 

// ��������
// �γ̹�ϣ���bucket������ӦС��65535(unsigned short�ķ�Χ)����ʵ�ʴ洢�Ŀγ�����û������
#define COURSE_BUCKET 15000
// ѧ����ϣ���bucket��������������
#define STUDENT_BUCKET 800000
// ����Ͷ�ȡ���ļ�������
#define COURSE_FILE_NAME ("COURSE_DATA.csv")
#define STUDENT_FILE_NAME ("STUDENT_DATA.csv")
#define READ_LOG_NAME ("Log.txt")
#define PRINT_FILE_NAME ("Print.txt")

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

// ��Ԫ���Եĺ궨��
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

template<class T> void HashTable<T>::PrintHashTable(int n, std::ostream& out)
{
	int print_count = 0;
	out << "//////////////////////////////////////////////\n����" << currentSize << "�����ݡ�\n";
	if (n)
		out << "��" << n << "ҳ�����\n\n";
	else
		out << "��Ϣһ����\n\n";
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
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
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
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
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
		std::cerr << "�����ڴ�ʧ�ܣ�\n";
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
		std::cerr << "�ڴ�������\n";
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
	for (i = 0; i < num; i++) // �������
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
	while (low <= high) {   //�۰����
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
	out << "ѧ�����֣� " << student.name << "  ";
	out << "ѧ�ţ� " << student.NO << '\n';
	//out << "Bucket����: " << student.num << " / " << student.maxSize << '\n';
	return out;
}


//////////////////////////AnalyzedTime��///////////////////////////////////////////////////////////////////////////////
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
				&& weekBegin > 0 && day > 0) // temp�б�����ǵڼ��ڿε���Ϣ����֮ǰ�Ѿ���֪���ܴκ����ڼ�
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
								return false; // <int2> �ĺϷ��Լ�飬������Ϊtrue˵���Ϸ�
						}
						else  // "Classess<int>-"  �Ϸ����룺��<int>��13�ڿ�
							classEnd = 13;
					}
					else // "Classess<int>!" �Ƿ�����
						return false;
				}
				else   // "Classess<int>"  �Ϸ����룺��<int>�ڿ�
					classEnd = classBegin;

				// ��bool[][][]���޸Ŀ�ʱ
				for (int week = weekBegin; week <= weekEnd; week++)
					for (int clasS = classBegin; clasS <= classEnd; clasS++)
						time[week - 1][day - 1][clasS - 1] = add;
			}
			else
				return false;
		}
		else if (temp.size() > 5 && temp.size() <= 10 && temp.substr(0, 5) == "Weeks")
			// temp�б�����ǵڼ��ܵ���Ϣ
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
								return false; // <int2> �ĺϷ��Լ�飬������Ϊtrue˵���Ϸ�
						}
						else  // "Week<int>-"  �Ϸ����룺��<int>��16��
							weekEnd = 16;
					}
					else // "Week<int>!" �Ƿ�����
						return false;
				}
				else   // "Weeks<int>"  �Ϸ����룺��<int>��
					weekEnd = weekBegin;
			}
			else
				return false;
		}
		else if (temp.size() == 3) // temp�б���������ڼ�����Ϣ
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
		courseBucketIdx = studentToDel.courseList[i]; //ѧ���洢��ѡ����Ϣ���ǿγ̵�Key�����ǿγ̵�Bucket����
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
		// �����ڿγ̹�ϣ���е���ͬbucket�Ƿ��д������ѧ��ѡ�Ŀ�
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
		// ���û�еĻ���ɾ����bucket������
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
	// �����ڿγ̹�ϣ���е���ͬbucket�Ƿ��д������ѧ��ѡ�Ŀ�
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
	// ���û�еĻ���ɾ����bucket������
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
				std::cout << "\nKey�� " << p->key << '\n';
				std::cout << p->data;
				count++;
			}
		}
	}
	std::cout << "\nһ����" << count << "���γ̡�" << std::endl;
}

// .csv�ļ����棬��������
// �γ����ݣ�key, name, place, time, maxSize, studentKey *
// ѧ�����ݣ�key, name, NO

void CourseSystem::ReadFromFile()
{
	clock_t startRead = clock(), endRead;
	std::ifstream inFileStudent, inFileCourse;
	std::ofstream outLog;
	outLog.open(READ_LOG_NAME, std::ios::out | std::ios::trunc);
	// ��ȡѧ������
	inFileStudent.open(STUDENT_FILE_NAME, std::ios::in);
	if (!inFileStudent.is_open())
	{ 
		std::cout << "ѧ�������ļ�" << STUDENT_FILE_NAME << "��ȡʧ�ܣ�" << std::endl;
		outLog << "ѧ�������ļ�" << STUDENT_FILE_NAME << "��ȡʧ�ܣ�" << std::endl;
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
					std::cout << "Key��" << key << " ��ȡѧ������ʧ�ܣ�\n";
					outLog << "Key��" << key << " ��ȡѧ������ʧ�ܣ�\n";
					continue;
				}
				else if (!getline(lineStream, NO))
				{
					std::cout << "Key��" << key << " ��ȡѧ��ѧ��ʧ�ܣ�\n";
					outLog << "Key��" << key << " ��ȡѧ��ѧ��ʧ�ܣ�\n";
					continue;
				}
				else
				{
					if (!studentList.Insert(key, Student(name, NO)))
					{
						std::cout << "Key��" << key << " ���ѧ��ʧ�ܣ�������ͬkey��ѧ����Ϣ��\n";
						outLog << "Key��" << key << " ���ѧ��ʧ�ܣ�������ͬkey��ѧ����Ϣ��\n";
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
	// ��ȡ�γ�����
	inFileCourse.open(COURSE_FILE_NAME, std::ios::in);
	if (!inFileCourse.is_open())
	{
		std::cout << "�γ������ļ�" << COURSE_FILE_NAME << "��ȡʧ�ܣ�" << std::endl;
		outLog << "�γ������ļ�" << COURSE_FILE_NAME << "��ȡʧ�ܣ�" << std::endl;
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
					std::cout << "Key��" << courseKey << " ��ȡ�γ�����ʧ�ܣ�\n";
					outLog << "Key��" << courseKey << " ��ȡ�γ�����ʧ�ܣ�\n";
					continue;
				}
				else if (!getline(lineStream, place, ','))
				{
					std::cout << "Key��" << courseKey << " ��ȡ�ڿεص�ʧ�ܣ�\n";
					outLog << "Key��" << courseKey << " ��ȡ�ڿεص�ʧ�ܣ�\n";
					continue;
				}
				else if (!(getline(lineStream, time, ',') && tempT.SetTime(time)))
				{
					std::cout << "Key��" << courseKey << " ��ȡ�γ�ʱ��ʧ�ܣ�\n";
					outLog << "Key��" << courseKey << " ��ȡ�γ�ʱ��ʧ�ܣ�\n";
					continue;
				}
				else if (!(lineStream >> maxSize))
				{
					std::cout << "Key��" << courseKey << " ��ȡ�γ�����ʧ�ܣ�\n";
					outLog << "Key��" << courseKey << " ��ȡ�γ�����ʧ�ܣ�\n";
					continue;
				}
				else
				{
					if (!courseList.Insert(courseKey, Course(name, place, time, maxSize)))
					{
						std::cout << "Key��" << courseKey << " ��ӿγ�ʧ�ܣ�������ͬkey�Ŀγ���Ϣ��\n";
						outLog << "Key��" << courseKey << " ��ӿγ�ʧ�ܣ�������ͬkey�Ŀγ���Ϣ��\n";
						continue;
					}
					lineStream.get();
					int count = 0;
					while (lineStream >> studentKey)
					{
						lineStream.get();
						if (count >= maxSize)
						{
							std::cout << "CourseKey��" << courseKey << " StudentKey��" << studentKey << " ���ѡ��ʧ�ܣ��γ�����������\n";
							outLog << "CourseKey��" << courseKey << " StudentKey��" << studentKey << " ���ѡ��ʧ�ܣ��γ�����������\n";
							continue;
						}
						if (!studentList.Search(studentKey))
						{
							std::cout << "CourseKey��" << courseKey << " StudentKey��" << studentKey << " ���ѡ��ʧ�ܣ�ѧ����Ϣ�����ڣ�\n";
							outLog << "CourseKey��" << courseKey << " StudentKey��" << studentKey << " ���ѡ��ʧ�ܣ�ѧ����Ϣ�����ڣ�\n";
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
	std::cout << "���ݶ�ȡ��ɣ�����ʱ�䣺" << (float)(endRead - startRead) / CLOCKS_PER_SEC << 's' << std::endl;
	outLog << "���ݶ�ȡ��ɣ�����ʱ�䣺" << (float)(endRead - startRead) / CLOCKS_PER_SEC << 's' << std::endl;
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
		std::cout << "д���ļ�ʧ�ܣ�" << std::endl;
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
		std::cout << "д���ļ�ʧ�ܣ�" << std::endl;
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
	std::cout << "���ݱ���ɹ���" << STUDENT_FILE_NAME << ' ' << COURSE_FILE_NAME 
		<< " ����ʱ�䣺" << (float)(endWrite - startWrite) / CLOCKS_PER_SEC << 's' << std::endl;
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
		std::cout << "��ʾ�γ���Ϣ��\n��ѡ����ʾ���пγ�(0)������ʾ��nҳ(n)��";
	else
		std::cout << "��ʾѧ����Ϣ��\n��ѡ����ʾ����ѧ��(0)������ʾ��nҳ(n)��";
	if (std::cin >> n)
	{
		// ��ȫ���������Ļ��n���󣬲����ļ������ʽ
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
			std::cout << "��Ϣ��������ļ���" << PRINT_FILE_NAME << std::endl;
		}
		PAUSE;
		return;
	}
	else
	{
		std::cout << "�������" << std::endl;
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
	std::cout << "��ϵͳ����ӿγ̣�\n����������γ̵�Key�ţ����9λ�����γ����ơ��ڿεص�Ϳγ̵����������" << std::endl;
	if (std::cin >> key >> name >> place >> maxSize)
	{
		if (courseList.Search(key))
		{
			std::cout << "�Ѿ�������ͬKey�Ŀγ̣�" << std::endl;
			PAUSE;
			return;
		}
		std::cout << "������γ�ʱ�䣺\n" << "\t����ʾ����\n" <<
			"\t\tWeeks1-16 Thu Classes1-3\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13\n" <<
			"\t\tWeeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13 Weeks4-8 Sat Classes2-3" << std::endl;
		REM_ENTER;
		if (getline(std::cin, time))
		{
			if (t.SetTime(time, false)) //�ж�����ʱ���Ƿ�Ϸ�
			{
				if (courseList.Insert(key, Course(name, place, time, maxSize)))
				{
					std::cout << "��ӿγ̳ɹ���" << std::endl;
					PAUSE;
					return;
				}
				else
				{
					std::cout << "��ӿγ�ʧ�ܣ�" << std::endl;
					PAUSE;
					return;
				}
			}
			else
			{
				std::cout << "����ʱ��Ƿ���" << std::endl;
				PAUSE;
				return;
			}
		}
		std::cout << "�������" << std::endl;
		PAUSE;
		return;
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::AddStudent()
{
	unsigned key;
	std::string name, NO;
	CLS;
	ResetIStrm();
	std::cout << "��ϵͳ�����ѧ����\n����������ѧ����Key�ţ����9λ����ѧ��������ѧ�ţ�" << std::endl;
	if (std::cin >> key >> name >> NO)
	{
		if (studentList.Search(key))
		{
			std::cout << "�Ѿ�������ͬKey��ѧ����" << std::endl;
			PAUSE;
			return;
		}
		if (studentList.Insert(key, Student(name, NO)))
		{
			std::cout << "���ѧ���ɹ���" << std::endl;
			PAUSE;
			return;
		}
		std::cout << "���ѧ��ʧ�ܣ�" << std::endl;
		PAUSE;
		return;
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::SearchCourse() 
{
	unsigned key;
	CLS;
	ResetIStrm();
	std::cout << "��ѯ�γ���Ϣ��\n������Ҫ��ѯ�Ŀγ̵�Key��";
	if (std::cin >> key)
	{
		if (courseList.Search(key))
		{
			Course& CourseToSearch = courseList.Find(key).data;
			std::cout << "��ѯ�����\n\n" << CourseToSearch << "\n��ѡ�ÿγ̵�ѧ����\n\n";
			for (int i = 0; i < CourseToSearch.num; i++)
			{
				std::cout << "Key�� " << studentList.Find(CourseToSearch.studentList[i]).key << '\n';
				std::cout << studentList.Find(CourseToSearch.studentList[i]).data <<'\n';
			}
			PAUSE;
			return;
		}
		else
		{
			std::cout << "�γ̲����ڣ�" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::RemInfo(bool isCourse) 
{
	unsigned key;
	CLS;
	ResetIStrm();
	const std::string object = isCourse ? "�γ�" : "ѧ��";
	std::cout << "��ϵͳ��ɾ��" << object << "��Ϣ��\n������Ҫɾ����" << object << "��Key��";
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
				std::cout << "��ѯ�����\n" << courseList.Find(key).data << std::endl;
			else
				std::cout << "��ѯ�����\n" << studentList.Find(key).data << std::endl;
			std::cout << "��(1)��(0)ɾ����" << object << "��";
			bool del;
			if (std::cin >> del)
			{
				if (del)
				{
					if (isCourse)
					{
						// ��ɾ��ѡ�����ſε�ѧ�������ſε�ѡ�ι�ϵ����ɾ�����ſε�����
						RemCourseFromStudents(key);
						courseList.Remove(key);
					}
					else
					{
						// ��ɾ����ѧ��ѡ�����пε�ѡ�ι�ϵ����ɾ����ѧ��������
						RemStudentFromCourses(key);
						studentList.Remove(key);
					}
					std::cout << "�Ѵ�ϵͳ���Ƴ���" << object << "��" << std::endl;
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
				std::cout << "�������" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << object << "�����ڣ�" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::PickCourse(unsigned studentKey)
{
	unsigned courseKey;
	CLS;
	ResetIStrm();
	std::cout << "ѧ��ѡ�Σ�\n������Ҫѡ��Ŀγ̵�Key��";
	if (std::cin >> courseKey)
	{
		if (courseList.Search(courseKey))
		{
			Course& CourseToSearch = courseList.Find(courseKey).data;
			std::cout << "��ѯ�����\n\n" << CourseToSearch << '\n';
			if (CourseToSearch.maxSize == CourseToSearch.num)
			{
				std::cout << "�ÿγ��������޷�ѡ�Σ�" << std::endl;
				PAUSE;
				return;
			}
			if (CourseToSearch.Search(studentKey) != -1)
			{
				std::cout << "�ÿγ��Ѿ�ѡ���޷��ظ�ѡ�Σ�" << std::endl;
				PAUSE;
				return;
			}
			AnalyzedTime totalTime = TotalTime(studentKey), courseTime;
			courseTime.SetTime(CourseToSearch.time);
			if (totalTime.IsConflict(courseTime))
			{
				std::cout << "ʱ���ͻ���޷�ѡ�Σ�" << std::endl;
				PAUSE;
				return;
			}
			bool pick;
			std::cout << "��(1)��(0)ѡ��ÿγ̣�";
			if (std::cin >> pick)
			{
				if (pick)
				{
					if (PickCourseInTable(studentKey, courseKey))
					{
						std::cout << "ѡ�γɹ���" << std::endl;
						PAUSE;
						return;
					}
					else
					{
						std::cout << "ѡ��ʧ�ܣ�" << std::endl;
						PAUSE;
						return;
					}
				}
				PAUSE;
				return;
			}
			else
			{
				std::cout << "�������" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "�γ̲����ڣ�" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::ExitCourse(unsigned studentKey)
{
	unsigned courseKey;
	CLS;
	ResetIStrm();
	std::cout << "ѧ����ѡ�γ̣�\n������Ҫ��ѡ�Ŀγ̵�Key��";
	if (std::cin >> courseKey)
	{
		if (courseList.Search(courseKey))
		{
			Course& CourseToSearch = courseList.Find(courseKey).data;
			std::cout << "��ѯ�����\n\n" << CourseToSearch << std::endl;
			if (CourseToSearch.Search(studentKey) == -1)
			{
				std::cout << "�ÿγ�δѡ��" << std::endl;
				PAUSE;
				return;
			}
			bool isExit;
			std::cout << "��(1)��(0)��ѡ�ÿγ̣�";
			if (std::cin >> isExit)
			{
				if (isExit)
				{
					if (ExitCourseInTable(studentKey, courseKey))
					{
						std::cout << "��ѡ�ɹ���" << std::endl;
						PAUSE;
						return;
					}
					else
					{
						std::cout << "��ѡʧ�ܣ�" << std::endl;
						PAUSE;
						return;
					}
				}
				PAUSE;
				return;
			}
			else
			{
				std::cout << "�������" << std::endl;
				PAUSE;
				return;
			}
		}
		else
		{
			std::cout << "�γ̲����ڣ�" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::StudentLoop()
{
	CLS;
	ResetIStrm();
	std::cout << "�û��飺ѧ��\n������ѧ����Key��";
	unsigned studentKey;
	if (std::cin >> studentKey)
	{
		if (studentList.Search(studentKey))
		{
			while (1)
			{
				CLS;
				ResetIStrm();
				std::cout << "ѧ����Ϣ��\n";
				std::cout << "Key�� " << studentKey << '\t' << studentList.Find(studentKey).data;
				std::cout << "\n��ѡ�����\n�鿴��ѡ�γ�(1)��ѡ��(2)����ѡ(3)���߷����ϼ��˵�(0)\n";
				int command;
				if (std::cin >> command)
				{
					if (command == 1)
					{
						std::cout << "��ѡ�γ�һ����\n";
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
					std::cout << "�������" << std::endl;
					PAUSE;
					return;
				}
			}
		}
		else
		{
			std::cout << "δ���ҵ���ѧ����" << std::endl;
			PAUSE;
			return;
		}
	}
	std::cout << "�������" << std::endl;
	PAUSE;
	return;
}

void CourseSystem::AdminLoop()
{
	while (1)
	{
		CLS;
		ResetIStrm();
		std::cout << "�û��飺����Ա\n��ѡ�����\n��ӿγ���Ϣ(1)��ɾ���γ���Ϣ(2)�����ѧ����Ϣ(3)��ɾ��ѧ����Ϣ(4)����ȡ����(5)����������(6)���߷����ϼ��˵�(0)\n";
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
			std::cout << "�������" << std::endl;
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
		std::cout << "ѧ��ѡ��ϵͳ\n��ѡ�����\n��ʾѧ����Ϣ(1)����ʾ�γ���Ϣ(2)����ѯ�γ�ѡ�����(3)��"
			<< "ѧ�������(4)������Ա�����(5)�����˳�����(0)\n";
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
			std::cout << "�������" << std::endl;
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
	std::cout << "(����������밴Enter������)\n" << std::endl;
	std::cin.ignore(1000, '\n');
}


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
	TEST(clist.Insert(11111111, Course("���ݽṹ", "����Ժ204", "1-16�� ��һ 4-5��", 100)), false);
	TEST(clist.Insert(36323543, Course("���ݷ���", "����Ժ104", "1-16�� �ܶ� 3-5��", 25)), false);
	clist.PrintHashTable(1);
	clist.PrintHashTable(2);
	clist.PrintHashTable(3);
	END_TEST();

	START_TEST();
	// ����ĳ���γ̣��鿴�Ƿ����
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

void test2()
{
	START_TEST();
	// ���챣��ѧ����Ϣ�Ĺ�ϣ��
	HashTable<Student> clist(Divisor(STUDENT_BUCKET), STUDENT_BUCKET);
	// ���ѧ����Ϣ
	// Student���캯���� Student(std::string name, std::string NO);
	TEST(clist.Insert(15564546, Student("������", "11320111")), true);
	TEST(clist.Insert(13124121, Student("����", "11320154")), true);
	TEST(clist.Insert(36323543, Student("����", "TJ210104")), true);
	TEST(clist.Insert(11111509, Student("����", "D1120524")), true);
	TEST(clist.Insert(11111111, Student("����", "S1120213")), true);
	TEST(clist.Insert(11111310, Student("�Ŷ���", "11119123")), true);
	clist.PrintHashTable();
	END_TEST();

	START_TEST();
	// ����ĳ��ѧ�����鿴�Ƿ����
	TEST(clist.Search(11111111), true);
	for (int i = 0; i < 80; i++)
	{  // Ϊĳ��ѧ����ӿγ̵�bucket����
		TEST(clist.Find(11111111).data.AddCourseBucket(i + 100), true);
	}
	// ����ĳ��ѧ������ʾ��Ϣ
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	for (int i = 60; i < 80; i++)
	{ // Ϊĳ��ѧ��ɾ���γ̵�bucket����
		TEST(clist.Find(11111111).data.RemCourseBucket(i + 100), true);
	}
	std::cout << clist.Find(11111111).data << std::endl;
	END_TEST();

	START_TEST();
	// ����ĳ��ѧ������ʾ��Ϣ
	TEST(clist.Search(1111111), false);
	TEST(clist.Search(11111111), true);
	std::cout << clist.Find(11111111).data << std::endl;
	// �Ƴ�ĳ��ѧ��
	TEST(clist.Remove(11111509), true);
	clist.PrintHashTable();
	END_TEST();
}

void test3()
{
	START_TEST();
	CourseSystem cSys(COURSE_BUCKET, STUDENT_BUCKET);
	AnalyzedTime t1, t2;
	// �����Ƿ����ж��ַ����Ƿ�Ϸ�
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
	// ����ʱ����������Ƿ�����
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
	cSys.courseList.Insert(1, Course("�������", "��ɽԺ503", "Weeks1-16 Mon Classes1-3", 35));
	cSys.courseList.Insert(2, Course("��ѧ��Ϣ", "��ɽԺ507", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(3, Course("���ݷ���", "����Ժ104", "Weeks1-16 Wed Classes1-3", 25));
	cSys.courseList.Insert(4, Course("Python", "��ɽԺ403", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(5, Course("���ݽṹ", "����Ժ204", "Weeks1-16 Thu Classes1-2", 100));
	cSys.courseList.Insert(6, Course("����������", "��102", "Weeks1-16 Fri Classes1-2", 200));
	cSys.studentList.Insert(1, Student("������", "11320111"));
	cSys.studentList.Insert(2, Student("����", "11320154"));
	cSys.studentList.Insert(3, Student("����", "TJ210104"));
	cSys.studentList.Insert(4, Student("����", "D1120524"));
	cSys.studentList.Insert(5, Student("����", "S1120213"));
	cSys.studentList.Insert(6, Student("�Ŷ���", "11119123"));
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
	cSys.courseList.Insert(1, Course("�������", "��ɽԺ503", "Weeks1-16 Mon Classes1-3", 35));
	cSys.courseList.Insert(2, Course("��ѧ��Ϣ", "��ɽԺ507", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(3, Course("���ݷ���", "����Ժ104", "Weeks1-16 Wed Classes1-3", 25));
	cSys.courseList.Insert(4, Course("Python", "��ɽԺ403", "Weeks1-16 Tue Classes1-2", 30));
	cSys.courseList.Insert(5, Course("���ݽṹ", "����Ժ204", "Weeks1-16 Thu Classes1-2", 100));
	cSys.courseList.Insert(6, Course("����������", "��102", "Weeks1-16 Fri Classes1-2", 200));
	cSys.studentList.Insert(1, Student("������", "11320111"));
	cSys.studentList.Insert(2, Student("����", "11320154"));
	cSys.studentList.Insert(3, Student("����", "TJ210104"));
	cSys.studentList.Insert(4, Student("����", "D1120524"));
	cSys.studentList.Insert(5, Student("����", "S1120213"));
	cSys.studentList.Insert(6, Student("�Ŷ���", "11119123"));
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
 
	//_CrtDumpMemoryLeaks();  // ����ڴ��Ƿ�й©
	return 0;
}