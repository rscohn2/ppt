class Log():
    def __init__(self, filename='ppt.log.out'):
        self.filename = filename
        self.out = open(filename, 'w')
        self.indent = 0

    def close(self):
        self.out.write('#EOF\n')
        self.out.close();

    def emit(self, str):
        self.out.write('  ' * self.indent + str + '\n')

    def call(self, name):
        self.emit('call ' + name)
        self.indent += 1

    def ncall(self, name):
        self.emit('ccall ' + name)

    def ret(self, name):
        self.emit('return ' + name)
        self.indent -= 1
