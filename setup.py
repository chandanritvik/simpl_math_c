from setuptools import setup, Extension

module = Extension('mexc_api',
                  sources=['mexc_api.c'])

setup(name='mexc_api',
      version='1.0',
      ext_modules=[module])