from distutils.core import setup, Extension
setup(name='pinpytrace', version='1.0',  \
      ext_modules=[Extension('pinpytrace', ['ppt_helper.c'])])
