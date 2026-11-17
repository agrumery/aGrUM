KTBNInference is the exact interface algorithm of :cite:`murphy2002dynamic` (Section 3.4), generalized from 2-TBNs to
the k-TBNs of `pyagrum.ktbn.KTBN`, to atemporal variables and to interventions. The model is never unrolled: the
memory and the time per slice do not depend on the horizon.

**The forward interface.** In a 2-TBN, the forward interface :math:`I_t` is the set of the variables of slice :math:`t`
which have a child in slice :math:`t+1`; it d-separates the past from the future :cite:`murphy2002dynamic`. For a k-TBN,
:math:`I_t` is the set of the instances :math:`V_s`, :math:`s \leq t`, which still have a child after :math:`t` (with a
kernel reaching :math:`\ell` slices back, the last :math:`\ell` instances of :math:`V`), plus every atemporal variable,
which is a parent at every slice and so never leaves the interface.

**Windows.** The window of slice :math:`t` is :math:`H_t = I_{t-1} \cup \mathbf{V}_t`. It is compiled once into a
junction tree (moralization of the families, :math:`I_{t-1}` and :math:`I_t` each forced into a clique, triangulation).
By time homogeneity, all the windows from slice :math:`k` on have the same shape: one junction tree serves the whole
repeating part, and only the first :math:`k` slices get their own. Inside a window, Shafer-Shenoy message passing is
used (see `pyagrum.ShaferShenoyInference`). The forward message is

.. math::

   m_t(I_t) = \frac{1}{c_t} \sum_{H_t \setminus I_t} m_{t-1}(I_{t-1})
              \prod_{V \in \mathbf{V}_t} P(v_t \mid \mathrm{pa}_{V_t})\,\lambda_{V_t}(v_t),

where :math:`\lambda` is the likelihood of an observation (hard or soft) and :math:`c_t` normalizes :math:`m_t`, so
that :math:`\log P(\mathbf{e} \mid do(\cdot)) = \sum_t \log c_t` (``logObservationProbability``). When observations exist,
a backward sweep sends the symmetric messages :math:`r_t(I_{t-1})`, and each posterior is read in the window where
:math:`m_{t-1}` and :math:`r_{t+1}` meet (smoothing). Without observation, the backward messages are uniform, so only
the forward sweep is run.

**Interventions.** :math:`do(V_t = v)` replaces the CPT of :math:`V_t` by a point mass on :math:`v`, cutting it from
its causes :cite:`pearl2009causality`: the effect reaches only its descendants, while an observation also revises its
ancestors. Both can be combined.

**Targets.** The computation is restricted to the requisite variables: the targets (all the variables by default), the
observed variables and their ancestors.

**Implementation limit.** The cost per slice is exponential in the width of the window, which grows with the size of
the interface: every atemporal variable (and every lag of the kernel) widens it. Observations or interventions after
the horizon are ignored. With nearly deterministic CPTs, the backward messages may underflow on long horizons, which
raises an error.
