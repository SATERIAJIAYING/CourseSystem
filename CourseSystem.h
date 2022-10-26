#pragma once

template<class T> struct ChainNode {
	// �ֱ��Ӧ�γ̺�ѧ����id
	unsigned key;
	ChainNode<T>* link;

	// ��Ӧÿ���γ̻���ѧ������Ϣ
	T data;	
	ChainNode<T>() : key(0), link(NULL) {}
};

template<class T> class HashTable {
	friend class CourseSystem;
private:
	// ��ϣ�����е�����
	int divisor;
	int currentSize, tableSize;
	ChainNode<T>** bucket;

	// ����key���ض�Ӧ��bucket�����ź�ƥ��Ԫ�صĵ�ַ�����û���ҵ���ӦԪ��p���ؿ�ָ��
	int Hash(unsigned k, ChainNode<T>*& p);
public:
	HashTable<T>(int divisor, int sz);
	~HashTable<T>();

	// ����key�ڱ����ҵ���Ӧ��Ԫ�أ�����ҵ��˷���false���Ҳ���������ҷ���true
	bool Insert(unsigned k, const T& el);

	// ����key������ӦԪ���Ƿ��ڱ���
	bool Search(unsigned k); 

	// ����key�ڱ����ҵ���Ӧ��Ԫ�أ�����Ҳ�������false���ҵ���ɾ���ҷ���true
	bool Remove(unsigned k);

	// ����key�ҵ���Ӧ�ڵ�
	// ���øú���ǰ�����Search(k)����֤��ӦԪ�ش���
	ChainNode<T>& Find(unsigned k);

	// ��ʾ���б���Ԫ��
	// ���n=0ֱ����ʾ���н����n��Ϊ0����5��Ԫ��Ϊһҳ����ʾ��nҳ���
	void PrintHashTable(int n = 0);
};

// ��ϣ���еĿγ���Ϣ
class Course {
	friend class CourseSystem;
private:
	// �γ���ѧ��������
	int num, maxSize;

	// �洢ѡ��ѧ��id��̬����
	unsigned* studentList;
	std::string name, place, time;
public:
	Course() : studentList(NULL), num(0), maxSize(0), name(""), place(""), time("") {}
	Course& operator=(const Course& c);
	Course(std::string name, std::string place, std::string time, int maxSize);
	~Course();

	// ���ѧ��
	bool AddStudent(unsigned studentId);

	// �Ƴ�ѧ��
	bool RemStudent(unsigned studentId);

	// ����ѧ��������bucket�����������Ҳ�������-1
	int Search(unsigned studentId);

	// ����ѧ�����������֮ǰ�Ѿ�ѡ�ÿε�ѧ���������µĿ�������ѧ������������Ĳ�����false
	bool SetSize(int sz);

	friend std::ostream& operator <<(std::ostream& out, Course &course);
};

// ��ϣ���е�ѧ����Ϣ
class Student {
	friend class CourseSystem;
private:
	// ѧ���пγ̵�����
	int num, maxSize;

	// �洢ѧ����ѡ�γ�����bucket�������Ķ�̬����
	unsigned short* courseList;
	std::string name, NO;
public:
	Student() : courseList(NULL), num(0), maxSize(0), name(""), NO("") {}
	Student& operator=(const Student& s);
	Student(std::string name, std::string NO)
		: name(name), NO(NO), courseList(NULL), num(0), maxSize(0) {}
	~Student();

	// ������̬������������8�ֽ�Ϊ��λ
	bool SetSize(bool broaden = true);

	// ��ӿγ�bucket������
	bool AddCourseBucket(unsigned short courseBucketIndex);

	// �Ƴ��γ�bucket������
	bool RemCourseBucket(unsigned short courseBucketIndex);

	// ���ҿγ̴�ŵ�λ�ã����ؿγ̶�Ӧbucket�����������Ҳ�������-1
	int Search(unsigned short courseBucketIndex);

	friend std::ostream& operator <<(std::ostream& out, Student& student);
};

// ���ز�����bucket������������������ϣ����������
int Divisor(int bucket);

// ���������ʱ����
//     std::string����ʱ��ĸ�ʽʾ����
//     "Weeks1-16 Thu Classes1-3" 
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3"
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13" 
//     "Weeks1-16 Thu Classes1-3 Sun Classes1-5 Tue Classes3 Classes11-13 Weeks4-8 Sat Classes2-3"
class AnalyzedTime {
private:
	// 16�ܣ�7�죬ÿλ����1�ڿΣ�ÿ�����13�ڿ�
	bool time[16][7][13];  
public:
	AnalyzedTime();
	AnalyzedTime& operator =(const AnalyzedTime& t);
	bool operator ==(const AnalyzedTime& t);
	AnalyzedTime& operator +=(const AnalyzedTime& t);
	AnalyzedTime& operator -=(const AnalyzedTime& t);

	// �ж�����ʱ���Ƿ��ͻ
	bool IsConflict(const AnalyzedTime& t);

	// ���ʱ�䣬bool[][][]��false
	void ClearTime();

	// ��ӻ����Ƴ�ʱ�䣬�����������ַ����е�ʱ����Ϣ��Ҳ�����ж��ַ����Ƿ�Ϸ�
	bool SetTime(const std::string strTime, bool add = true);
};

// ����ѡ��ϵͳ��ui���£�
// ���� PrintInfo(ѧ��)
//   �� PrintInfo(�γ�)
//   �� SearchCourse
//   �� StudentLoop �������� PrintPickedCourse
//   ��                   �� PickCourse
//   ��                   �� ExitCourse
//   �� AdminLoop ���������� AddCourse 
//   ��                   �� AddStudent
//   ��                   �� RemInfo �������� RemStudentFromCourses
//   ��                                    �� RemCoursesFromStudent
//   �� ��д�ļ�
// ��ϵ������ϣ����װ�û�������������
class CourseSystem {
private:
	HashTable<Course> courseList;
	HashTable<Student> studentList;
public:
	CourseSystem(int courseBucket, int studentBucket)
		: courseList(Divisor(courseBucket), courseBucket), 
		studentList(Divisor(studentBucket), studentBucket) {}

	// ɾ���γ̹�ϣ���е���Ӧѧ������Ϣ������ɾ��ѧ��ǰ���ѧ��ѡ�ι�ϵ
	void RemStudentFromCourses(unsigned studentKey);
	// ɾ��ѧ����ϣ���е���Ӧ�γ̵���Ϣ������ɾ���γ�ǰ���ѧ��ѡ�ι�ϵ
	void RemCourseFromStudents(unsigned courseKey);

	// ����ѧ����ѡ�γ̣�����γ���ռ�õ�ʱ���
	AnalyzedTime TotalTime(unsigned studentKey);

	// ��������ϣ���еǼ�ѧ����ѡ�β���
	bool PickCourseInTable(unsigned studentKey, unsigned courseKey);
	// ��������ϣ���еǼ�ѧ������ѡ����
	bool ExitCourseInTable(unsigned studentKey, unsigned courseKey);

	// ��ʾѧ��������ѡ�Ŀγ���Ϣ
	void PrintPickedCourse(unsigned studentKey);

	// �Ը�ע��Ϊ�ֽ��ߣ����������ĳ�Ա���������ݽṹ�йأ����³�Ա����ʵ�ֵ���ui�Ĺ���

	// ��ʾ���пγ�/ѧ����Ϣ
	void PrintInfo(bool isCourse = true);

	// ��ӿγ���Ϣ����ѡ��ϵͳ��������
	void AddCourse();

	// ���ѧ����Ϣ����ѡ��ϵͳ��������
	void AddStudent();

	// ��ѯ�γ���Ϣ������ʾ����ѡ�ε�ѧ��
	void SearchCourse();

	// ɾ���γ�/ѧ����Ϣ
	void RemInfo(bool isCourse = true);

	// ѧ��ѡ��
	void PickCourse(unsigned studentKey);
	// ѧ����ѡ
	void ExitCourse(unsigned studentKey);

	// ѧ���Ĳ�����ѭ��
	void StudentLoop();

	friend void test5();
};

// ����������״̬����ջ�������ˢ�����������
void ResetIStrm();

// �Ϳγ̹�ϣ���йصĲ���
void test1();

// ��ѧ����ϣ���йصĲ���
void test2();

// ��ʱ�������صĲ���
void test3();

// ��ϵͳ���ɾ����Ϣ�йصĲ���
void test4();

// ��ѧ�������йصĲ���
void test5();
