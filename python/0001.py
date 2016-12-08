import random

def randomNum(digits,nums):
    for i in range(0,nums):
        num = ""
        for j in range(0,digits):
            num += str(random.randint(0,9))
        print num

