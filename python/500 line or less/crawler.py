import socket
from io import BlockingIOError

# def fetch(url):
#     sock = socket.socket()
#     sock.connect(('xkcd.com', 80))
#     request = 'GET {} HTTP/1.0\r\nHost: xkcd.com\r\n\r\n'.format(url)
#     sock.send(request.encode('ascii'))
#     response = b''
#     chunk = sock.recv(4096)
#     while chunk:
#         response += chunk
#         chunk = sock.recv(4096)
#
#     print response
#
# fetch("http://xkcd.com/650/")

def fetch(url):
    sock = socket.socket()
    # using non-blocking sockets
    sock.setblocking(False)
    try:
        sock.connect(('xkcd.com', 80))
    except BlockingIOError:
        pass

    # a way to know when the connection is established
    request = 'GET {} HTTP/1.0\r\nHost: xkcd.com\r\n\r\n'.format(url)
    encoded = request.encode('ascii')
    while True:
        try:
            sock.send(encoded)
            break  # Done.
        except OSError as e:
            pass
