# -*- coding: utf-8 -*-
from nltk.tokenize import RegexpTokenizer
import re

class soluiton0011(object):
    def __init__(self,filename):
        self.fileobj = open(filename)
        self.fileContent = self.fileobj.read()
        # 正则表达式的使用
        # self.fileContent = "sd ds sd d's"
        # tokenizer = RegexpTokenizer("[d's]+")
        tokenizer = RegexpTokenizer("\n",gaps=True)
        self.words = tokenizer.tokenize(self.fileContent)
        self.patterns = [re.compile(word) for word in self.words]

    def filterWord(self,word):
        for pattern in self.patterns:
            matchObj = re.search(pattern,word)
            if matchObj is not None:
                print "Freedom"
                return None
        print "Human Rights"

    def show(self):
        print self.patterns

so = soluiton0011('filtered_words.txt')
while True:
    word = raw_input("Enter your input: ")
    so.filterWord(word)