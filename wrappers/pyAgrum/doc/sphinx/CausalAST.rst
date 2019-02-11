Abstract Syntax Tree for Do-Calculus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The pyCausal package compute every causal query into an Abstract Syntax Tree (CausalFormula) that represents the exact computations to be done in order to answer to the probabilistic causal query.

The different types of node in an CausalFormula are presented below and are organized as a hierarchy of classes from :class:`pyAgrum.causal.ASTtree`.

.. inheritance-diagram:: pyAgrum.causal.ASTplus pyAgrum.causal.ASTminus pyAgrum.causal.ASTdiv pyAgrum.causal.ASTmult pyAgrum.causal.ASTsum pyAgrum.causal.ASTjointProba pyAgrum.causal.ASTposteriorProba
    :top-classes: pyAgrum.causal.ASTtree


Internal node structure
#######################

.. autoclass:: pyAgrum.causal.ASTtree

.. autoclass:: pyAgrum.causal.ASTBinaryOp

Basic Binary Operations
#######################

.. autoclass:: pyAgrum.causal.ASTplus

.. autoclass:: pyAgrum.causal.ASTminus

.. autoclass:: pyAgrum.causal.ASTdiv

.. autoclass:: pyAgrum.causal.ASTmult

Complex operations
##################

.. autoclass:: pyAgrum.causal.ASTsum

.. autoclass:: pyAgrum.causal.ASTjointProba

.. autoclass:: pyAgrum.causal.ASTposteriorProba
