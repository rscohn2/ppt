import numpy as np
import os

def matmulx():
    n = 1000
    a = np.array(np.random.rand(n,n), dtype = np.double);
    b = np.array(np.random.rand(n,n), dtype = np.double);
    c = np.dot(a,b)
    #print('pid: ' + str(os.getpid()))
    #x = raw_input('hit enter to continue.')
    #c = np.dot(a,b)
    #x = raw_input('hit enter to continue.')

matmulx()
