from setuptools import setup, Extension

module = Extension('simple_math_c',
                  sources=['simple_math_c.c'],
                  libraries=["crypto"])

setup(name='simple_math_c',
      version='1.0',
      install_requires=[
      "requests>=2.0.0"],
      ext_modules=[module])