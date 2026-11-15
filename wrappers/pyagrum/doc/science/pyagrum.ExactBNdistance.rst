ExactBNdistance compares two Bayesian networks :math:`P` and :math:`Q` defined on the same variables (same names, same
domains) by enumerating all their joint configurations :math:`x`, whose probabilities :math:`p = P(x)` and
:math:`q = Q(x)` are given by the chain rule of each network. The cost is the size of the joint domain: the method is
limited to small networks, `pyagrum.MCBNDistance` and `pyagrum.GibbsBNdistance` estimate the same quantities by
sampling. ``compute()`` returns:

- ``klPQ`` and ``klQP``, the Kullback-Leibler divergences :cite:`kullback1951information` in both directions, in bits
  (logarithm in base 2):

  .. math::

     KL(P \| Q) = \sum_{x:\, p > 0} p \log_2 \frac{p}{q}, \qquad KL(Q \| P) = \sum_{x:\, q > 0} q \log_2 \frac{q}{p}.

  A configuration with :math:`p > 0` and :math:`q = 0` makes :math:`KL(P\|Q)` infinite: ``klPQ`` is then
  :math:`+\infty`, and ``errorPQ`` counts these configurations (``klQP`` and ``errorQP`` for :math:`KL(Q\|P)`).

- ``hellinger``, the Hellinger distance :cite:`hellinger1909neue` in the form of :cite:`kokolakis2001bayesian`, with no
  factor 1/2:

  .. math::

     H(P, Q) = \sqrt{\sum_x \left(\sqrt{p} - \sqrt{q}\right)^2} = \sqrt{2 - 2\rho},
     \qquad \rho = \sum_x \sqrt{p\,q},

  where :math:`\rho` is the Bhattacharyya coefficient. :math:`H` lies in :math:`[0, \sqrt 2]`, and is :math:`\sqrt 2`
  times the distance normalized to :math:`[0, 1]` by the factor 1/2 found elsewhere.

- ``bhattacharya``, the Bhattacharyya distance :cite:`kailath1967divergence`, with a natural logarithm (in nats, unlike
  the other quantities): :math:`B(P, Q) = -\ln \rho`, in :math:`[0, +\infty]`, infinite when the supports of :math:`P`
  and :math:`Q` are disjoint.

- ``jensen-shannon``, the Jensen-Shannon divergence :cite:`lin1991divergence` in bits, with :math:`M = (P + Q)/2`:

  .. math::

     JS(P, Q) = \frac{1}{2} KL(P \| M) + \frac{1}{2} KL(Q \| M) \in [0, 1].

  It is symmetric and always finite (:math:`M > 0` wherever :math:`P > 0` or :math:`Q > 0`), hence a safer choice than
  the Kullback-Leibler divergences when the supports differ.
