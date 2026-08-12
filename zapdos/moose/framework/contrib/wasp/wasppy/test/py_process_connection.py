from wasp import *
import os, sys, subprocess

class PyProcessConnection(Connection):
    '''Connection which writes to server stdin and reads from its stdout'''

    def __init__(self):
        super().__init__()
        self.has_server_started = False
        self.server_errors = string()

    def read(self, object, errors):
        # read expected "Content-Length: value\n" header part of available data
        header_part = self.server_process.stdout.readline()
        # split string using whitespace to create [Content-Length:][value] list
        header_list = header_part.split()
        # check that list contains two items and "Content-Length:" is first one
        if len(header_list) != 2 or header_list[0] != "Content-Length:":
            self.server_errors += "Bad 'Content-Length' property: '" + header_part + "'\n"
            return False
        # check that content length string can convert to integer without issue
        if not header_list[1].isdigit():
            self.server_errors += "Bad 'Content-Length' value: '" + header_list[1] + "'\n"
            return False
        # number of bytes for '\n' plus content part is content length plus one
        read_bytes = int(header_list[1]) + 1
        # next read expected number of bytes to capture content part of message
        content_part = self.server_process.stdout.read(read_bytes)
        # combine parts together to build JSON-RPC string making entire payload
        rpc_string = header_part + content_part
        # pass this entire payload to RPCStringToObject and convert into object
        success = RPCStringToObject(rpc_string, object, errors)
        return success

    def write(self, object, errors):
        # pass given object to objectToRPCString and convert into writable text
        rpc_string = string()
        success = objectToRPCString(object, rpc_string, errors)
        # it is important to flush after this write call because it is buffered
        self.server_process.stdin.write(str(rpc_string))
        self.server_process.stdin.flush()
        return success

    def isServerRunning(self):
        return self.has_server_started and self.server_process.poll() is None

    def getServerErrors(self):
        return self.server_errors

    def startServer(self, path, args=[]):
        if self.isServerRunning():
            self.server_errors += "Server cannot be started since it is already running\n"
            return False
        if not os.path.isfile(path):
            self.server_errors += "Server cannot be started as '" + path + "' not found\n"
            return False
        # create subprocess with universal_newlines enabled so streams are text
        # but this also treats protocol '\r\n' as '\n' and read should use that
        self.server_process = subprocess.Popen([sys.executable]+[path]+args,
                                               stdin=subprocess.PIPE,
                                               stdout=subprocess.PIPE,
                                               universal_newlines=True)
        self.has_server_started = True
        return True

    def waitForFinished(self):
        self.server_process.wait()
        self.server_process.stdout.close()
        self.server_process.stdin.close()
        return True
