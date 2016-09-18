1.1 理解TCP基础
{
    1.1.1 寻址
    {
        1.TCP要识别远程的机器
        2.TCP需要知道是与远程机器上的哪个程序通信
    }

    1.1.2 可靠性
    {
        1.每个信息包都包含一个校验码
        2.TCP要求接收方每收到一个信息包都反馈一下
        3.TCP没传送一个信息包都会传送一个序号
    }

    1.1.3 路由
    {
        在Internet上负责接收信息包并决定如何把它们传输到目的地的设备叫路由器
    }

    1.1.4 安全
    {
        1.SSL
        2.TLS
    }
}

1.2 使用客户/服务器模式
{
    1.2.1 服务器端端口号
    {
        mac下/etc/services下有端口列表
    }

    1.2.2 客户端端口号
    {
        1.客户端会有操作系统随机挑选一个端口号
    }
}

1.3 理解UDP
{
    1.不能保证数据是否真的能被收到
    2.不能保证数据是不是只接收一次
    3.不能保证收到的信息次序是否和发送时候一致
}

1.4 理解物理传输和以太网
{
    1.可以在不同的物理网络硬件之间传送数据
    2.以太网可以向本地网络所有的工作站广播信息包
}

1.5 Python网络编程
{
    1.5.1 底层接口
    {
        1.5.1.1 基本客户端操作
        {
            import socket, sys
            port = 70
            # 程序需要两个命令行参数：主机名和文件名
            host = sys.argv[1]
            filename = sys.argv[2]

            # 通过调用socket.socket()来建立一个Socket
            # 参数告诉系统需要一个Internet socket来进行TCP通信
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((host, post))
            s.sendall(filename + "\r\n")

            while 1:
                buf = s.recv(2048)
                # 获得相应后，在屏幕上打印出来
                if not len(buf):
                    break
                # 打印出Gopher服务器根目录的文件列表
                sys.stdout.write(buf)
        }

        1.5.1.2 错误和异常
        {
            上面的例子没有错误检查。
            当给出一个不存在的主机名时，我们应该在程序中特殊处理这个异常，如下

            try:
                s.connect((host, port))
            except socket.gaierror, e:
                print "Error connecting to server: %s" % e
                sys.exit(1)
        }

        1.5.1.3 文件类对象
        {
            用文件类接口重写

            import socket, sys
            port = 70
            host = sys.argv[1]
            filename = sys.argv[2]
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((host, port))
            # 使用makefile()函数来生成文件类对象
            fd = s.makefile('rw', 0)
            fd.write(filename + "\r\n")
            for line in fd.readlines():
                sys.stdout.write(line)
            fd.close()
        }

        1.5.1.4 基本服务器操作
        {
            用python编写服务器程序

            import socket

            host = ''
            port = 51423

            # 调用socket.socket()函数来建立一个socket
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # 把socket设置成可复用的(reusable)
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
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
                clientfile.write("Welcome, " + str(clientaddr) + "\n")
                clientfile.write("Please enter a string: ")
                # 从客户端读一个字符串，显示一个应答
                line = clientfile.readline().strip()
                clientfile.write("You entered %d characters.\n" % len(line))
                # 关闭文件对象和socket对象
                clientfile.close()
                clientsock.close()
        }
    }

    1.5.2 高级接口
    {
        python有很多协议模块
        
        例子，使用高级模块实现与Gopher服务器的通信
        {
            import gopherlib, sys

            host = sys.argv[1]
            file = sys.argv[2]

            # 让gopherlib模块负责建立socket和连接
            f = gopherlib.send_selector(file, host)
            for line in f.readlines():
                sys.stdout.write(line)
        }

        例子，python使用高级模块处理URL
        {
            import urllib, sys

            host = sys.argv[1]
            file = sys.argv[2]

            f = urllib.urlopen('gopher://%s%s' % (host, file))
            for line in f.readlines():
                sys.stdout.write(line)
        }

        例子，使用urllib模块实现多种文件下载程序
        {
            import urllib, sys

            f = urllib.urlopen(sys.argv[1])
            while 1:
                buf = f.read(2048)
                if not len(buf):
                    break
                sys.stdout.write(buf)
        }
    }
}