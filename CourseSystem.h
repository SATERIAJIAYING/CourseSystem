#pragma once

template<class T> struct ChainNode {
	// 分别对应课程和学生的id
	unsigned key;
	ChainNode<T>* link;

	// 对应每个课程或者学生的信息
	T data;	
	ChainNode<T>() : key(0), link(NULL) {}
};

template<class T> class HashTable {
private:
	// 哈希函数中的余数
	int divisor;
	int currentSize, tableSize;
	ChainNode<T>** bucket;

	// 根据key返回对应的bucket索引号和匹配元素的地址，如果没有找到对应元素p返回空指针
	int Hash(unsigned k, ChainNode<T>*& p);
public:
	HashTable<T>(int divisor, int sz);
	~HashTable<T>();

	// 根据key在表中找到相应的元素，如果找到了返回false，找不到则插入且返回true
	bool Insert(unsigned k, const T& el);

	// 根据key搜索对应元素是否在表中
	bool Search(unsigned k); 

	// 根据key在表中找到相应的元素，如果找不到返回false，找到则删除且返回true
	bool Remove(unsigned k);

	// 调用该函数前须调用Search(k)来保证对应元素存在
	ChainNode<T>& Find(unsigned k);

	// 显示所有表中元素
	void PrintHashTable();
};

class Course {
private:
	int num, maxSize;

	// 存储选课学生id动态数组
	unsigned* studentList;
	std::string name, place, time;
public:
	Course() : studentList(NULL), num(0), maxSize(0), name(""), place(""), time("") {}
	Course& operator=(const Course& c);
	Course(std::string name, std::string place, std::string time, int maxSize);
	~Course();

	// 添加学生
	bool AddStudent(unsigned studentId);

	// 移除学生
	bool RemStudent(unsigned studentId);

	// 查找学生，返回bucket的索引，如找不到返回-1
	int Search(unsigned studentId);

	// 更改学生容量，如果之前已经选该课的学生数大于新的课容量，学生容量不会更改并返回false
	bool SetSize(int sz);

	friend std::ostream& operator <<(std::ostream& out, Course &course);
};

class Student {
private:
	int num, maxSize;

	// 存储学生所选课程所在bucket的索引的动态数组
	unsigned short* courseList;
	std::string name, NO;
public:
	Student() : courseList(NULL), num(0), maxSize(0), name(""), NO("") {}
	Student& operator=(const Student& s);
	Student(std::string name, std::string NO)
		: name(name), NO(NO), courseList(NULL), num(0), maxSize(0) {}
	~Student();

	// 调整动态数组容量，以8字节为单位
	bool SetSize(bool broaden = true);

	// 添加课程bucket索引号
	bool AddCourseBucket(unsigned short courseBucketIndex);

	// 移除课程bucket索引号
	bool RemCourseBucket(unsigned short courseBucketIndex);

	// 查找课程存放的位置，返回课程对应bucket的索引，如找不到返回-1
	int Search(unsigned short courseBucketIndex);

	friend std::ostream& operator <<(std::ostream& out, Student& student);
};

// 返回不大于bucket的最大素数，用于求哈希函数的余数
int Divisor(int bucket);

class CourseSystem {
private:
	HashTable<Course> courseList;
	HashTable<Student> StudentList;
public:
	CourseSystem(int courseBucket, int studentBucket)
		: courseList(Divisor(courseBucket), courseBucket), StudentList(Divisor(studentBucket), studentBucket) {}
};

// 被解析后的时间类
//     std::string保存时间的格式示例：
//     "Weeks1-16 Mon Classes1-3" 
//     "Weeks1-16 Mon Classes1-3 Sun Classes1-5 Tue Classes3"
//     "Weeks1-16 Mon Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13" 
//     "Weeks1-16 Mon Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13 Weeks4-8 Sat Classes2-3"
class AnalyzedTime {
private:
	// 16周，7天，每位代表1节课，每天最多13节课
	bool time[16][7][13];  
public:
	AnalyzedTime();
	AnalyzedTime& operator =(const AnalyzedTime& t);
	bool operator ==(const AnalyzedTime& t);
	AnalyzedTime& operator +=(const AnalyzedTime& t);
	AnalyzedTime& operator -=(const AnalyzedTime& t);

	// 判断两个时间是否冲突
	bool IsConflict(const AnalyzedTime& t);

	// 清空时间，bool[][][]置false
	void ClearTime();

	// 添加或者移除时间，解析保存在字符串中的时间信息，若字符串非法则返回false
	bool SetTime(const std::string strTime, bool add = true);
};



// 和课程哈希表有关的测试
void test1();

// 和学生哈希表有关的测试
void test2();

// 和时间解析相关的测试
void test3();
