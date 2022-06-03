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

`pyAgrum <http://agrum.org>`_ is a scientific C++ and Python library dedicated to
Bayesian networks (BN) and other Probabilistic Graphical Models. Based on the C++
`aGrUM <https://agrum.lip6.fr>`_ library, it provides a high-level interface to
the C++ part of aGrUM allowing to create, manage and perform efficient computations
with Bayesian networks  and others probabilistic graphical models : Markov networks (MN),
influence diagrams (ID) and LIMIDs, credal networks (CN), dynamic BN (dBN), probabilistic relational models (PRM).

.. image::  _static/fastModelsWithPyAgrum.png

The module is generated using the `SWIG <http://www.swig.org>`_ interface
generator. Custom-written code was added to make the interface more
user friendly.

pyAgrum aims to allow to easily use (as well as to prototype new algorithms on)
Bayesian network and other graphical models.

pyAgrum contains :
  * a :ref:`comprehensive API documentation <Reference manual>`.
  * :ref:`tutorials as jupyter notebooks <Notebooks>`.
  * a `gitlab repository <https://gitlab.com/agrumery/aGrUM>`_.
  * and a `website <http://agrum.org>`_.

.. toctree::
  :maxdepth: 1

  reference
  notebooks

Indices and tables
==================

   * :ref:`genindex`
   * :ref:`modindex`
   * :ref:`search`
