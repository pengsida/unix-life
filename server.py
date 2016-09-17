import socket

host = ''
port = 51423

# 调用socket.socket()函数来建立一个socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 把socket设置成可复用的(reusable)
s.setsocket(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# 把主机设置成空字符串，程序这样可以接受来自任意地方的连接
# 把端口设置成51423
s.bind((host, port))
# 调用listen()函数，等候来自客户端的连接，同时设定最多只有一个等候处理的连接
s.listen(1)

print "Server is running on port %d; press Ctrl-C to terminate." \
    % port
while 1:
    # accept()函数返回两个信息：一个新的连接客户端的socket和客户端的IP地址、端口号
    clientsock, clientaddr = s.accept()
    # 使用文件类对象
    clientfile = clientsock.makefile('rw', 0)
    clientfile.write("Welcom, " + str(clientaddr) + "\n")
    clientfile.write("Please enter a string: ")
    # 从客户端读一个字符串，显示一个应答
    line = clientfile.readline().strip()
    clientfile.write("You entered %d characters.\n" % len(line))
    # 关闭文件对象和socket对象
    clientfile.close()
    clientsock.close()