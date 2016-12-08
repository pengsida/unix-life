class solution4(object):
    def __init__(self, nums1, nums2):
        self.nums1 = nums1
        self.nums2 = nums2
        self.nums1_len = len(nums1)
        self.nums2_len = len(nums2)
        self.total_len = self.nums1_len + self.nums2_len
        self.result = 0

    def process(self, k):
        start_1 = 0
        start_2 = 0
        while True:
            if start_1 >= self.nums1_len:
                return self.nums2[start_2+k-1]
            if start_2 >= self.nums2_len:
                return self.nums1[start_1+k-1]
            if k == 1:
                return min(self.nums1[start_1], self.nums2[start_2])
            part1_k = min(k/2, self.nums1_len - start_1)
            index1 = part1_k + start_1 - 1
            index2 = k - part1_k + start_2 - 1
            if self.nums1[index1] > self.nums2[index2]:
                start_2 = index2 + 1
                k = part1_k
            elif self.nums1[index1] < self.nums2[index2]:
                start_1 = index1 + 1
                k -= part1_k
            else:
                return self.nums1[index1]

    def get_result(self):
        if self.total_len % 2 != 0:
            self.result = self.process(self.total_len / 2 + 1)
        else:
            self.result = (self.process(self.total_len / 2) + self.process(self.total_len / 2 + 1)) * 1.0 / 2

    def show(self):
        print self.result


nums1 = [1,2]
nums2 = [3,4]

so = solution4(nums1,nums2)
so.get_result()
so.show()