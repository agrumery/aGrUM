MCBNDistance estimates the quantities of `pyagrum.ExactBNdistance` (definitions, logarithm bases, ``errorPQ``) from
:math:`N` independent samples :math:`x^{(k)}` of :math:`P`, drawn forward in a topological order (logic sampling
:cite:`henrion1988propagating`). With :math:`p_k = P(x^{(k)})`, :math:`q_k = Q(x^{(k)})`, :math:`r_k = q_k / p_k` and
:math:`m_k = (p_k + q_k)/2`, a sum over the configurations weighted by :math:`q` becomes a mean of :math:`r_k`
(importance weights):

.. math::

   \widehat{KL}(P\|Q) = \frac{1}{N} \sum_k \log_2 \frac{p_k}{q_k}, \qquad
   \widehat{KL}(Q\|P) = \frac{1}{N} \sum_k r_k \log_2 r_k, \qquad
   \hat\rho = \frac{1}{N} \sum_k \sqrt{r_k},

.. math::

   \hat H = \sqrt{\max(0,\, 2 - 2\hat\rho)}, \qquad
   \hat B = -\ln \hat\rho, \qquad
   \widehat{JS} = \frac{1}{2N} \sum_k \left(\log_2 \frac{p_k}{m_k} + r_k \log_2 \frac{q_k}{m_k}\right)
                 + \frac{1}{2}\left(1 - \frac{1}{N} \sum_k r_k\right).

(The last term of :math:`\widehat{JS}` is added only when a parameter of :math:`P` is null, and :math:`\widehat{JS}`
is kept in :math:`[0, 1]`.)

A sample with :math:`q_k = 0` is counted in ``errorPQ`` and in :math:`N`; it proves that :math:`KL(P\|Q)` is infinite
(``klPQ`` is then :math:`+\infty`, as in the exact computation), and its term in :math:`\widehat{JS}` is
:math:`\log_2 (p_k/m_k) = 1`. The Hellinger distance uses :math:`H^2 = 2 - 2\rho`: the configurations where
:math:`p = 0` add nothing to :math:`\rho`, so that :math:`\hat\rho` estimates it without bias.

**Implementation limits.** A configuration with :math:`p = 0` is never drawn. Hence ``errorQP`` stays 0 even when
:math:`KL(Q\|P)` is infinite. :math:`\widehat{JS}` takes the mass :math:`Q(p = 0)` into account through its last term,
since the mean of the :math:`r_k` estimates :math:`Q(p > 0)`; this term is added only when a parameter of :math:`P` is
null (otherwise :math:`Q(p=0) = 0` exactly), and :math:`\widehat{JS}` is kept in :math:`[0, 1]`.
:math:`\widehat{KL}(Q\|P)`, this last term and :math:`\hat\rho` rely on the weights :math:`r_k`, and are imprecise when
:math:`Q` gives a large probability to configurations that are rare under :math:`P`.

**Stopping criterion.** After each sample with :math:`q_k > 0`, the error is the change of :math:`\widehat{KL}(P\|Q)`
caused by this sample. The sampling stops when this error, checked every ``periodSize`` (200) samples, is below
``epsilon`` (:math:`10^{-5}`), when its relative change between two checks is below ``minEpsilonRate``
(:math:`10^{-10}`), or at the limit of iterations (:math:`10^7`) or of time. It is a criterion on the stability of
:math:`\widehat{KL}(P\|Q)` only, not a confidence bound: the other quantities may be less precise.
