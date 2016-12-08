import os

class countLines(object):
    def __init__(self,dirname):
        self.dirname = dirname
        self.filenames = []
        self.codeLines = 0
        self.spaceLines = 0
        self.noteLines = 0
        self.countFilenames(self.dirname)

    def countFilenames(self,dirname):
        for filename in os.listdir(dirname):
            self.filenames.extend([dirname + '/' +filename])
            if os.path.isdir(dirname + '/' + filename):
                self.countFilenames(dirname + '/' + filename)

    def openFIle(self,filename):
        self.filename = filename
        self.fileObject = open(filename)

    def countLine(self):
        for line in self.fileObject:
            # if self.filename == '0007.py':
            #     print len(line)
            if len(line) == 1:
                self.spaceLines += 1
            elif '#' in line or '//' in line:
                self.noteLines += 1
            else:
                self.codeLines += 1

    def count(self):
        for filename in self.filenames:
            if filename.split('.')[-1] in 'py c cpp m':
                self.openFIle(filename)
                self.countLine()
        print self.codeLines
        print self.spaceLines
        print self.noteLines

test = countLines(os.getcwd())
test.count()
