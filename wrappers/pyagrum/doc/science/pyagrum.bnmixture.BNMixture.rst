A BNMixture is a finite mixture of Bayesian networks on the same variables (same names, same domains), the network
:math:`i` having the distribution :math:`P_i` and a non-negative weight :math:`w_i`. It stands for

.. math::

   P(x) = \frac{\sum_i w_i\, P_i(x)}{\sum_i w_i}.

The weights need not sum to 1 (``normalize`` divides them by their sum). Adding a network equal to one already in
the mixture adds its weight to that of the existing network instead of storing it twice. The reference network only
serves to display the mixture and to look up its variables: it is the first network added, then the one of highest
weight after ``updateRef``. The confidence in an arc shown by the displays is the weighted frequency of the networks
that contain it (`pyagrum.bnmixture.BNMixture.arcConfidence`), as the confidence in a feature of
:cite:`friedman1999data` (see `pyagrum.bnmixture.BootstrapMixture`).
