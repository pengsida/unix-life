#coding=utf-8
import sys
import MySQLdb
import randint

# # 连接数据库
# conn = MySQLdb.connect(
#     host = 'localhost',
#     port = 3306,
#     user = 'root',
#     db = 'test',
# )
# # 想要操作数据库需要创建游标
# cur = conn.cursor()
# # 创建数据表
# # cur.execute("create table student(id int, name varchar(20),class varchar(30),age varchar(10))")
# # 插入数据
# # 方式1
# # cur.execute("insert into student values('2','Tom','3 year 2 class','9')")
# # 方式2
# sqli = "insert into student values(%s,%s,%s,%s)"
# cur.execute(sqli,('3','Huhu','2 year 1 class','7'))
# # 查询数据
# # 查询表中有多少条数据
# aa = cur.execute("select * from student")
# # 获取表中的数据,从表头开始获取
# a = cur.fetchone()
# # 将游标定位到表中的第一条数据
# cur.scroll(0,'absolute')
# # 一次性获取表中所有数据,以dic的数据类型返回
# cur.fetchmany()
# print a
# # 修改查询条件的数据
# cur.execute("update student set class='3 year 1 class' where name = 'Tom'")
# # 删除查询条件的数据
# cur.execute("delete from student where age = '9'")
#
# # 关闭数据库
# cur.close()
# conn.commit()
# conn.close()

# conn = MySQLdb.connect(
#     db = 'test',
# )
# # 想要操作数据库需要创建游标
# cur = conn.cursor()
# # 创建数据表
# cur.execute("create table table_psd(id int, num varchar(20))")
# # 插入数据
# # 方式1
# # cur.execute("insert into student values('2','Tom','3 year 2 class','9')")
# # 方式2
# sqli = "insert into table_psd values(%s,%s)"
# # for i in range(0,200):
# #     num = randint.proRand(3)
# #     cur.execute(sqli,(i,num))
# num = randint.proSqli(3,5)
# cur.executemany(sqli,num)
# nums = cur.execute("select * from table_psd")
# # 获取表中的数据,从表头开始获取
# a = cur.fetchmany(nums)
# for aa in a:
#    print aa
#
# cur.execute("drop table table_psd")
#
# # 关闭数据库
# cur.close()
# conn.commit()
# conn.close()

def proRandTable(digits,nums):
    dataTable = MySQLdb.connect(db='test')
    cur = dataTable.cursor()
    cur.execute("show tables")
    tablerows = cur.fetchall()
    flag = 0
    for table in tablerows:
        if flag == 1:
            break
        for ta in table:
            if ta == 'randTable':
                flag = 1
                break
    if flag == 1:
        cur.execute("drop table randTable")
    cur.execute("create table if not exists randTable(id int, randNum varchar(10))")
    sqli = "insert into randTable values(%s,%s)"
    randNum = randint.proSqli(3,5)
    cur.executemany(sqli,randNum)
    nums = cur.execute("select * from randTable")
    a = cur.fetchmany(nums)
    for aa in a:
        print aa
    cur.execute("drop table randTable")
    cur.close()
    dataTable.commit()
    dataTable.close()

proRandTable(3,3)