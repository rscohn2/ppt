import json

class Node(dict):
    def __init__(self, key, native):
        self.key = key
        self.native = native
        return

class CallStack():
    def __init__(self):
        self.list = []

    def top(self):
        return self.list[len(self.list)-1]

    def pop(self):
        if len(self.list) == 1:
            return self.top()
        return self.list.pop()

    def push(self, item):
        self.list.append(item)
        return item

class CallGraph(dict):
    def __init__(self, filename='ppt.cg.out'):
        self.filename = filename
        self.callstack = CallStack()
        self.callstack.push(self.lookup('root'))
        self.dumpcount = 0

    def close(self):
        self.dump(force=True)

    def lookup(self, key, native=False):
        if key in self:
            return self[key]
        n = Node(key, native)
        self[key] = n
        return n

    def ncall(self, key):
        n = self.lookup(key, True)
        self.callstack.top()[key] = n
        self.dump()

    def call(self, key):
        n = self.lookup(key)
        self.callstack.top()[key] = n
        self.callstack.push(n)
        self.dump()

    def ret(self, key):
        self.callstack.pop()

    def mark_native(self):
        '''Set mark if a function is native or calls native code'''
        for src in self:
            n = self[src]
            n.mark = n.native
        changed = True
        while changed:
            changed = False
            for src in self:
                srcn = self[src]
                if srcn.mark:
                    continue
                for dst in srcn:
                    dstn = self[dst]
                    if dstn.mark:
                        srcn.mark = True
                        changed = True
                        break

    def annotated_name(self, node):
        return ('**' if node.mark else '') + node.name

    def write_graph(self):
        self.mark_native()
        with open(self.filename, 'w') as fp:
            fp.write('Callgraph\n')
            fp.write('---------\n')
            for src in self:
                fp.write(self.annotated_name(self[src]) + '\n')
                n = self[src]
                for dst in n:
                    fp.write('  ' + self.annotated_name(n[dst]) + '\n')
            fp.write('#EOF\n')

    def node_dict(self, src):
        return {'key': src.key, 'native': src.native, 'dsts': [src[dst].key for dst in src]}

    def write_json_graph(self):
        d = {'nodes': [self.node_dict(self[src]) for src in self]}
        with open(self.filename, 'w') as fp:
             json.dump(d, fp, indent=4)
        
    def dump(self, force=False):
        self.dumpcount -= 1
        if not force and self.dumpcount > 0:
            return
        self.dumpcount = 100000
        self.write_json_graph()

