16.1 使用BaseHTTPServer
{
    BaseHTTPServer模块提供了web服务器所需要的基本东西
    他定义了两个类，一个server对象类和一个request处理类

    例子，示范BaseHTTPServer的用法
    {
        from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

        class RequestHandler(BaseHTTPRequestHandler):
            def _writeheaders(self):
                # 服务器响应消息,http状态码，200代表成功返回网页
                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_header()

            def do_HEAD(self):
                self._writeheaders()
            
            def do_GET(self):
                self._writeheaders()
                self.wfile.write("""<HTML><HEAD><TITLE> Sample Page</TITLE></HEAD><BODY>This is a sample HTML page. Every page this server provides will look like this.</BODY></HTML>""")
        
        serveraddr = ('', 8765)
        sevr = HTTPServer(serveraddr, RequestHandler)
        sevr.serve_forever()
    }
    然而这个例子只会给每个客户端相同的文档

    下面还有个例子，会提供两个文档，一个静态的，一个动态生成的
    {
        from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
        import time

        starttime = time.time()

        class RequestHandler(BaseHTTPRequestHandler):
            def _writeheaders(self, doc):
                if doc in None:
                    self.send_response(404)
                else:
                    self.send_response(200)
                
                self.send_header('Content-type', 'text/html')
                self.end_headers()
            
            def _getdoc(self, filename):
                if filename == '/':
                    return """<html><head><title>Sample Page</title></head>
                    <body>This is a sample page. You can also look at the
                    <a href="stats.html">server statistics</a>
                    </body></html>
                    """
                elif filename == '/stats.html':
                    return """<html><head><title>Statistics</title></head>
                    <body>This server has been running for %d seconds.
                    </body></html>
                    """ % int(time.time() - starttime)
                else:
                    return None
            
            def do_HEAD(self):
                doc = self._getdoc(self.path)
                self._writeheaders(doc)
            
            def do_GET(self):
                doc = self._getdoc(self.path)
                self._writeheaders(doc)
                if doc is None:
                    self.wfile.write("""<html><head><title>Not Found</title></head>
                    <body>The requested document '%s' was not found.</body>
                    """ % self.path)
                    self.wfile.write(doc)

        serveraddr = ('', 8765)
        sevr = HTTPServer(serveraddr, RequestHandler)
        sevr.serve_forever()
    }
    这个例子只能服务一个客户端，不适合应用于产品级的服务器

    16.1.2 同时处理多个请求
    {
        SocketServer提供的解决方案
        {
            1.forking，为每一个到来的连接开启一个新的进程，所有进程都是独立的
            2.threading，使用python thread来处理连接
        }

        例子，添加thread方法，完全支持多任务
        {
            # -*- coding = utf-8 -*-
            from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
            from SocketServer import ThreadingMixIn
            import time, threading

            starttime = time.time()

            class RequestHandler(BaseHTTPRequestHandler):
                def _writeheaders(self, doc):
                    if doc is None:
                        self.send_response(404)
                    else:
                        self.send_response(200)
                    self.send_header('Content-type', 'text/html')
                    self.end_headers()

                def _getdoc(self, filename):
                    if filename == '/':
                        return """<html><head><title>Statistics</title></head>
                        <body>This is a sample page. you can also look at the <a href="stats.html">server statistics
                        </body></html>
                        """
                    elif filename == 'stats.html':
                        return """<html><head><title>Statistics</title></head>
                        <body>This server has been running for %d seconds
                        </body></html>
                        """ % int(time.time() - starttime)
                    else:
                        return None
                    
                def do_HEAD(self):
                    doc = _getdoc(self.path)
                    _writeheaders(doc)

                def do_GET(self):
                    print "Handling with thread", threading.currentThread().getName()
                    doc = _getdoc(self.path)
                    _writeheaders(doc)
                    if doc is None:
                        self.wfile.write("""<html><head><title>Not Found</title></head>
                        <body>The requested document '%s' was not found. </body>
                        """ % self.path)
                    else:
                        self.wfile.write(doc)
            
            class ThreadingHTTPServer(ThreadingMixIn, HTTPServer):
                pass
            
            serveraddr = ('', 8765)
            # 貌似只要用这个类就能使用多线程
            srvr = ThreadingHTTPServer(serveraddr, RequestHandler)
            srvr.serve_forever()
        }
    }
}

16.2 SimpleHTTPServer
{
    SimpleHTTPServer类扩展了BaseHTTPServer类，提供了当前工作目录里面的符合规则的文件

    例子，实现简单的SimpleHTTPServer
    {
        from BaseHTTPServer import HTTPServer
        from SimpleHTTPServer import SimpleHTTPRequestHandler

        serveraddr = ('', 8765)
        srvr = HTTPServer(serveraddr, SimpleHTTPRequestHandler)
        srvr.serve_forever()
    }
}

16.3 CGIHTTPServer
{
    CGIHTTPServer可以运行所提供的文件中的CGI脚本程序
    CGI服务器用forking代替了threading

    例子
    {
        from BaseHTTPServer import HTTPServer
        from CGIHTTPServer import CGIHTTPRequestHandler
        from SocketServer import ForkingMixIn

        class ForkingServer(ForkingMixIn, HTTPServer):
            pass

        serveraddr = ('', 8765)
        srvr = ForkingServer(serveraddr, CGIHTTPRequestHandler)
        srvr.serve_forever()
    }
}

16.4 实现新协议
{
    from SocketServer import ThreadingMixIn, TCPServer, StreamRequestHandler
    import time

    class TimeRequestHandler(StreamRequestHandler):
        def handle(self):
            req = self.rfile.readline().strip()
            if req == "asctime":
                result = time.asctime()
            elif req == "seconds":
                result = str(int(time.time()))
            elif req == "rfc822":
                result = time.strftime("%a, %d %b %Y %H: %M:%S + 0000", time.gmtime())
            else:
                result = """Unhandled request. Send a line with one of the following words:
                asctime -- for human-readable time
                seconds -- seconds since the Unix Epoch
                rfc822 -- data/time in format used for mail and news posts"""
            self.wfile.write(result + "\n")

    class TimeServer(ThreadingMixIn, TCPServer):
        allow_reuse_address = 1

    serveraddr = ('', 8765)
    srvr = TimeServer(serveraddr, TimeRequestHandler)
    srvr.serve_forever()
}