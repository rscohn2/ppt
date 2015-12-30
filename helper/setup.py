from distutils.core import setup, Extension
setup(name='ppt', version='1.0',  \
      packages=['ppt'], \
      ext_modules=[Extension('ppt.pinhelper', ['pin_helper.c'])])
