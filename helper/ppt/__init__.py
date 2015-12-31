import sys

import ppt.pinhelper as ph

class PPT:
    def __init__(self):
        return

    def __enter__(self):
        self.indent = 0
        self.out = open('ppt.out','w')
        self.old_dispatch = sys.getprofile()
        sys.setprofile(self.trace_dispatch)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        sys.setprofile(self.old_dispatch)
        self.out.close()

    def log(self, str):
        self.out.write('  ' * self.indent + str + '\n')

    def format_frame(self, frame):
        return frame.f_code.co_filename + ':' + frame.f_code.co_name

    def format_ccall(self, func):
        if func.__module__:
            return func.__module__ + ':' + func.__name__
        else:
            return ':' + func.__name__

    def trace_dispatch(self, frame, event, arg):
        if event is 'call':
            self.log('call ' + self.format_frame(frame))
            self.indent += 1
        elif event is 'return':
            self.log('return ' + self.format_frame(frame))
            self.indent -= 1
        elif event is 'c_call':
            self.log('ccall ' + self.format_ccall(arg))
            self.indent += 1
            ph.start_trace()
        elif event is 'c_return':
            cfunctions = ph.stop_trace()
            for f in cfunctions:
                self.log('ccall ' + f)
            self.log('creturn ' + self.format_ccall(arg))
            self.indent -= 1
        elif event is 'c_exception':
            self.log('cexception ' + self.format_frame(frame))
            self.indent -= 1
        else:
            self.log('Unknown event: ' + event + ' Name: ' + frame.f_code.co_name)
