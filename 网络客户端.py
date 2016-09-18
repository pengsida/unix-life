2.1 理解socket
{
    python通过socket模块提供访问操作系统socket库的接口
}

2.2 建立socket
{
    建立socket的步骤
    {
        1.建立一个实际的socket对象
        {
            此时需要告诉系统两件事：通信类型和协议家族
            通信类型基本上是AF_INET
            协议家族一般表示TCP通信的SOCK_STREAM

            建立一个socket连接，一般用如下代码：
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        }
        2.把socket对象连接到远程服务器上
        {
            连接socket一般需要提供一个元组，其中包含远程主机名和远程端口
            一般用如下代码：
            s.connect(("www.example.com", 80))
        }
        
        一个完整的例子
        {
            import socket

            print "Creating socket..."
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print "done."

            print "Connecting to remote host..."
            s.connect(("www.google.com", 80))
            print "done."
        }
    }

    2.2.1 寻找端口号
    {
        python的socket库包含一个getservbyname()的函数，可以用于自动查询端口号列表
        为了查询这个列表，需要两个参数：协议名和端口名。
        端口名是一个字符串，这样可以被转换成一个端口号

        例子
        {
            import socket

            print "Creating socket..."
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print "done."

            print "Looking up port number..."
            port = socket.getservbyname('http', 'tcp')
            print "done."

            print "Connecting to remote host on post %d..." % port
            s.connect(("www.google.com", port))
            print "done."
        }
    }

    2.2.2 从socket获取信息
    {
        例子
        {
            import socket

            print "Creating socket..."
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print "done."

            print "Looking up port number..."
            port = socket.getservbyname('http', 'tcp')
            print "done."

            print "Connecting to remote host on port %d..." % port
            s.connect(("www.google.com", port))

            # 显示本身的IP地址和端口号
            print "Connected from", s.getsockname()
            # 显示远程机器的IP地址和端口号
            print "Connected to", s.getpeername()
        }
    }
}

2.3 利用socket通信
{
    可以利用socket发送和接受数据
    python有socket对象和文件类对象
}

2.4 处理错误
{
    2.4.1 socket异常
    {
        import socket, sys

        host = sys.argv[1]
        textport = sys.argv[2]
        filename = sys.argv[3]

        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 与一般I/O和通信问题有关的socket.error
        except socket.error, e:
            print "Strange error creating socket: %s" % e
            sys.exit(1)
        
        try:
            port = int(textport)
        except ValueError:
            try:
                port = socket.getservbyname(textport, 'tcp')
            except socket.error, e:
                print "Couldn't find your port: %s" % e
                sys.exit(1)
        
        try:
            s.connect((host, port))
        # 与查询地址信息有关的socket.gaierror
        except socket.gaierror, e:
            print "Address-related error connecting to server: %s" % e
            sys.exit(1)
        except socket.error, e:
            print "Connection error: %s" % e
            sys.exit(1)
        
        try:
            s.sendall("GET %s HTTP/1.0\r\n\r\n" % filename)
        except socket.error, e:
            print "Error sending data: %s" % e
            sys.exit(1)
        
        while 1:
            try:
                buf = s.recv(2048)
            except socket.error, e:
                print "Error receiving data: %s" % e
                sys.exit(1)
            if not len(buf):
                break
            sys.stdout.write(buf)
    }

    2.4.2 遗漏的错误
    {
        有时候通信出了问题，但没有产生异常，因为没有从操作系统传回错误

        例子
        {
            import socket, sys, time

            host = sys.argv[1]
            textport = sys.argv[2]
            filename = sys.argv[3]

            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            except socket.error, e:
                print "Strange error creating socket: %s" % e
            
            try:
                port = int(textport)
            except ValueError:
                try:
                    port = socket.getservbyname(textport, 'tcp')
                except socket.error, e:
                    print "Couldn't find your port: %s' % e"
                    sys.exit(1)
                
            try:
                s.connect((host, port))
            except socket.gaierror, e:
                print "Address-related error connecting to server: %s" % e
                sys.exit(1)
            except socket.error, e:
                print "Connection error: %s" % e
                sys.exit(1)
            
            print "sleeping..."
            time.sleep(10)
            print "Continuing."

            try:
                s.sendall("GEY %s HTTP/1.0\r\n\r\n" % filename)
            except socket.error, e:
                print "Error sending data (detected by shutdown): %s" % e
                sys.exit(1)
            
            try:
                # 上面有提到，有时候通信出了问题，但是却没有产生异常，因为没有从操作系统传回错误
                # 有时候在网络上发送数据的调用会在远程服务器确保已经收到信息之前返回
                # 为了解决这个问题，应该调用shutdown()函数。这样可以强制清除缓存里面的内容，同时如果有任何问题就会产生一个异常
                s.shutdown(1)
            except socket.error, e:
                print "Error sending data (detected by shutdown): %s" % e
                sys.exit(1)
            
            while 1:
                try:
                    buf = s.recv(2048)
                except socket.error, e:
                    print "Error receiving data: %s" % e
                    sys.exit(1)
                if not len(buf):
                    break
                sys.stdout.write(buf)
        }
        
        2.4.3 文件类对象引起的错误
        {
            使用makefile()函数从socket得到一个文件类对象，实际上，这个文件类对象调用了实际的socket
            所以文件类对象产生的异常和socket自己的send()和recv()函数产生的是一样的

            例子,shutdownfile.py
            {
                import socket, sys, time

                host = sys.argv[1]
                textport = sys.argv[2]
                filename = sys.argv[3]

                try:
                    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                except socket.error, e:
                    print "Strange error creating socket: %s" % e
                    sys.exit(1)
                
                try:
                    port = int(textport)
                except ValueError:
                    try:
                        port = socket.getservbyname(textport, 'tcp')
                    except socket.error, e:
                        print "Couldn't find your port: %s" % e
                        sys.exit(1)

                try:
                    s.connect((host, port))
                except socket.gaierror, e:
                    print "Address-related error connecting to server: %s" % e
                    sys.exit(1)
                except socket.error, e:
                    print "Connection error: %s" % e
                    sys.exit(1)

                fd = s.makefile('rw', 0)

                print "Sleeping..."
                time.sleep(10)
                print "Continuing"

                try:
                    fd.write("GET %s HTTP/1.0\r\n\r\n" % filename)
                except socket.error, e:
                    print "Error sending data: %s" % e
                    sys.exit(1)
                
                # 这里用了makefile()，没有调用send()
                # 所以其实没有必要调用flush()和shutdown()函数
                try:
                    fd.flush()
                except socket.error, e:
                    print "Error sending data (detected by flush): %s" % e
                    sys.exit(1)
                
                try:
                    # makfile()返回的对象并不提供一个对shutdown()的调用，需要有原始的socket对象调用它
                    s.shutdown(1)
                    s.close()
                except socket.error, e:
                    print "Error sending data (detected by shutdown): %s" % e
                    sys.exit(1)
                
                while 1:
                    try:
                        buf = fd.recv(2048)
                    except socket.error, e:
                        print "Error receiving data: %s" % e
                        sys.exit(1)
                    if not len(buf):
                        break
                    sys.stdout.write(buf)
            }
        }
    }
}

2.5 使用UDP
{
    UDP通信几乎不使用文件类对象，因为它们往往不能为数据如何发送和接收提供足够的控制

    例子，基本的UDP客户端
    {
        import socket, sys

        host = sys.argv[1]
        textport = sys.argv[2]

        # 程序调用的是SOCK_DGRAM而不是SOCK_STREAM，这就向系统提示socket将使用UDP通信而不是TCP
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            port = int(textport)
        except ValueError:
            port = socket.getservbyname(textport, 'udp')
        
        s.connect((host, port))
        print "Enter data to transmit: "
        data = sys.stdin.readline().strip()
        s.sendall(data)
        print "Looking for replies; press Ctrl-C or Ctrl-Break to stop"
        while 1:
            buf = s.recv(2048)
            if not len(buf):
                break
            sys.stdout.write(buf)
    }

    有时候，使用UDP可以根本就不调用connect()
    例子，udptime.py
    {
        import socket, sys, struct, time

        hostname = 'time.nist.gov'
        port = 37

        host = socket.gethostbyname(hostname)

        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.sendto('', (host, port))

        print "Looking for replies; press Ctrl-C to stop"
        # recvfrom()的调用会返回一个元组，其中包括两个数据，实际接收数据和发送数据的机器地址
        # 这里的实际接收数据是从1900年1月1日开始到现在二进制编码的秒数
        buf = s.recvfrom(2048)[0]
        if len(buf) != 4:
            print "Wrong-sized reply %d: %s" % (len(buf), buf)
            sys.exit(1)
        
        # 解开这个数据
        secs = struct.unpack("!T", buf)[0]
        # 减去从1900年到1970年之间的秒数
        secs -= 2208988800
        # 转换成UNIX的时间格式并在屏幕打印出来
        print time.ctime(int(secs))
    }
}