学习博文：一起写一个web服务器

一个简单的web服务器
{
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
        client_connect, client_address = listen_socket.accept()
        request = client_connection.recv(1024)
        print request
        http_response = """
            HTTP/1.1 200 OK
            Hello, world
        """
        client_connection.sendall(http_response)
        client_connection.close()
}

WSGI服务器可以适应不同的web框架
WSGI服务器的实现
{
    # -*- coding: utf-8 -*-
    import socket
    import StringIO
    import sys
    import traceback

    class WSGIServer(object):
        address_family = socket.AF_INET
        socket_type = socket.SOCK_STREAM
        request_queue_size = 1

        def __init(self, server_address):
            self.listen_socket = socket.socket(self.address_family, self.sock_type)
            self.listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.listen_socket.bind(server_address)
            self.listen_socket(self.request_queue_size)
            host, port = self.listen_socket.getsockname()[:2]
            # Return a fully qualified domain name for name
            self.server_name = socket.getfqdn(host)
            self.server_port = port
            # Return headers set by Web framework/Web application
            self.headers_set = []
        
        def set_app(self, application):
            self.application = application

        def serve_forever(self):
            while True:
                try:
                    self.client_connection, client_address = self.listen_socket.accept()
                except KeyboardInterrupt:
                    raise
                except:
                    traceback.print_exc()
                    continue

                self.handle_one_request()

        def handle_one_request(self):
            self.request_data = self.client_connection.recv(1024)
            print(''.join('< {line}\n'.format(line = line) for line in self.request_data.splitlines()))
            self.parse_request(self.request_data)
            env = self.get_environ()
            result = self.application(env, self.start_response)
            self.finish_response(result)
        
        def parse_request(self, text):
            request_line = text.splitlines()[0]
            # Return a copy of the string with trailing characters removed.
            request_line.rstrip('\r\n')
            # eg. GET /hello HTTP/1.1
            (self.request_method, self.path, self.request_version) = request_line.split()
        
        def get_environ(self):
            env = {}
            # Required WSGI variables
            env['wsgi.version'] = (1, 0)
            env['wsgi.url_scheme'] = 'http'
            env['wsgi.input'] = StringIO.StringIO(self.request_data)
            env['wsgi.errors'] = sys.stderr
            env['wsgi.multithread'] = False
            env['wsgi.multiprocess'] = False
            env['wsgi.run_once'] = False
            # Required CGI variables
            # eg. GET /hello localhost 8888
            env['REQUEST_METHOD'] = self.request_method
            env['PATH_INFO'] = self.path
            env['SERVER_NAME'] = self.server_name
            env['SERVER_PORT'] = str(self.server_port)
            return env
        
        def start_response(self, status, response_headers, exc_info = None):
            server_headers = [('Date', 'Tue, 31 Mar 2015 12:54:48 GMT'), ('Server', 'WSGIServer 0.2'),]
            self.headers_set = [status, response_headers + server_headers]
        
        def finish_response(self, result):
            try:
                status, response_headers = self.headers_set
                response = 'HTTP/1.1 {status}\r\n'.format(status = status)
                for header in response_headers:
                    response += '{0}: {1}\r\n'.format(*headers)
                response += '\r\n'
                for data in result:
                    response += data
                print(''.join('> {line}\n'.format(line = line) for line in response.splitlines()))
                self.client_connection.sendall(response)
            finally:
                self.client_connection.close()
    
    SERVER_ADDRESS = (HOST, PORT) = '', 8888

    def make_server(server_address, application):
        server = WSGIServer(server_address)
        server.ser_app(application)
        return server
    
    if __name__ == '__main__':
        if len(sys.argv) < 2:
            sys.exit('Provide a WSGI application object as module: callable')
        app_path = sys.argv[1]
        module, application = app_path.split(':')
        module = __import__(module)
        application = getattr(module, application)
        httpd = make_server(SERVER_ADDRESS, application)
        print('WSGIServer: Serving HTTP on port {port} ...\n'.format(port = PORT))
        httpd.serve_forever()
}

上面一段代码其实来得有点莫名其妙，里面的一些语句功能还未了解。需要进一步知道WSGI和CGI的用法

WSGI的强大：提供了Python web服务器和Python Web框架之间的一个最小的接口
下面的代码片段展示了WSGI接口的服务器和框架端
{
    # 参数为一个可调的application
    def run_application(application):
        headers_set = []
        environ = {}

        def start_response(status, response_headers, exc_info = None):
            # 服务器把状态，响应头和响应体合并到HTTP响应里
            headers_set[:] = [status, response_headers]

        # 参数为一个包含了WSGI/CGI变量的字典和一个可调用的start_response
        result = application(environ, start_response)
    
    def app(environ, start_response):
        start_response('200 OK', [('Content-Type', 'text/plain')])
        return ['Hello world!']
    
    run_application(app)
}

这个博客文章我看来只能说真是太烂了