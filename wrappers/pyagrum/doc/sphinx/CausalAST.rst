Abstract Syntax Tree for Do-Calculus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The pyCausal package compute every causal query into an Abstract Syntax Tree (CausalFormula) that represents the exact computations to be done in order to answer to the probabilistic causal query.

The different types of node in an CausalFormula are presented below and are organized as a hierarchy of classes from :class:`pyagrum.causal.ASTtree`.

.. inheritance-diagram:: pyagrum.causal.ASTplus pyagrum.causal.ASTminus pyagrum.causal.ASTdiv pyagrum.causal.ASTmult pyagrum.causal.ASTsum pyagrum.causal.ASTjointProba pyagrum.causal.ASTposteriorProba
    :top-classes: pyagrum.causal.ASTtree


Internal node structure
#######################

.. autoclass:: pyagrum.causal.ASTtree

.. autoclass:: pyagrum.causal.ASTBinaryOp

Basic Binary Operations
#######################

.. autoclass:: pyagrum.causal.ASTplus

.. autoclass:: pyagrum.causal.ASTminus

.. autoclass:: pyagrum.causal.ASTdiv

.. autoclass:: pyagrum.causal.ASTmult

Complex operations
##################

.. autoclass:: pyagrum.causal.ASTsum

.. autoclass:: pyagrum.causal.ASTjointProba

.. autoclass:: pyagrum.causal.ASTposteriorProba
