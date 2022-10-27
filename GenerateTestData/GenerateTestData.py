import random
import csv


def main():
    courseNum = 20000
    studentNum = 1000000
    maxSizePerCourse = 150
    courseFileName = 'COURSE_DATA.csv'
    studentFileName = 'STUDENT_DATA.csv'

    generate_course_data(courseNum, studentNum, maxSizePerCourse, courseFileName)
    generate_student_data(studentNum, studentFileName)


def gb2312():
    """
    随机生成GB2312编码汉字
    """
    head = random.randint(0xb0, 0xf7)
    body = random.randint(0xa1, 0xf9)
    val = f'{head:x} {body:x}'
    ch = bytes.fromhex(val).decode('gb2312')
    return ch


def generate_course_data(courseNum, studentNum, maxSizePerCourse, courseFileName):
    dayLs = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    with open(courseFileName, 'w', encoding='gb2312') as f:
        writer = csv.writer(f)
        for i in range(courseNum):
            key = i * 100 + random.randint(0, 99)
            n = random.randint(2, 20)
            name = "".join([gb2312() for j in range(n)])
            n = random.randint(2, 15)
            place = "".join([gb2312() for j in range(n)])
            n = random.randint(1, 4)
            time = ""
            for j in range(n):
                nLs = sorted([random.randint(1, 16) for k in range(2)])
                time += "Weeks{}-{} ".format(nLs[0], nLs[1]) + random.choice(dayLs)
                nLs = sorted([random.randint(1, 13) for k in range(2)])
                time += " Classes{}-{} ".format(nLs[0], nLs[1])
            maxSize = random.randint(5, maxSizePerCourse)
            course = [key, name, place, time, maxSize]
            n = random.randint(5, maxSize)
            for j in range(n):
                course.append(random.randint(0, studentNum - 1))
            writer.writerow(course)


def generate_student_data(studentNum, studentFileName):
    with open(studentFileName, 'w', encoding='gb2312') as f:
        writer = csv.writer(f)
        for i in range(studentNum):
            key = i
            n = random.randint(2, 4)
            name = "".join([gb2312() for j in range(n)])
            n = random.randint(8, 9)
            NO = "".join([str(random.randint(0, 9)) for j in range(n)])
            writer.writerow([key, name, NO])


if __name__ == '__main__':
    main()

