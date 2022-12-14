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
	friend class CourseSystem;
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

	// 根据key找到对应节点
	// 调用该函数前须调用Search(k)来保证对应元素存在
	ChainNode<T>& Find(unsigned k);

	// 显示所有表中元素
	// 如果n=0直接显示所有结果；n不为0则以5个元素为一页，显示第n页结果
	void PrintHashTable(int n = 0, std::ostream& out = std::cout);
};

// 哈希表中的课程信息
class Course {
	friend class CourseSystem;
private:
	// 课程中学生的数量
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

// 哈希表中的学生信息
class Student {
	friend class CourseSystem;
private:
	// 学生中课程的数量
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

	// 调整动态数组容量，以8字节为单位，broaden为true代表扩容，反之缩减容量
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

// 被解析后的时间类
//     std::string保存时间的格式示例：
//     "Weeks1-16 Thu Classes1-3" 
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3"
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13" 
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13 Weeks4-8 Sat Classes2-3"
class AnalyzedTime {
private:
	// 16周，7天，每位代表1节课，每天最多13节课
	bool time[16][7][13];  
public:
	AnalyzedTime();
	// 如果weeks为0，代表1-16周第days天第classes节课
	AnalyzedTime(int weeks, int days, int classes);
	AnalyzedTime& operator =(const AnalyzedTime& t);
	bool operator ==(const AnalyzedTime& t);
	AnalyzedTime& operator +=(const AnalyzedTime& t);
	AnalyzedTime& operator -=(const AnalyzedTime& t);

	// 判断两个时间是否冲突
	bool IsConflict(const AnalyzedTime& t);

	// 清空时间，bool[][][]置false
	void ClearTime();

	// 添加或者移除时间，解析保存在字符串中的时间信息，也可以判断字符串是否合法
	bool SetTime(const std::string strTime, bool add = true);
};

// 整个选课系统的ui如下：
// MainLoop ─┬ PrintInfo(学生) (1)
//           ├ PrintInfo(课程) (2)
//           ├ SearchCourse (3)
//           ├ StudentLoop (4) ───┬ PrintPickedCourse (1)
//           │                    ├ PickCourse (2)
//           │                    ├ ExitCourse (3)
//           │                    └ PrintCourseTable (4)
//           └ AdminLoop (5) ────┬ AddCourse (1)
//                               ├ RemInfo(课程) (2)
//                               ├ AddStudent (3)
//                               ├ RemInfo(学生) (4)
//                               ├ ResetCourse (5)
//                               ├ ResetStuednt (6)
//                               ├ ReadFromFile (7)
//                               └ WriteInFile (8)

// 连系两个哈希表，封装用户交互的最顶层的类
class CourseSystem {
private:
	HashTable<Course> courseList;
	HashTable<Student> studentList;
public:
	CourseSystem(int courseBucket, int studentBucket)
		: courseList(Divisor(courseBucket), courseBucket), 
		studentList(Divisor(studentBucket), studentBucket) {}

	// 删除课程哈希表中的相应学生的信息，用于删除学生前清除学生选课关系
	void RemStudentFromCourses(unsigned studentKey);
	// 删除学生哈希表中的相应课程的信息，用于删除课程前清除学生选课关系
	void RemCourseFromStudents(unsigned courseKey);

	// 根据学生已选课程，求出课程所占用的时间段
	AnalyzedTime TotalTime(unsigned studentKey);

	// 在两个哈希表中登记学生的选课操作
	bool PickCourseInTable(unsigned studentKey, unsigned courseKey);
	// 在两个哈希表中登记学生的退选操作
	bool ExitCourseInTable(unsigned studentKey, unsigned courseKey);

	// 显示学生所有已选的课程信息
	void PrintPickedCourse(unsigned studentKey);

	// 构建课程表csv文件并且从外部应用打开
	void PrintCourseTable(unsigned studentKey);

	// 从文件中读取信息
	// 读取学生\课程信息时，遇到相同key则不在表中添加该信息；
	void ReadFromFile();
	// 将保存的信息写入文件
	void WriteInFile();

	//================================================================================
	// 以该注释为分界线，该类上述的成员函数与数据结构有关，以下成员函数实现的是ui的功能

	// 显示所有课程/学生信息
	void PrintInfo(bool isCourse = true);

	// 添加课程信息，从选课系统界面输入
	void AddCourse();

	// 添加学生信息，从选课系统界面输入
	void AddStudent();

	// 查询课程信息，会显示所有选课的学生
	void SearchCourse();

	// 删除课程/学生信息
	void RemInfo(bool isCourse = true);

	// 学生选课
	void PickCourse(unsigned studentKey);
	// 学生退选
	void ExitCourse(unsigned studentKey);

	// 修改某个学生的信息
	void ResetStudent();

	// 修改某个课程的信息
	void ResetCourse();

	// 学生的操作的循环
	void StudentLoop();

	// 修改数据操作的循环
	void AdminLoop();

	// 主循环
	void MainLoop();

	friend void test5();
	friend void test6();
};

// 重置输入流状态，清空缓冲区，刷新输出缓冲区
void ResetIStrm();

// 和课程哈希表有关的测试
void test1();

// 和学生哈希表有关的测试
void test2();

// 和时间解析相关的测试
void test3();

// 和系统添加删除信息有关的测试
void test4();

// 和学生操作有关的测试
void test5();

// 预设一部分学生和课程信息的测试
void test6();
