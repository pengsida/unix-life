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
            host = sys.argv[1]
            filename = sys.argv[2]

            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((host, post))
            s.sendall(filename + "\r\n")

            while 1:
                buf = s.recv(2048)
                if not len(buf):
                    break
                sys.stdout.write(buf)
        }
    }
}