from setuptools import setup, Extension

module = Extension('mexc_api',
                  sources=['mexc_api_module.c'],
                  libraries=['ssl', 'crypto'],
                  include_dirs=['/usr/include', '/usr/local/include'],
                  library_dirs=['/usr/lib', '/usr/local/lib'])

setup(name='mexc_api',
      version='1.0',
      description='MEXC API Extension Module',
      ext_modules=[module],
      install_requires=['requests'])