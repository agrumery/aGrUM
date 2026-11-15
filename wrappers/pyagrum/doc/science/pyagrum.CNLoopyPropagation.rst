CNLoopyPropagation implements L2U :cite:`ide2008approximate`, the loopy version of 2U :cite:`fagiuoli19982u`. 2U
adapts Pearl's propagation to credal networks with binary variables and computes exactly the lower and upper
posteriors of a polytree; L2U iterates its messages on a network with loops, as loopy belief propagation does (see
`pyagrum.LoopyBeliefPropagation`), and its bounds are then approximate, with no guarantee of being inner or outer. It
requires a separately specified binary credal network whose interval bounds have been prepared
(``CredalNet.computeBinaryCPTMinMax``); a network with non-binary variables is first binarized by
``CredalNet.approximatedBinarization``, which adds the outer approximation of the binarization to the one of the loops
:cite:`antonucci2010generalized`.

**The messages.** With binary variables, a distribution is a single number, and every message is an interval:

- a message :math:`\pi` from a parent :math:`U` to :math:`X` bounds :math:`P(U = 1)` given the evidence on the side
  of :math:`U`;
- a message :math:`\Lambda` from a child to :math:`X` bounds the likelihood ratio :math:`\lambda(1)/\lambda(0)` of the
  evidence on the side of the child; the ratio of :math:`X` is the product of those of its children (lower bounds
  together, upper bounds together).

The bounds of :math:`\pi(X)` are obtained by enumerating the combinations of the extreme values of the parents'
messages, with the lower (resp. upper) conditional probabilities :math:`P(X = 1 \mid u)`; the messages to the parents
are computed in the same way. The posterior of :math:`X` combines the two intervals, monotonically:

.. math::

   \underline{P}(X = 1 \mid e) = \frac{1}{1 + \left(1/\underline{\pi} - 1\right)/\underline{\Lambda}}, \qquad
   \overline{P}(X = 1 \mid e) = \frac{1}{1 + \left(1/\overline{\pi} - 1\right)/\overline{\Lambda}},

and :math:`P(X = 0 \mid e)` follows by complement. Only hard evidence is accepted (an observed variable has
:math:`\Lambda = \infty` for the value 1, :math:`0` for the value 0).

**Initialization and schedule.** The :math:`\pi` intervals are first propagated in a topological order and every
:math:`\Lambda` is 1, so that without evidence the first values are the forward bounds. By default
(``nodeToNeighbours``), only the nodes whose information changed send messages at the next iteration; the other
orders send messages along all the arcs, in a fixed (``ordered``) or a new random (``randomOrder``) order at each
iteration.

**The stopping criterion.** After each iteration, :math:`\varepsilon` is the largest change of a lower or upper
posterior over all the variables. The propagation stops when no node has a new message to send (``nodeToNeighbours``),
or with the criteria of the approximation scheme: :math:`\varepsilon` at most ``epsilon`` (0.05 by default), relative
change of :math:`\varepsilon` between two iterations below ``minEpsilonRate`` (0.01), ``maxIter`` (10000) iterations;
there is no time limit by default. On a network with loops, the convergence of L2U is not proved, although it is
usually observed :cite:`antonucci2010generalized`.
