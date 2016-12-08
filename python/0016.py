# -*- coding=utf-8 -*-

import re
from xlwt import Workbook
from xlwt import Worksheet

class solution0016(object):
    def __init__(self,filename):
        fileobj = open(filename)
        self.content = fileobj.read()
        self.pattern = re.compile('\[(.*), (.*), (.*)\]')
        self.data = []

    def get_data(self):
        self.data = re.findall(self.pattern,self.content)

    def write_data(self):
        book = Workbook(encoding='utf-8')
        sheet = book.add_sheet('0016')
        row = 0
        for line in self.data:
            for col in range(len(line)):
                sheet.write(row,col,line[col])
            row += 1
        book.save("numbers.xls")

    def show(self):
        print self.content
        print self.data

so = solution0016("numbers.txt")
so.get_data()
so.write_data()
so.show()