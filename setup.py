from setuptools import setup, Extension

module = Extension('simple_math_c',
                  sources=['simple_math_c.c'])

setup(name='simple_math_c',
      version='1.0',
      ext_modules=[module])