A Markov random field represents a distribution as a normalized product of non negative factors,

.. math::

   P(x) = \frac{1}{Z} \prod_{f} \phi_f(x_f), \qquad Z = \sum_x \prod_f \phi_f(x_f),

each factor :math:`\phi_f` being defined on a set of variables :math:`x_f` (at most one factor per set). The graph
of the model links two variables iff they appear in a common factor; by the Hammersley-Clifford theorem, the positive
distributions that factorize over the cliques of an undirected graph are exactly those that satisfy its Markov
properties :cite:`besag1974spatial`. The factors are not normalized and have no probabilistic meaning of their own.

``fromBN`` uses the conditional probability tables of a Bayesian network as factors (then :math:`Z = 1`), so that the
graph is the moral graph of the Bayesian network :cite:`lauritzen1988local`; ``toDotAsFactorGraph`` draws the model
as a factor graph, a bipartite graph between the variables and the factors :cite:`kschischang2001factor`.
