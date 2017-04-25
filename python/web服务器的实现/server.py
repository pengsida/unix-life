# -*- coding: utf-8 -*-
import BaseHTTPServer
import sys
import os

# 基本步骤
# 1. 等待某个人连接我们的服务器并向我们发送一个HTTP请求
# 2. 解析该请求
# 3. 了解该请求希望请求的内容
# 4. 服务器根据请求抓取需要的数据（从服务器本地文件中读或者程序动态生成）
# 5. 将数据格式化为请求需要的格式
# 6. 送回HTTP响应

class ServerException(Exception):
    pass

class BaseCase(object):
    def handle_file(self, handler, full_path):
        try:
            with open(full_path, 'rb') as reader:
                page = reader.read()
            handler.send_content(page)
        except IOError as msg:
            msg = "'{0}' cannot be read: {1}".format(handler.path, msg)
            handler.handle_error(msg)

    def index_path(self, handler):
        return os.path.join(handler.full_path, 'index.html')

    def test(self, handler):
        assert False, 'Not implemented.'

    def act(self, handler):
        assert False, 'Not implemented.'

class CaseNoFile(BaseCase):
    """ 路径不存在 """
    def test(self, handler):
        return not os.path.exists(handler.full_path)

    def act(self, handler):
        raise ServerException("{0} not found".format(handler.path))

class CaseExistingFile(BaseCase):
    """ 路径是一个文件 """
    def test(self, handler):
        return os.path.isfile(handler.full_path)

    def act(self, handler):
        self.handle_file(handler, handler.full_path)

class CaseDirectoryIndexFile(BaseCase):
    """ 路径是一个目录 """
    def test(self, handler):
        return os.path.isdir(handler.full_path) and os.path.isfile(self.index_path(handler))

    def act(self, handler):
        self.handle_file(handler, self.index_path(handler))

class CaseAlwaysFail(BaseCase):
    """ 默认处理类 """
    def test(self, handler):
        return True

    def act(self, handler):
        raise ServerException("Unknown object '{0}".format(handler.path))

class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    Page = '''\
        <html>
            <body>
                <table>
                    <tr>  <td>Header</td>         <td>Value</td>          </tr>
                    <tr>  <td>Date and time</td>  <td>{date_time}</td>    </tr>
                    <tr>  <td>Client host</td>    <td>{client_host}</td>  </tr>
                    <tr>  <td>Client port</td>    <td>{client_port}</td>  </tr>
                    <tr>  <td>Command</td>        <td>{command}</td>      </tr>
                    <tr>  <td>Path</td>           <td>{path}</td>         </tr>
                </table>
            </body>
        </html>
    '''

    Error_Page = """\
    <html>
        <body>
            <h1>Error accessing {path}</h1>
            <p>{msg}</p>
        </body>
    </html>
    """

    Cases = [CaseNoFile(),
             CaseExistingFile(),
             CaseDirectoryIndexFile(),
             CaseAlwaysFail()]

    full_path = ""

    def do_GET(self):
        try:
            self.full_path = os.getcwd() + self.path
            for case in self.Cases:
                handler = case
                if handler.test(self):
                    handler.act(self)
                    break

        except Exception as msg:
            self.handle_error(msg)

        page = self.create_page()
        self.send_content(page)

    def create_page(self):
        values = {
            'date_time': self.date_time_string(),
            'client_host': self.client_address[0],
            'client_port': self.client_address[1],
            'command': self.command,
            'path': self.path
        }
        page = self.Page.format(**values) # 可变参数
        return page

    def send_content(self, page, status=200):
        self.send_response(status)
        self.send_header("Content-Type", "text/html")
        self.send_header("Content-Length", str(len(page)))
        self.end_headers()
        self.wfile.write(page)

    def handle_error(self, msg):
        error_page = self.Error_Page.format(path=self.path, msg=msg)
        self.send_content(error_page, 404)
    
if __name__ == '__main__':
    server_address = ('', 8080)
    server = BaseHTTPServer.HTTPServer(server_address, RequestHandler)
    server.serve_forever()
