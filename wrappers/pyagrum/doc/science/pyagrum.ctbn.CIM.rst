A conditional intensity matrix (CIM) :cite:`nodelman2002continuous` gives, for each configuration :math:`u` of the
parents of :math:`X`, the intensity matrix :math:`Q_{X \mid u}` of a homogeneous Markov process: the off-diagonal
entries :math:`q_{xx' \mid u} \geq 0` are the rates of the transitions :math:`x \rightarrow x'` and each row sums to 0,
:math:`q_{xx \mid u} = -q_{x \mid u} = -\sum_{x' \neq x} q_{xx' \mid u}`. While its parents keep the values :math:`u`,
:math:`X` stays in :math:`x` for a time of exponential law of parameter :math:`q_{x \mid u}` (mean
:math:`1/q_{x \mid u}`), then moves to :math:`x'` with probability :math:`q_{xx' \mid u} / q_{x \mid u}`. A state with
:math:`q_{x \mid u} = 0` is absorbing. The product ``*`` of two CIMs is their amalgamation (see `pyagrum.ctbn.CTBN`).
