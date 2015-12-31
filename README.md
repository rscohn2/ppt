# ppt
Pin Python Tracer

Python tracer that shows native extensions as well as python functions. It is configured to only show functions in libraries that begin with libmkl, but could easily show all libraries by removing the filter.

Configuration
-------------

PPT relies on pin to trace the native code. You need to download pin from here: https://software.intel.com/en-us/articles/pin-a-binary-instrumentation-tool-downloads
    tar zxf pin-.....tar.gz

1. Point PIN_ROOT in makefile to the top level directory of the pin kit
2. Point PYTHON in makefile to the python that you are using

Building
--------
    make all
    
Testing
-------
    make test
Look in ppt.out for the results. Search for matmulx, it should call dot. If you are using MKL, you will also see the MKL functions that are called.
Running
-------
Look at makefile for the command line for 'make test'
The results are in ppt.out.

How it works
------------
The ppt package uses the cpython profiling capability to get a call back for each call and return, including built-in functions. When the program enters a built-in, ppt tells the pintool to trace the execution of native code. When the program returns from the built-in, ppt tells the pintool to stop tracing and return the functions that were called. Every function that is called is only listed once, and the order will not match the actual calling order.

I only wanted to trace MKL functions, so I limit the tracing to mkl libraries. If you want to trace native code, then edit ppt_tool.cpp. Search for mkl to find the filter.
