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
  tensor

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

Glossary
========

.. glossary::

    BN
      Bayesian Network

    UG
      Undirected Graph

    dBN
      Dynamic Bayesian Network

    DAG
      Directed Acyclic Graph

    PDAG
      Partially Directed Acyclic Graph

    EM
      Expectation-Maximization algorithm, dealing with missng data

    MLE
      Maximum Likelihood Estimation

    AIC
      Akaike Information Criterion

    BIC
      Bayesian Information Criterion

    O3PRM
      Open Object Oriented Probabilistic Relational Model, Object oriented language for specification of PRM

    MRF
      Markov Random Field

    ID
      Influence Diagram

    LIMID
      Limited Memory Influence Diagram

    CN
      Credal Network

    dBN
      Dynamic Bayesian Network

    PRM
      Probabilistic Relational Model

    API
      Application Programming Interface

    Graphical model
      A probabilistic model for which a graph expresses the conditional dependence structure between random variables.

    Bayesian network
      A probabilistic graphical model that represents a set of random variables and their conditional dependencies in the form of a directed acyclic graph  (DAG).

    Markov random field
      A type of undirected graphical model that represents a set of random variables and their conditional dependencies in the form of an undirected graph (UG).

    Influence diagram
      A type of graphical model that represents a set of random variables and their conditional dependencies in the form of a directed acyclic graph.

    Limited memory influence diagram
      A type of influence diagram

    Credal network
      A type of graphical model that represents a set of random variables and their conditional dependencies in the form of a directed acyclic graph with sets of probability distributions.

    Dynamic Bayesian network
      A type of graphical model that represents a set of random variables and their conditional dependencies in the form of a directed acyclic graph that changes over (discrete) time. It is a generalisation of Markov Chain (with partial observation).

    Probabilistic relational model
      A type of graphical model that represents a set of random variables and their conditional dependencies using graphs and patterns (such as relational databases, or Object Oriented programming language).

Indices and tables
==================

  * :ref:`genindex`
