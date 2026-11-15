LoopyBeliefPropagation applies Pearl's message passing for polytrees :cite:`pearl1988probabilistic` to a Bayesian
network with loops. With :math:`\lambda_X(x)` the evidence on :math:`X` (likelihood vector, the indicator of the value
for a hard evidence, 1 without evidence), the messages along each arc :math:`U \to X` are

.. math::

   \pi_X(x) = \sum_{u} P(x \mid u) \prod_{U_i \in \mathrm{pa}(X)} \pi_{U_i \to X}(u_i), \qquad
   \pi_{X \to Y}(x) \propto \pi_X(x)\, \lambda_X(x) \prod_{Z \in \mathrm{ch}(X) \setminus \{Y\}} \lambda_{Z \to X}(x),

.. math::

   \lambda_{X \to U}(u) \propto \sum_{x} \lambda_X(x) \prod_{Z \in \mathrm{ch}(X)} \lambda_{Z \to X}(x)
   \sum_{u' } P(x \mid u, u') \prod_{U_i \neq U} \pi_{U_i \to X}(u_i),

each message being normalized, and the posterior is
:math:`P(x \mid e) \propto \pi_X(x)\,\lambda_X(x) \prod_{Z} \lambda_{Z \to X}(x)`. On a polytree it is exact; with
loops it is an approximation, often good but with no guarantee of convergence :cite:`murphy1999loopy`.

**Choices of pyAgrum.** All the messages start uniform, and a first sweep follows a topological order. Then each
iteration visits all the nodes in a random order (drawn from a generator with a fixed seed, so that two runs give the
same result), each node updating at once all its outgoing messages from the latest messages received (asynchronous
updates). The error of an iteration is the largest Kullback-Leibler divergence (in bits) between the former and the
new value of a message, and the iterations stop when it falls below ``epsilon`` (default :math:`10^{-8}`), when its
relative variation falls below the minimal rate (default :math:`10^{-10}`), or after ``maxIter`` iterations (default
100).
