A CLG of pyAgrum is a linear Gaussian Bayesian network: all its variables are continuous and each variable
:math:`X` is a linear function of its parents plus an independent Gaussian noise (a linear structural equation),

.. math::

   X = \mu_X + \sum_{Y \in \mathrm{Pa}(X)} \beta_{YX}\, Y + \varepsilon_X, \qquad
   \varepsilon_X \sim \mathcal{N}(0, \sigma_X^2),

so that the conditional density of :math:`X` is :math:`\mathcal{N}(\mu_X + \sum_Y \beta_{YX}\, y, \sigma_X^2)`. This
is the representation of the Gaussian influence diagrams of :cite:`shachter1989gaussian` (a regression coefficient
per arc, a conditional variance per variable), up to the parametrization of the mean: :cite:`shachter1989gaussian`
attach to each variable its unconditional mean, while :math:`\mu_X` here is the intercept of its equation. The joint
distribution is multivariate normal: with :math:`B = (\beta_{YX})` the matrix of the arc coefficients and
:math:`D = \mathrm{diag}(\sigma_X^2)`, the vector of the variables is :math:`(I - B^\top)^{-1}(\mu + \varepsilon)`,
hence the mean :math:`m = (I - B^\top)^{-1}\mu` and the covariance
:math:`\Sigma = (I - B^\top)^{-1} D\, (I - B^\top)^{-\top}`.

**Conventions of pyAgrum.**

- The parameter ``mu`` of a variable is the intercept :math:`\mu_X` of its equation: it is the mean of :math:`X` only
  when :math:`X` has no parent (the means of the other variables are given by `pyagrum.clg.CLGVariableElimination`).
- The parameter ``sigma`` is the standard deviation :math:`\sigma_X` of the noise (the conditional standard
  deviation of :math:`X` given its parents), not its variance. It must be positive: the densities of the model, and
  its exact inference, need :math:`\sigma_X > 0`.
- The coefficient of an arc cannot be 0: a variable without influence on another one is not its parent.
- The log-likelihood of a database is :math:`\sum_{k} \sum_X \ln \varphi(x^{(k)};\, \mu_X + \sum_Y \beta_{YX}
  y^{(k)}, \sigma_X^2)` (natural logarithm, :math:`\varphi` the normal density), the maximized quantity of the
  parameter estimation of `pyagrum.clg.learning.CLGLearner`.
- A CLG is written as text with the syntax of `pyagrum.clg.SEM`.

**Implementation limit.** A deterministic variable (a null standard deviation) cannot be represented: a variable
added with :math:`\sigma_X = 0`, or a database with a constant column given to `pyagrum.clg.learning.CLGLearner`, is
refused.
