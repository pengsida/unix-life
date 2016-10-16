#include "apue.h"
#include <setjmp.h>

#define TOK_ADD 5

jmp_buf jmpbuffer; // jmpbuffer存放着在调用longjmp时能用来恢复栈状态的所有信息

void do_line(char*);
void cmd_add(void);
int get_token(void);

int main()
{
    char line[MAXLINE];
    if(setjmp(jmpbuffer) != 0) // 在希望返回到的位置调用setjmp
        printf("error");
    while(fgets(line, MAXLINE, stdin) != NULL) // 从标准输入中读入一行
        do_line(line);
    exit(0);
}

char *tok_ptr;

void do_line(char *ptr)
{
    int cmd;
    tok_ptr = ptr;
    while((cmd = get_token()) > 0) // 调用get_token从该输入行中取下一个标记
    {
        switch(cmd)
        {
            case TOK_ADD:
                cmd_add();
                break;
        }
    }
}

void cmd_add(void)
{
    int token;
    token = get_token();
    if(token < 0)
        longjmp(jmpbuffer, 1); // 第二个参数将成为setjmp处返回的值
    // rest of processiing for this command
}

int get_token(void)
{
    // fetch next token from line pointed to by tok_ptr
}