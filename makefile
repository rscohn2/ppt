PIN_ROOT=/home/rscohn1/pin-2.14-71313-gcc.4.4.7-linux
PYTHON=/home/rscohn1/python/bin/python

all: ext pintool

ext:
	cd helper; $(PYTHON) setup.py install

pintool:
	cd tool; make obj-intel64/ppt_tool.so

test: ext pintool
	$(PIN_ROOT)/pin -t tool/obj-intel64/ppt_tool.so -- $(PYTHON) -m ppt.__main__ test/matmul.py
	python cgview.py

scipy: ext pintool
	$(PIN_ROOT)/pin -t tool/obj-intel64/ppt_tool.so -- $(PYTHON) -m ppt.__main__ test/scipybench.py

numscipytest: ext pintool
	$(PIN_ROOT)/pin -t tool/obj-intel64/ppt_tool.so -- $(PYTHON) -m ppt.__main__ test/numscipytest.py
