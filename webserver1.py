# -*- coding: utf-8 -*-
import socket
host, port = '', 8888
# 建立socket对象
listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 设置socket可选项
listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# 绑定
listen_socket.bind((host, port))
# 监听
listen_socket.listen(1)
print 'Serving HTTP on port %s ...' % port

while 1:
    client_connection, client_address = listen_socket.accept()
    request = client_connection.recv(1024)
    print request
    http_response = """
        HTTP/1.1 200 OK
        Hello, world
    """
    client_connection.sendall(http_response)
    client_connection.close()