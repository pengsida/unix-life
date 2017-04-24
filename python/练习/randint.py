import random

def randomNum(digits,nums):
    for i in range(0,nums):
        num = ""
        for j in range(0,digits):
            num += str(random.randint(0,9))
        print num

def proRand(digits):
    num = ""
    for i in range(0,digits):
        num += str(random.randint(0,9))
    return num

def proSqli(digits,nums):
    num = []
    for i in range(0,nums):
        stri = ""
        for j in range(0,digits):
            stri += str(random.randint(0,9))
        num.append((i,stri))
    return num
