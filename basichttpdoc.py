from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
import time

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
    
    # HTTP请求例子，HEAD /stats.html HTTP/1.0(HTTP Method, Path, HTTP Version)
    def do_HEAD(self):
        doc = self._getdoc(self.path)
        self._writeheaders(doc)
    
    # HTTP请求例子，GET /stats.html HTTP/1.1(HTTP Method, Path, HTTP Version)
    def do_GET(self):
        doc = self._getdoc(self.path)
        self._writeheaders(doc)
        if doc is None:
            self.wfile.write("""<html><head><title>Not Found</title></head>
            <body>The requested document '%s' was not found.</body>
            """ % self.path)
        else:
            self.wfile.write(doc)

serveraddr = ('', 8765)
sevr = HTTPServer(serveraddr, RequestHandler)
sevr.serve_forever()
