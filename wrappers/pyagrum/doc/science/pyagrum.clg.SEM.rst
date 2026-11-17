SEM reads and writes a `pyagrum.clg.CLG` as a text of linear structural equations :cite:`pearl2009causality`, one per
variable:
``X = c + b1*Y1 + b2 Y2 [s]`` stands for :math:`X = c + b_1 Y_1 + b_2 Y_2 + \varepsilon_X` with
:math:`\varepsilon_X \sim \mathcal{N}(0, s^2)`, and ``X = c[s]`` for a variable without parent.

**Conventions.**

- The constant :math:`c` is the intercept ``mu`` of the variable (0 when it is absent), each term :math:`b\,Y` is an
  arc :math:`Y \to X` of coefficient :math:`b`, and the number in brackets is the standard deviation of the noise,
  not its variance.
- The ``*`` is optional; several constants, or several terms of the same parent, are summed (``F = E + 2E [1]`` gives
  a coefficient 3); a term whose coefficients sum to 0 is refused, as is any null coefficient of a CLG.
- ``#`` starts a comment; a line without ``=`` continues the previous equation and is appended to it as it is
  (``2`` followed by a line ``B[2]`` reads ``2B[2]``).
- Numbers may use the exponent notation (``1e-05``), read before a variable name: ``2e3A`` is :math:`2000\,A`,
  and 2 times a variable ``e3`` is written ``2*e3``.
- A parent without equation, or a part of a line that is not a term, raises a ``SyntaxError``.
- When writing, the equations follow a topological order, a coefficient 1 is omitted and the others are written
  ``b*Y``, so that the text is read back as the same model.
