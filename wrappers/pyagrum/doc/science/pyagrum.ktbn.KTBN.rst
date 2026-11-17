A k-TBN is a time-homogeneous dynamic Bayesian network :cite:`dean1989model` given by a template of :math:`k`
consecutive time slices :math:`0, \dots, k-1`. It generalizes the 2-TBN of :cite:`friedman1998learning` and
:cite:`murphy2002dynamic` (a prior network for the first slice and a transition network between two slices): a
variable at time :math:`t` may depend on the variables of the slices :math:`t-k+1, \dots, t`, so that the process is a
Markov chain of order :math:`k-1` (:math:`k = 2` is the usual first-order case).

**The template.** The slices :math:`0, \dots, k-2` hold the initial distribution, the slice :math:`k-1` holds the
transition kernel, which is reused at every later time step. With :math:`\mathbf{V}_t` the temporal variables at time
:math:`t` and :math:`\mathbf{C}` the atemporal ones, the joint distribution over a horizon :math:`T \geq k` is

.. math::

   P(\mathbf{c}, \mathbf{v}_{0:T-1}) = \prod_{C \in \mathbf{C}} P(c \mid \mathrm{pa}_C)
     \prod_{t=0}^{k-2} \prod_{V} P_t(v_t \mid \mathrm{pa}_{V_t})
     \prod_{t=k-1}^{T-1} \prod_{V} P_{k-1}(v_t \mid \mathrm{pa}_{V_t}),

where the parents of :math:`V_t`, for :math:`t \geq k-1`, are those of :math:`V_{k-1}` in the template, shifted by
:math:`t-k+1` slices (the lags are kept). An arc never goes back in time; arcs inside a slice are allowed.

**Atemporal variables** (pyAgrum's extension) are constant through time: they have a single instance, only atemporal
parents, and may be parents of temporal variables at any slice. Since they are shared by all the slices, they couple
the whole horizon (see `pyagrum.ktbn.KTBNInference`).

**Unrolling** over :math:`T` slices copies the slices :math:`0, \dots, k-1` of the template and gives every slice
:math:`t \geq k` the families and the CPTs of the slice :math:`k-1`. The summary graph keeps only the arcs that enter
the slice :math:`k-1` (the repeated pattern), labelled by their lag.
