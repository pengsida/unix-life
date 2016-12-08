# -*- coding=utf-8 -*-
from nltk.tokenize import RegexpTokenizer
import re

class solution0012(object):
    def __init__(self,filename):
        fileobj = open(filename)
        filecontent = fileobj.read()
        self.words = RegexpTokenizer('\n',gaps=True).tokenize(filecontent)
        self.patterns = [(re.compile(pattern),len(pattern.decode('utf-8'))*"*") for pattern in self.words]

    def replaceWords(self,word):
        for (pattern,repl) in self.patterns:
            word = re.sub(pattern,repl,word)
        print word

so = solution0012("filtered_words.txt")
while True:
    word = raw_input("Enter your word: ")
    so.replaceWords(word)