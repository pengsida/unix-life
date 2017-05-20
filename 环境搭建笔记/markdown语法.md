[markdown语法大全](https://segmentfault.com/markdown)

# 一级标题

## 二级标题

### 三级标题

##### 列表

1.
2.
3.

无序序列：

- 内容
- 
- 
或
* 内容
* 
*

> 这是引用

#### 插入链接

[baidu](http://baidu.com)

![图片](https://github.com/wwtalwtaw/markdown-photos/raw/master/markdown.jpg)

 <center><img src="https://github.com/wwtalwtaw/markdown-photos/raw/master/markdown.jpg" width = "300" height = "200" alt="图片"/></center>

**包含一段文本就是粗体的语法**

*包含一段文本就是斜体的语法*

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |


---
分割线


插入代码
```python
# -*- coding: utf-8 -*-
from pylab import *

x = range(10, 90, 10)
y = range(10, 90, 10)

plot(x, y)
xlabel("x-axis")
ylabel("y-axis")
show()
```

<center>放到中间</center>

公式：
$$ x = {-b \pm \sqrt{b^2-4ac} \over 2a}. $$

$$
x \href{why-equal.html}{=} y^2 + 1
$$

显示html标签：只要变迁两边加`
```
`<h1>`
```

设置锚点：

[Inline HTML](#html)

<h3 id="html">Inline HTML</h3>