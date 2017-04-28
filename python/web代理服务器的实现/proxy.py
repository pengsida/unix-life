import socket
import thread
import sys

def get_host_and_port(request):
    first_line = request.split('\n')[0]
    url = first_line.split(' ')[1]

    http_pos = url.find("://")
    if http_pos == -1:
        temp = url
    else:
        temp = url[(http_pos+3):]
    
    host_pos = temp.find("/")
    if host_pos == -1:
        host_pos = len(temp)

    port_pos = temp.find(":")

    if (port_pos == -1 or host_pos < port_pos):
        port = 80
        host = temp[:host_pos]
    else:
        port = int((temp[(port_pos+1):])[:host_pos-port_pos-1])
        host = temp[:port_pos]

    return host, port


def proxy_thread(client_socket):
    request = client_socket.recv(999999)
    host, port = get_host_and_port(request)

    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect((host, port))
        server_socket.send(request)

        while 1:
            data = server_socket.recv(999999)
            if len(data) > 0:
                client_socket.send(data)
            else:
                break

        server_socket.close()
        client_socket.close()
    except socket.error, (value, message):
        if server_socket:
            server_socket.close()

        if client_socket:
            client_socket.close()

        sys.exit(1)

port = 12000

proxy_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
proxy_socket.bind(('', port))
proxy_socket.listen(5)

while 1:
    client_socket, client_addr = proxy_socket.accept()
    # proxy_thread(client_socket, client_addr)
    thread.start_new_thread(proxy_thread, (client_socket,))