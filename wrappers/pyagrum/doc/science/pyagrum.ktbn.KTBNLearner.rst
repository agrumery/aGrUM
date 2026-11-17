KTBNLearner learns a `pyagrum.ktbn.KTBN` of a given order :math:`k \geq 2` from complete trajectories (one file per
trajectory, one row per time step). The likelihood of a k-TBN factorizes into the initial slices, the transition
kernel and the atemporal variables, so that, as :cite:`friedman1998learning` does for the prior and transition
networks of a 2-TBN, the three parts are learned independently, each as an ordinary Bayesian network learning problem
solved by `pyagrum.BNLearner` with the chosen algorithm, score, prior and constraints (MIIC by default, key
``BNLearnerPreferredAlgo`` of the section ``learning`` of the configuration).

**The three tables.** From a trajectory of length :math:`L`:

- the *transition table* holds the :math:`L-k+1` sliding windows :math:`(\mathbf{v}_t, \dots, \mathbf{v}_{t+k-1})`, with
  the atemporal values. The variables of the slices :math:`0, \dots, k-2` and the atemporal ones are forced to be roots,
  so that only the arcs entering the slice :math:`k-1` (the kernel) are learned; the counts are then exactly the
  sufficient statistics of the kernel, summed over all the time steps :math:`t \geq k-1`;
- the *initial table* holds one row per trajectory: its first :math:`k-1` time steps. Arcs going back in time are
  forbidden and the atemporal variables are roots;
- the *atemporal table* holds one row per trajectory with the atemporal values; it is used only when there are at least
  two atemporal variables (otherwise their distribution comes from the initial table).

The three learned networks are then glued: the initial slices from the second, the kernel (arcs and CPTs of slice
:math:`k-1`) from the first, the atemporal arcs and CPTs from the third. The constraints given by the user on
:math:`(variable, slice)` nodes are sent to the table that owns their head.

**Atemporal variables.** When they are not given, a variable is classified atemporal if its value never changes within
any trajectory (it may differ between trajectories).

**Implementation limit.** The initial slices are learned from as many rows as there are trajectories: with a few long
trajectories, the initial distribution is poorly estimated, whatever their length. A temporal variable which is
constant within every trajectory is taken for an atemporal one. The domains of the variables are read from the first
trajectory (unless a Bayesian network gives them). With missing values, the rows (windows) holding one are dropped
(``ignoreMissingSymbols``), which is unbiased only when the values are missing completely at random.
