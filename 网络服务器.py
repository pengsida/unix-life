本章目标：学习如何编写网络服务器

3.1 准备连接
{
    对于服务器，建立一个TCP连接的过程分为四步
    {
        1.建立socket对象
        2.设置socket选项
        3.绑定到一个端口
        4.侦听连接

        例子
        {
            host = ''
            port = 51423
            
            # 建立socket对象
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # 设置socket选项
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            # 绑定到一个端口
            s.bind((host, port))
            # 侦听连接
            s.listen(5)
        }

        3.1.1 建立socket对象
        {
            代码和客户端的一样
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        }

        3.1.2 设置和得到socket选项
        {
            如果设置SO_REUSEADDR的标记为true，操作系统就会在服务器socket被关闭或服务器进程终止后马上释放该服务器的端口

            SOL_SOCKET常用到的选项
            {
                1.SO_BINDTODEVICE
                2.SO_BROADCAST
                3.SO_DONTROUTE
                4.SO_KEEPALIVE
                5.SO_OOBINLINE
                6.SO_REUSEADDR
                7.SO_TYPE
            }

            python程序可以给出python所支持的socket选项列表
            {
                import socket
                solist = [x for x in dir(socket) if x.startswith('SO_')]
                solist.sort()
                for x in solist:
                    print x
            }
        }

        3.1.3 绑定socket
        {
            为服务器要求一个端口号，这个过程称为绑定

            代码
            s.bind(('', 80))
            这条指令请求80端口，它是标准的HTTP(Web)端口
        }

        3.1.4 侦听连接
        {
            listen()函数的参数指明了服务器实际处理连接的时候，允许有多少个未决的连接在队列中等待
        }
    }
}

3.2 接受连接
{
    大多数服务器都设计成运行不确定长的时间和同时服务于多个连接
    通常使服务器连续运行的办法来小心地设计一个无限循环
    例子
    {
        import socket

        host = ''
        port = 51423

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))
        print "Waiting for connections..."
        s.listen(1)

        # 它虽然是无限循环的，但当它调用accept()的时候，它只在有一个客户端连接后才返回。这时候，这个程序停止的，并不适用任何的CPU资源
        # 一个停止并等待输入或输出的程序称为被阻塞的程序
        while 1:
            clientsock, clientaddr = s.accept()
            print "Got connection from", clientsock.getpeername()
            clientsock.close()
    }
}

3.3 处理错误
{
    服务器总是应该考虑错误处理
    书中建议放置一个普通的错误处理来确保任何错误都不会从故障中漏掉
    一个错误处理就是一个简单的、标准的python异常处理，它可以处理网络相关的问题

    例子，errorserver.py
    {
        import socket, traceback

        host = ''
        port = 51423

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))
        s.listen(1)

        while 1:
            # 这个用于捕捉accept()的调用
            try:
                clientsock, clientaddr = s.accept()
            except KeyboardInterrupt:
                raise
            except:
                traceback.print_exc()
                continue
            
            try:
                print "Got connection from", clientsock.getpeername()
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                traceback.print_exc()
                
            try:
                clientsock.close()
            except KeyboardInterrupt:
                raise
            except:
                traceback.print_exc()
    }
}

3.4 使用UDP
{
    在服务器端使用UDP时不用使用listen()或accept()函数，仅仅使用recvfrom()函数就可以
    recvfrom()函数实际上会返回两个信息：收到的数据以及发送这些数据的程序地址和端口号
    因为UDP是无连接的协议，所以仅需要发送一个答复就行

    例子，简单的UDP应答服务器
    {
        import socket, traceback

        host = ''
        port = 51423

        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))

        while 1:
            try:
                message, address = s.recvfrom(8192)
                print "Got data from", address
                s.sendto(message, address)
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                traceback.print_exc()
    }

    例子，一个UDP网络时间服务器端的程序
    {
        import socket, traceback, time, struct

        host = ''
        port = 51423

        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))

        while 1:
            try:
                message, address = s.recvfromt(8192)
                secs = int(time.time())
                secs -= 60*60*24
                secs += 2208988800
                reply = struct.pack("!T", secs)
                s.sendto(reply, address)
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                traceback.print_exc()
    }
}