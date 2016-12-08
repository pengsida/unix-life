import re
import string

class solution2(object):
    def __init__(self, input):
        # self.input = raw_input()
        self.input = input
        self.list = self.input.split('+')
        self.num_lists = [re.findall(re.compile('\d'), num) for num in self.list]
        self.nums = []
        self.result = '{result}'

    def get_num(self):
        for num_list in self.num_lists:
            if len(num_list) != 0:
                num = ''.join(num_list)
                self.nums.append(num[::-1])

    def get_result(self):
        re = 0
        for num in self.nums:
            re += string.atoi(num)
        self.result = str(re)[::-1]
        self.result = [r for r in self.result]
        self.result = ' -> '.join(self.result)

    def show(self):
        print self.result

so = solution2("(2 -> 4 -> 3) + (5 -> 6 -> 4) + (0 0 0 1)")
so.get_num()
so.get_result()
so.show()