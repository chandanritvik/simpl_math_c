from setuptools import setup, Extension

module = Extension(
    'mexc_api',
    sources=['simple_math_c.c'],
    # Add platform-specific flags if needed
    extra_compile_args=['-O3'] if not platform.system() == 'Windows' else [],
)

setup(
    name='mexc_api',
    version='1.0.0',
    description='MEXC API Extension Module',
    author='Your Name',
    author_email='your.email@example.com',
    ext_modules=[module],
    python_requires='>=3.7',
    install_requires=[
        'requests',
    ],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Operating System :: OS Independent',
    ],
)