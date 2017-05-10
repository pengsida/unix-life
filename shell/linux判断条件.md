# 一、test命令
```
　　test命令是shell内部命令，它计算作为其参数的表达式是真还是假。

　　如果表达式为真，则test返回0（注意与C语言0表示false不同），如果表达式为假，返回非0。

　　shell提供了另外一个调用test命令的方式，就是用方括号［］来代替单词test。

　　调用test命令的格式为：

　　　　test expr1 opr expr2 或是 [ expr1 opr expr2 ]

　　　　test opr expr1 或是 [ opr expr1 ]

　　（注意方括号与表达式之间的空格必须存在）
```

if-then语句的格式如下所示：
```bash
if command
then
    commands
fi
```

if-then语句的另一种形式为：
```bash
if command; then
    commands
fi
```

# 二、逻辑运算符
```
　　-a　　expr1 -a expr2　　逻辑与

　　-o　　expr1 -o expr2　　逻辑或

　　！　　!expr1　　　　　　逻辑非
```

# 三、数值判断
```

　　-eq　　num1 -eq num2　　是否相等

　　-ne　　num1 -ne num2　　是否不相等

　　-gt　　num1 -gt num2　　是否大于

　　-ge　　num1 -ge num2　　是否大于等于

　　-lt　　num1 -lt num2　　是否小于

　　-le　　num1 -le num2　　是否小于等于
```

# 四、字符串判断
```
　　=　　str1 = str2　　字符串是否相等

　　!=　　str1 != str2　　字符串是否不等

　　-n　　-n str1　　　　字符串长度是否不等于0

　　-z　　-z str2　　　　字符串长度是否等于0
```

# 五、文件判断
```
　　-r　　-r filename　　文件是否存在且可读

　　-w　　-w filename　　文件是否存在且可写

　　-s　　-s filename　　文件是否存在且长度非0

　　-f　　-f filename　　文件是否存在且是普通文件

　　-d　　-d filename　　文件是否存在且是一个目录
```