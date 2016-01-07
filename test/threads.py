import thread
import threading


def hello():
    print('Hello from ' + str(thread.get_ident()))

def make_thread():
    t = threading.Thread(target=hello)
    t.start()
    return t

t1 = make_thread()
t2 = make_thread()
t1.join()
t2.join()
print('done')
