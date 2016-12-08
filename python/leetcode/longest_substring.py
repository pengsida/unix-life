class solution3(object):
    def __init__(self, s):
        self.stack = []
        self.max_size = 0
        self.string = s

    def get_result(self):
        num = 0
        for s in self.string:
            if s in self.stack:
                if num > self.max_size:
                    self.max_size = num
                del self.stack[:]
                num = 0
                continue
            self.stack.append(s)
            num += 1

    def show(self):
        print self.max_size

so = solution3("pwwkew")
so.get_result()
so.show()