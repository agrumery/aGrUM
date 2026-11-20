SimpleInference computes exactly the distribution at time :math:`t` of a `pyagrum.ctbn.CTBN` from its joint
intensity matrix :math:`Q_{\mathbf{X}}` (see `pyagrum.ctbn.CTBN`) :cite:`nodelman2002continuous`:

.. math::

   P(\mathbf{X}(t)) = P(\mathbf{X}(0)) \exp(t\, Q_{\mathbf{X}}),

with :math:`P(\mathbf{X}(0))` uniform over the joint states. The posterior of a variable is a marginal of
:math:`P(\mathbf{X}(t))`. With the default :math:`t = 5000`, it is the stationary distribution of an ergodic
process whenever the process mixes in a time much shorter than :math:`t` (rates not too small).

**Implementation limit.** The joint matrix has :math:`\prod_X |\mathrm{Val}(X)|` rows: only small models can be
handled. No evidence can be given.
