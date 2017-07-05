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
It provides a high-level interface to the part of aGrUM allowing to create, handle and make computations into Bayesian Networks.

.. warning:: For now, this documentation is mainly automatically generated.

.. image::  _static/fastBNwithPyAgrum.png

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make
the interface more friendly.

pyAgrum aims to allow to easily use (as well as to prototype new algorithms on) Bayesian network and other graphical models.

pyAgrum contains
  * a `comprehensive API documentation <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/pyAgrum/index.html>`_,
  * `examples as jupyter notebooks <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/01-tutorial.ipynb.html>`_,
  * and a `website <http://agrum.org>`_.


pyAgrum library
===============

.. toctree::
  :maxdepth: 5

  functions
  graph
  randomVariables
  potential
  bayesianNetwork
  PRM
  credalNetwork
  influenceDiagram
  miscellaneous
  exceptions

Python helpers : pyAgrum.lib
============================

pyAgrum.lib is a set of python tools.

.. toctree::
  :maxdepth: 2
  :titlesonly:

  lib.notebook
  lib.bn2graph
  lib.dBN
  pyAgrum.lib


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. Exhaustif but not usable
.. =====================
.. .. toctree::
..     pyAgrum
..     pyAgrum.lib
