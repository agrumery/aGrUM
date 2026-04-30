Abstract Syntax Tree for Do-Calculus
=====================================

The causal module computes every causal query as an Abstract Syntax Tree (AST) that represents
the exact probabilistic computations needed to answer a causal query.

The AST is an internal C++ data structure built by the do-calculus identification algorithm.
It is organised as a hierarchy of node types:

- **Leaf nodes**: joint probability :math:`P(X,Y,\ldots)` and posterior probability :math:`P(X \mid Y,\ldots)`
- **Binary operations**: :math:`+`, :math:`-`, :math:`\times`, :math:`\div`
- **Sum-out node**: marginalisation over a set of variables

Accessing the AST
-----------------

:func:`pyagrum.causalImpact` returns a tuple ``(formula, tensor, explanation)`` where ``formula``
is a :class:`pyagrum.CausalImpact` object carrying the identified AST. From it you can:

.. code-block:: python

   import pyagrum as gum

   bn = gum.fastBN("X->Y->Z;X->Z")
   cm = gum.CausalModel(bn)

   formula, tensor, explanation = gum.causalImpact(cm, on="Z", doing="X")

   # Render the identified formula as a LaTeX string
   print(formula.toLatex())

   # Inspect the AST as a nested dict
   print(formula.toDict())

   # Pretty-print the AST in the terminal
   formula.print_ast()

.. note::
   The AST node classes are internal to the C++ library and are not directly exposed in
   the Python API. Use :func:`pyagrum.causalImpact` to obtain and inspect a causal formula.

.. seealso::

   :doc:`CausalInference`
      :func:`pyagrum.causalImpact` and :class:`pyagrum.CausalImpact` — the entry point for obtaining an AST.
