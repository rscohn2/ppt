import types
import ppt.pinhelper as ph

def process_mod(mod):
    for s in dir(mod):
        i = getattr(mod,s)
        if type(i) is types.ModuleType and i.__package__ is s:
            process_mod(i)
        if type(i) is types.BuiltinMethodType or type(i) is types.BuiltinFunctionType:
            ph.trace_method(i)

def process_mods(mods):
    for m in mods:
        process_mod(__import__(m))
