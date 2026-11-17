CLGVariableElimination computes exact posterior densities in a `pyagrum.clg.CLG` by sum-product variable elimination
on Gaussian potentials written in canonical form, the canonical characteristics :math:`(g, h, K)` of
:cite:`lauritzen1992propagation`:

.. math::

   \phi(x) = \exp\left(g + h^\top x - \tfrac{1}{2}\, x^\top K x\right).

**From the model to the potentials.** The conditional density of :math:`X` with intercept :math:`\mu`, coefficients
:math:`\beta` (a column vector, one entry per parent) and noise variance :math:`\gamma = \sigma^2` is the canonical
form on :math:`(X, \mathrm{Pa}(X))`

.. math::

   K = \frac{1}{\gamma}\begin{pmatrix} 1 & -\beta^\top \\ -\beta & \beta\beta^\top \end{pmatrix}, \qquad
   h = \frac{\mu}{\gamma}\begin{pmatrix} 1 \\ -\beta \end{pmatrix}, \qquad
   g = -\frac{1}{2}\left(\frac{\mu^2}{\gamma} + \ln(2\pi\gamma)\right).

**Operations.** A product adds the :math:`(K, h, g)` of the factors (extended by zeros to the union of their
variables). Observing :math:`Y = y` turns :math:`(K, h, g)` on :math:`(X, Y)` into :math:`K_{XX}`,
:math:`h_X - K_{XY}\, y` and :math:`g + h_Y^\top y - \frac{1}{2} y^\top K_{YY}\, y`. Integrating :math:`Y` out gives

.. math::

   K_{XX} - K_{XY} K_{YY}^{-1} K_{YX}, \qquad h_X - K_{XY} K_{YY}^{-1} h_Y, \qquad
   g + \tfrac{1}{2}\left(|Y| \ln(2\pi) - \ln\det K_{YY} + h_Y^\top K_{YY}^{-1} h_Y\right).

**The algorithm.** All the potentials are first reduced by the evidence; the other non-target variables are then
eliminated (product of the potentials that contain the variable, then integration) in the elimination order given by
`pyagrum.JunctionTreeGenerator` for the graph of the model; the remaining potentials are multiplied and the result is
normalized by its integral. The posterior of the targets is the Gaussian of covariance :math:`K^{-1}` and mean
:math:`K^{-1} h`: it is exact, not an approximation.

**Conventions.** Evidence is hard: one observed value per variable (soft evidence is not supported). The posterior
of an observed variable is returned as a Gaussian centred on the observed value, with a null standard deviation; the
canonical posterior of an observed variable is refused.

**Implementation limit.** No junction tree is kept: each query runs a whole elimination. The canonical forms need a
positive standard deviation :math:`\sigma > 0` for every variable, which `pyagrum.clg.CLG` ensures.
