import sys
import inspect
import json

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

    def get_frame(self, frame):
        m = inspect.getmodule(frame)
        return (frame.f_code.co_name, m.__name__ if m else 'root')

    def get_ccall(self, func):
        return (func.__name__, func.__module__ if func.__module__ else 'root')

    def trace_dispatch(self, frame, event, arg):
        if event is 'call':
            self.recorder.call(self.get_frame(frame))
        elif event is 'return':
            self.recorder.ret(self.get_frame(frame))
        elif event is 'c_call':
            self.recorder.call(self.get_ccall(arg))
            ph.start_trace()
        elif event is 'c_return':
            cfunctions = ph.stop_trace()
            for f in cfunctions:
                x = f.split(':')
                self.recorder.ncall((x[0],x[1]))
            self.recorder.ret( self.get_ccall(arg))
        elif event is 'c_exception':
            self.recorder.ret(self.get_frame(frame))
