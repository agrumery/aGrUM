k-order Dynamic Bayesian Networks (k-TBN)
==========================================

A dynamic Bayesian network models a stochastic process by describing how the
distribution over a set of variables at time :math:`t` depends on the past. The
classical `2-TBN` limits that dependency to a single step backward
(:math:`t-1`). A **k-order dynamic Bayesian network** (k-TBN) generalizes this:
a variable at time :math:`t` may depend on any of the :math:`k` most recent
time slices :math:`t, t-1, \ldots, t-k+1`.

Rather than storing an (infinite) unrolled network, a k-TBN stores a compact
**template** made of exactly :math:`k` time slices. This template captures both
the initial distribution (slices :math:`0, \ldots, k-2`) and the transition
kernel (slice :math:`k-1`, reused for every :math:`t \geq k-1` since the process
is time-homogeneous).

Two kinds of variables are distinguished:

* **temporal** variables (processes), which evolve through time and are
  therefore represented by :math:`k` instances (one per slice) in the
  template;
* **atemporal** variables, which are constant through time (e.g. a static
  context parameter) and are represented by a single instance.

Internally, a temporal node is named with bracket notation: the :math:`t`-th
instance of a process ``base`` is ``base[t]`` (e.g. ``\"X[0]\"``, ``\"X[1]\"``).
Atemporal variables keep their bare name. Most of the public API lets you
address a node either that way, or through an explicit ``(base, slice)`` pair,
``slice`` being :attr:`pyagrum.ktbn.KTBN.ATEMPORAL` (``-1``) for an atemporal
variable.

**A minimal example**

.. code-block:: python

   import pyagrum as gum
   import pyagrum.ktbn as ktbn

   model = ktbn.KTBN(2)                      # order k=2
   model.addTemporal("X[2]")                 # a binary process
   model.addAtemporal("C[2]")                # a binary static context
   model.addArc("X", 0, "X", 1)              # X[0] -> X[1]
   model.addArc("C", ktbn.KTBN.ATEMPORAL, "X", 0)
   model.generateCPTs()                      # or fillCPT(...) node by node

   bn10 = model.unroll(10)                   # a plain BayesNet over 10 slices

   ie = ktbn.KTBNInference(model)
   ie.addObservation("X", 1, 1)              # observe X[1] = 1
   ie.addIntervention("C", ktbn.KTBN.ATEMPORAL, 0)   # do(C = 0)
   ie.addTarget("X")
   ie.makeInference(5)
   print(ie.posteriors("X"))                 # P(X[t] | ...) for t = 0..4

**Tutorial**

* `Tutorial on k-order dynamic Bayesian networks <notebooks/75-PyModels_KTBN.ipynb>`_

**Input / Output**

k-TBNs can be saved using the native :ref:`jgum-bgum-format`.

.. code-block:: python

   ktbn.saveKTBN(model, "model.jgum")   # jgum (JSON)
   ktbn.saveKTBN(model, "model.bgum")   # bgum (binary)

**Reference**

.. toctree::
   :maxdepth: 3

   KTBNModel

   KTBNInference

   KTBNLearner
