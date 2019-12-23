.. pyAgrum documentation master file, created by
   sphinx-quickstart on Mon Oct 17 16:59:01 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.


Introduction to pyAgrum
=======================

.. figure:: _static/agrum.png
    :align: right
    :alt: aGrUM
    :target: http://agrum.org

pyAgrum is a Python wrapper for the C++ `aGrUM <http://agrum.org>`_ library.
It provides a high-level interface to the C++ part of aGrUM allowing to create, manage and perform efficient computations with Bayesian Networks.

.. image::  _static/fastBNwithPyAgrum.png

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make
the interface more user friendly.

pyAgrum aims to allow to easily use (as well as to prototype new algorithms on) Bayesian network and other graphical models.

pyAgrum contains
  * a `comprehensive API documentation <https://pyagrum.readthedocs.io>`_,
  * `tutorials as jupyter notebooks <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/01-tutorial.ipynb.html>`_,
  * a `gitlab repository <https://gitlab.com/agrumery/aGrUM>`_,
  * and a `website <http://agrum.org>`_.

.. toctree::
  :caption: Main classes
  :maxdepth: 2

  bayesianNetwork

  graph
  randomVariables
  potential

.. toctree::
  :caption: pyAgrum.lib modules
  :maxdepth: 2
  :titlesonly:

  lib.notebook
  lib.bn2graph
  lib.dBN
  lib.classifier
  pyAgrum.lib

.. toctree::
  :caption: Causality in pyAgrum
  :maxdepth: 2
  :titlesonly:

  Causal

.. toctree::
  :caption: Other graphical models
  :maxdepth: 2

  PRM
  credalNetwork
  influenceDiagram

.. toctree::
  :caption: Miscellaneous
  :maxdepth: 2
  
  functions
  miscellaneous
  exceptions

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
