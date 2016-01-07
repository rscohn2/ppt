import numpy as np
import os

def matmulx():
    n = 100
    a = np.array(np.random.rand(n,n), dtype = np.float32);
    b = np.array(np.random.rand(n,n), dtype = np.float32);
    c = np.dot(a,b)
    #print('pid: ' + str(os.getpid()))
    #x = raw_input('hit enter to continue.')
    #c = np.dot(a,b)
    #x = raw_input('hit enter to continue.')

matmulx()
