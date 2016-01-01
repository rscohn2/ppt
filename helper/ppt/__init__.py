import sys

import ppt.pinhelper as ph
import ppt.cg as cg
import ppt.log as log

class PPT:
    def __init__(self, graph = False):
        if graph:
            self.recorder = cg.CallGraph()
        else:
            self.recorder = log.Log()

    def __enter__(self):
        self.old_dispatch = sys.getprofile()
        sys.setprofile(self.trace_dispatch)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        sys.setprofile(self.old_dispatch)
        self.recorder.close()

    def format_frame(self, frame):
        return frame.f_code.co_filename + ':' + frame.f_code.co_name

    def format_ccall(self, func):
        if func.__module__:
            return func.__module__ + ':' + func.__name__
        else:
            return ':' + func.__name__

    def trace_dispatch(self, frame, event, arg):
        if event is 'call':
            self.recorder.call(self.format_frame(frame))
        elif event is 'return':
            self.recorder.ret(self.format_frame(frame))
        elif event is 'c_call':
            self.recorder.call(self.format_ccall(arg))
            ph.start_trace()
        elif event is 'c_return':
            cfunctions = ph.stop_trace()
            for f in cfunctions:
                self.recorder.ncall(f)
            self.recorder.ret( self.format_ccall(arg))
        elif event is 'c_exception':
            self.recorder.ret(self.format_frame(frame))
