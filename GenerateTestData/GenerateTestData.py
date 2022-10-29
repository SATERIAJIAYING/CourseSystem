import random
import csv


def main():
    """
    生成用于选课系统的测试数据：
    注意这些数据中学生的选课关系是随机生成的，没有考虑限制学生必须选时间不冲突的课，
    因此导入选课系统中会提示很多选课失败。
    建议生成的数据量：学生数 * 学生平均选课数 = 课程数 * 课程平均学生
    """
    courseNum =  # TODO:设定生成课程的量
    studentNum =  # TODO:设定生成学生的量
    maxSizePerCourse = 150  # 最小课容量5
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
    dayLs = [' Mon', ' Tue', ' Wed', ' Thu', ' Fri', ' Sat', ' Sun']
    weekLs = [' Weeks1-16', ' Weeks4-8', ' Weeks1-12', ' Weeks4-16']
    count = 0
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
                time += random.choice(weekLs) + random.choice(dayLs)
                classBegin = random.randint(1, 11)
                time += " Classes{}-{} ".format(classBegin, classBegin + 2)
            maxSize = random.randint(5, maxSizePerCourse)
            course = [key, name, place, time, maxSize]
            for j in range(maxSize):
                course.append(count % studentNum)
                count += 1
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

