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
with Bayesian networks  and others probabilistic graphical models : Markov random fields (MRF),
influence diagrams (ID) and LIMIDs, credal networks (CN), dynamic BN (dBN), probabilistic relational models (PRM).

.. image::  _static/fastModelsWithPyAgrum.png

The module is generated using the `SWIG <http://www.swig.org>`_ interface
generator. Custom-written code was added to make the interface more
user friendly.

pyAgrum aims to allow to easily use (as well as to prototype new algorithms on)
Bayesian network and other graphical models.

pyAgrum contains :
  * :ref:`tutorials as jupyter notebooks <Notebooks>`.
  * a :ref:`comprehensive API documentation <Reference manual>`.
  * a `gitlab repository <https://gitlab.com/agrumery/aGrUM>`_.
  * and a `website <http://agrum.org>`_.


.. toctree::
  :caption: Tutorials and notebooks
  :name: Notebooks
  :titlesonly:
  :maxdepth: 1

  notebooks

Reference manual
================

.. toctree::
  :caption: 1- Fundamental components
  :maxdepth: 2

  graph
  randomVariables
  potential

.. toctree::
  :caption: 2- Bayesian networks
  :maxdepth: 2

  BNModel
  BNInference
  BNLearning
  BNTools
  Causal
  Classifier


.. toctree::
  :caption: 3- Other graphical models
  :maxdepth: 2

  infdiag
  credalNetwork
  markovRandomField
  PRM

.. toctree::
  :caption: 4- pyAgrum.lib modules
  :maxdepth: 2
  :titlesonly:

  lib.discretizer
  lib.notebook
  lib.image
  lib.explain
  lib.dBN
  pyAgrum.lib

.. toctree::
  :caption: 5- pyAgrum's (experimentals) models
  :maxdepth: 2

  ctbn
  clg

.. toctree::
  :caption: 5- Miscellaneous
  :maxdepth: 2
  :titlesonly:

  random
  fastSyntax
  functions
  listeners
  exceptions

.. toctree::
  :caption: 6- Customizing pyAgrum
  :maxdepth: 1
  :titlesonly:

  PyAgrumConfiguration


Indices and tables
==================

  * :ref:`genindex`
