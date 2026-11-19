ForwardSamplingInference estimates the long-run distribution of the variables of a `pyagrum.ctbn.CTBN` from simulated
trajectories.

**Sampling a trajectory.** Each variable starts in a state drawn uniformly. Then, at each step, every variable
:math:`X` draws a waiting time from the exponential law of parameter :math:`q_{x \mid u}` (its current state :math:`x`,
the current values :math:`u` of its parents, see `pyagrum.ctbn.CIM`); the variable with the shortest time changes,
to :math:`x'` with probability :math:`q_{xx' \mid u} / q_{x \mid u}`. This is the first-reaction method of
:cite:`gillespie1976general` applied to the variables, the new state being then drawn as in his direct method: the
minimum of independent exponential times has the law of the waiting time of the joint
process and is reached by each variable in proportion to its rate, so the trajectory is an exact sample of the CTBN.
All the times are drawn again after each transition, which is exact as the exponential law has no memory.

**The estimator.** The first ``burnIn`` transitions (100 by default) are discarded (fewer if the process reaches an
absorbing state, where all the exit rates are 0). Then, over a duration
:math:`T` (``timeHorizon``, 5000 by default), the posterior of a value :math:`x` is the fraction of time spent in it,

.. math::

   \hat P(X = x) = \frac{1}{T} \int_0^T \mathbb{1}[X(s) = x] \, ds ,

which converges to the stationary distribution when the process is ergodic, not to the distribution at a given time.
``averageInference`` and ``makeParallelInference`` add the times of several independent trajectories before
normalizing.

**Implementation limit.** There is no stopping criterion: the precision is set by the total simulated time only, and
the burn-in counts transitions, not time. No evidence can be given.
