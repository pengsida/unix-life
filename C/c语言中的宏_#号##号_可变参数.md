# c语言中的宏 #号##号 可变参数
C（和C++）中的宏（Macro）属于编译器预处理的范畴，属于编译期概念（而非运行期概念）。

下面对常遇到的宏的使用问题做了简单总结。 

# 关于#和## 

## #符号
在C语言的宏中，#的功能是将其后面的宏参数进行字符串化操作（Stringfication），简单说就是在对它所引用的宏变量通过替换后在其左右各加上一个双引号。比如下面代码中的宏： 

```c
#define WARN_IF(EXP) \ 
do{ \
    if (EXP) \ 
        fprintf(stderr, "Warning: " #EXP "\n"); } \ 
while(0) 
```

那么实际使用中会出现下面所示的替换过程： 
```c
WARN_IF (divider == 0); 
```

被替换为 

```c
do 
{ 
    if (divider == 0) 
        fprintf(stderr, "Warning" "divider == 0" "\n"); 
} while(0); 
```

这样每次divider（除数）为0的时候便会在标准错误流上输出一个提示信息。 

## ##符号
而##被称为连接符（concatenator），用来将两个Token连接为一个Token。注意这里连接的对象是Token就行，而不一定是宏的变量。比如你要做一个菜单项命令名和函数指针组成的结构体的数组，并且希望在函数名和菜单项命令名之间有直观的、名字上的关系。那么下面的代码就非常实用：
```c
struct command 
{ 
    char * name; 
    void (*function) (void); 
}; 

#define COMMAND(NAME) { NAME, NAME##_command } 
```

然后你就用一些预先定义好的命令来方便的初始化一个command结构的数组了： 

```c
struct command commands[] = 
{ 
    COMMAND(quit), 
    COMMAND(help), 
}
``` 

COMMAND宏在这里充当一个代码生成器的作用，这样可以在一定程度上减少代码密度，间接地也可以减少不留心所造成的错误。我们还可以n个##符号连接 n+1个Token，这个特性也是#符号所不具备的。比如： 
```c
#define LINK_MULTIPLE(a,b,c,d) a##_##b##_##c##_##d 

typedef struct _record_type LINK_MULTIPLE(name,company,position,salary); 
```

这里这个语句将展开为： 
```c

typedef struct _record_type name_company_position_salary; 
```

# 关于...的使用 

...在C宏中称为Variadic Macro，也就是变参宏。比如： 
```c

#define myprintf(templt,...) fprintf(stderr,templt,__VA_ARGS__) 
```
或者 
```c
#define myprintf(templt,args...) fprintf(stderr,templt,args) 
```

第一个宏中由于没有对变参起名，我们用默认的宏__VA_ARGS__来替代它。第二个宏中，我们显式地命名变参为args，那么我们在宏定义中就可以用 args来代指变参了。同C语言的stdcall一样，变参必须作为参数表的最有一项出现。当上面的宏中我们只能提供第一个参数templt时，C标准要求我们必须写成： 
```c

myprintf(templt,); 
```

的形式。这时的替换过程为： 
```c
myprintf("Error!\n",); 
```
替换为： 
```c
fprintf(stderr,"Error!\n",); 
```
这是一个语法错误，不能正常编译。这个问题一般有两个解决方法。首先，GNU CPP提供的解决方法允许上面的宏调用写成： 
```c
myprintf(templt); 
```
而它将会被通过替换变成： 
```c
fprintf(stderr,"Error!\n",); 
```
很明显，这里仍然会产生编译错误（非本例的某些情况下不会产生编译错误）。除了这种方式外，c99和GNU CPP都支持下面的宏定义方式： 
```c
#define myprintf(templt, ...) fprintf(stderr,templt, ##__VAR_ARGS__) 
```
这时，##这个连接符号充当的作用就是当__VAR_ARGS__为空的时候，消除前面的那个逗号。那么此时的翻译过程如下： 
```c
myprintf(templt); 
```
被转化为： 
```c
fprintf(stderr,templt); 
```
这样如果templt合法，将不会产生编译错误。 