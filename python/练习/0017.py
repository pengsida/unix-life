# -*- coding=utf-8 -*-
from xlrd import open_workbook
import xml.dom.minidom as dom
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

# from lxml import etree
# from xlrd import open_workbook
# from xlrd import cellname
#
# class solution0017(object):
#     def __init__(self,filename):
#         self.book = open_workbook(filename=filename)
#         self.sheet = self.book.sheet_by_index(0)
#         self.content = ""
#
#     def get_content(self):
#         self.content += '\n\t{'
#         for row in range(self.sheet.nrows - 1):
#             self.content += '\n\t'
#             line = '"%s" : ["%s", %s, %s, %s]' % (self.sheet.cell(row,0).value,self.sheet.cell(row,1).value,self.sheet.cell(row,2).value,self.sheet.cell(row,3).value,self.sheet.cell(row,4).value)
#             self.content += line
#             self.content += ','
#         self.content += '\n\t'
#         line = '"%s" : ["%s", %s, %s, %s]' % (self.sheet.cell(row,0).value,self.sheet.cell(row,1).value,self.sheet.cell(row,2).value,self.sheet.cell(row,3).value,self.sheet.cell(row,4).value)
#         self.content += line
#         self.content += '\n\t}\n'
#
#     def buildXML(self):
#         root = etree.Element('root')
#         student_root = etree.ElementTree(root)
#         student = etree.SubElement(root,'student')
#         student.text = self.content
#         student.append(etree.Comment('\n\t学生信息表\n\t"id" : [名字, 数学, 语文, 英文]\n'.decode('utf-8')))
#         print etree.tostring(root,pretty_print=True,xml_declaration=True,encoding='UTF-8')
#         file = open('student.xml','w')
#         file.write(etree.tostring(root,xml_declaration=True,encoding='utf-8'))
#
#     def show(self):
#         data = []
#         for row in range(self.sheet.nrows):
#             for col in range(self.sheet.ncols):
#                 data.append(self.sheet.cell(row,col).value)
#
# so = solution0017('student.xls')
# so.get_content()
# so.buildXML()

class solution0017(object):
    def __init__(self,filename):
        self.book = open_workbook(filename=filename)
        self.sheet = self.book.sheet_by_index(0)
        self.content = ""

    def get_content(self):
        self.content += "{"
        for row in range(self.sheet.nrows):
            # self.content.append(tuple(self.sheet.row_values(row)))
            self.content += "\n\t\t\t"
            self.content += '"%s" : ["%s", %s, %s, %s]' % tuple(self.sheet.row_values(row))
            self.content += ','
        self.content = self.content[:len(self.content)-1]
        self.content += "\n\t\t}"

    def build_xml(self):
        doc = dom.Document()
        root = doc.createElement('root')
        doc.appendChild(root)
        students = doc.createElement('students')
        root.appendChild(students)
        students_comment = doc.createComment('学生信息表 "id" : [名字, 数学, 语文, 英文]')
        students_content = doc.createTextNode(self.content)
        students.appendChild(students_comment)
        students.appendChild(students_content)

        file = open('student.xml','w')
        file.write(doc.toprettyxml(encoding='UTF-8').replace('&quot;','"'))
        print doc.toprettyxml(encoding='UTF-8').replace('&quot;','"')


so = solution0017('student.xls')
so.get_content()
so.build_xml()