class Node(dict):
    def __init__(self, name, mark):
        self.name = name
        self.mark = mark
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

    def lookup(self, name, mark=False):
        if name in self:
            return self[name]
        n = Node(name, mark)
        self[name] = n
        return n

    def ncall(self, name):
        n = self.lookup(name, True)
        self.callstack.top()[name] = n
        self.dump()

    def call(self, name):
        n = self.lookup(name)
        self.callstack.top()[name] = n
        self.callstack.push(n)
        self.dump()

    def ret(self, name):
        self.callstack.pop()

    def mark(self):
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

    def dump(self, force=False):
        self.dumpcount -= 1
        if not force and self.dumpcount > 0:
            return
        self.dumpcount = 10000
        self.mark()
        with open(self.filename, 'w') as fp:
            fp.write('Callgraph\n')
            fp.write('---------\n')
            for src in self:
                fp.write(self.annotated_name(self[src]) + '\n')
                n = self[src]
                for dst in n:
                    fp.write('  ' + self.annotated_name(n[dst]) + '\n')
            fp.write('#EOF\n')
