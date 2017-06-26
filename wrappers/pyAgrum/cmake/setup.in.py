"""pyAgrum setup file
See:
https://pyagrum.lip6.fr
"""
# Always prefer setuptools over distutils
try:
  from setuptools import setup, Extension
except ImportError:
  from distutils.core import setup, Extension

from io import open

README = ''
with open('README.rst', mode='r', encoding='utf-8') as fd:
  README = fd.read()

swig_files = [ "pyAgrum/pyAgrum.i" ]
flags = ["-fopenmp",
         "-std=c++1y", 
         "-Wall", 
         "-pedantic", 
         "-fopenmp", 
         "-fPIC"]

options = ["-c++",
           "-I/usr/loca/include",
           "-I/usr/include",
           "-I./pyAgrum/extensions",
           "-I./pyAgrum/doc"]

my_ext = Extension( name='_pyAgrum',
    sources=swig_files,
    include_dirs=["pyAgrum/extensions", "/usr/include"],
    define_macros=[("NDEBUG", None)],
    extra_compile_args=flags,
    language="c++",
    libraries=['agrum'],
    swig_opts=options
    )

setup(
    name='pyAgrum',
    version='@PYAGRUM_VERSION@',
    description='pyAgrum is a Python wrapper for the C++ aGrUM library',
    url='https://pyagrum.lip6.fr',
    maintainer='Pierre-Henri Wuillemin and Christophe Gonzales',
    maintainer_email='info@agrum.org',
    platforms=['any'],
    license='GPL',
    packages=['pyAgrum', 'pyAgrum/lib', 'pyAgrum/lib/_utils'],
    long_description=README,
    include_package_data=True,
    ext_modules=[my_ext],
    install_requires=['numpy'],
    classifiers=[
    'Development Status :: 4 - Beta', 
    'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
    'Operating System :: Microsoft :: Windows',
    'Operating System :: MacOS :: MacOS X', 
    'Operating System :: POSIX :: Linux',
    'Programming Language :: C++',
    'Programming Language :: Python',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 3.5',
    'Programming Language :: Python :: 3.6',
    'Programming Language :: Python :: 2',
    'Programming Language :: Python :: 2.7',
    'Topic :: Scientific/Engineering :: Artificial Intelligence',
    'Topic :: Scientific/Engineering :: Information Analysis',
    'Topic :: Scientific/Engineering :: Mathematics',
    'Topic :: Software Development :: Libraries',
    'Topic :: Software Development :: Libraries :: Python Modules',
    'Intended Audience :: Developers',
    'Intended Audience :: Financial and Insurance Industry',
    'Intended Audience :: Information Technology',
    'Intended Audience :: Science/Research',
    ],
    keywords='probabilities probabilistic-graphical-models inference diagnosis',
    )
