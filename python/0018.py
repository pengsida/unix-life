# -*- coding=utf-8 -*-
from xlrd import open_workbook
import xml.dom.minidom as dom
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

class solution0018(object):
    def __init__(self,filename):
        self.book = open_workbook(filename=filename)
        self.sheet = self.book.sheet_by_index(0)
        self.content = ""

    def get_content(self):
        self.content += "{"
        for row in range(self.sheet.nrows):
            # self.content.append(tuple(self.sheet.row_values(row)))
            self.content += "\n\t\t\t"
            self.content += '"%s" : "%s"' % tuple(self.sheet.row_values(row))
            self.content += ','
        self.content = self.content[:len(self.content)-1]
        self.content += "\n\t\t}"

    def build_xml(self):
        doc = dom.Document()
        root = doc.createElement('root')
        doc.appendChild(root)
        students = doc.createElement('citys')
        root.appendChild(students)
        students_comment = doc.createComment('城市信息')
        students_content = doc.createTextNode(self.content)
        students.appendChild(students_comment)
        students.appendChild(students_content)

        file = open('citys.xml','w')
        file.write(doc.toprettyxml(encoding='UTF-8').replace('&quot;','"'))
        print doc.toprettyxml(encoding='UTF-8').replace('&quot;','"')


so = solution0018('citys.xls')
so.get_content()
so.build_xml()