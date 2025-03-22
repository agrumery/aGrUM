# pyAgrum

[![PyPI version](https://img.shields.io/pypi/v/pyAgrum.svg?logo=pypi&logoColor=FFE873)](https://pypi.org/project/pyAgrum/)
[![Supported Python versions](https://img.shields.io/pypi/pyversions/pyagrum.svg?logo=python&logoColor=FFE873)](https://pypi.org/project/pyAgrum/)

pyAgrum is a Python wrapper for the C++ aGrUM library (using SWIG interface generator). It provides a high-level interface to the part of aGrUM allowing to create, model, learn, use, calculate with and embed Bayesian Networks and other graphical models. Some specific (python and C++) codes are added in order to simplify and extend the aGrUM API.

Several topics have been added to pyAgrum (as pure python modules using pyAgrum) :

- Scikit-learn-compliant probabilistic classifiers based on Bayesian networks,
- Probabilistic causality (causal networks, do-calculus),
- dynamic Bayesian network.

See the [tutorials as jupyter notebooks](https://webia.lip6.fr/~phw//aGrUM/docs/last/notebooks/Tutorial.ipynb.html) for more details.