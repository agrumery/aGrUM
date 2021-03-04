"""pyAgrum setup file
See:
https://agrum.gitlab.io/
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

#TODO copy requirements from requirements.txt
# REQUIREMENTS = list()
# with open('requirements.txt', mode='r', encoding='utf-8') as fd:
#   REQUIREMENTS=fd.read().splitlines()

swig_files = [ "pyAgrum/pyAgrum.i" ]
flags = ["-fopenmp",
         "-std=c++1y", 
         "-Wall", 
         "-pedantic", 
         "-fopenmp", 
         "-fPIC"]

options = ["-c++",
           "-I/usr/local/include",
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
    description='Bayesian networks and other Probabilistic Graphical Models',
    url='https://agrum.gitlab.io/',
    maintainer='Lionel Torti and Gaspard Ducamp',
    maintainer_email='info@agrum.org',
    platforms=['any'],
    license='LGPLv3',
    packages=['pyAgrum', 'pyAgrum/lib', 'pyAgrum/lib/_utils'],
    long_description=README,
    include_package_data=True,
    ext_modules=[my_ext],
    install_requires=['numpy','six','configparser'],
    classifiers=[
    'Development Status :: 5 - Production/Stable', 
    'License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)',
    'Operating System :: Microsoft :: Windows',
    'Operating System :: MacOS :: MacOS X', 
    'Operating System :: POSIX :: Linux',
    'Programming Language :: C++',
    'Programming Language :: Python',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 3.6',
    'Programming Language :: Python :: 3.7',
    'Programming Language :: Python :: 3.8',
    'Programming Language :: Python :: 3.9',
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
