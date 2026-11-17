SimpleInference is the exact inference of a `pyagrum.ctbn.CTBN` through its joint intensity matrix
:cite:`nodelman2002continuous`: all the CIMs are amalgamated into :math:`Q_{\mathbf{X}}`, and the distribution at time
:math:`t` is

.. math::

   P(\mathbf{X}(t)) = P_0 \exp(t\, Q_{\mathbf{X}}),

computed with a matrix exponential, :math:`P_0` being the uniform distribution over the joint states. The posterior of
a variable is the marginal of :math:`P(\mathbf{X}(t))`. With the default :math:`t = 5000`, the result is, in practice,
the stationary distribution of an ergodic process.

**Implementation limit.** The joint matrix has :math:`\prod_X |\mathrm{Val}(X)|` rows: only small models can be
handled. No evidence can be given.
