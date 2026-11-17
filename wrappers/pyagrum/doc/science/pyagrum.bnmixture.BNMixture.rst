A BNMixture is a finite mixture of Bayesian networks :math:`B_1, \dots, B_m` on the same variables (same names, same
domains), with non-negative weights :math:`w_i`: it stands for the distribution

.. math::

   P(x) = \frac{\sum_i w_i\, P_{B_i}(x)}{\sum_i w_i}.

The weights need not sum to 1 (``normalize`` divides them by their sum). Adding a network equal to one already in
the mixture adds its weight to that of the existing network instead of storing it twice. The reference network is the
one of highest weight: it only serves to display the mixture and to look up its variables. The confidence in an arc
shown by the displays is the weighted frequency of the networks that contain it, as the confidence in a feature of
:cite:`friedman1999data` (see `pyagrum.bnmixture.BootstrapMixture`).
