class solution1(object):
    def __init__(self, list, target):
        self.num_list = list
        self.target = target
        self.result = []

    def get_result(self):
        for i in range(len(self.num_list)):
            if self.num_list[i] > self.target:
                continue
            for j in range(i+1,len(self.num_list)):
                if self.num_list[i] + self.num_list[j] == self.target:
                    self.result.append((i,j))

    def show_result(self):
        for re in self.result:
            print re

list = [2,7,11,15,4,5]
target = 9
so = solution1(list,target)
so.get_result()
so.show_result()