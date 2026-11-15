InformationTheory computes the usual quantities of information theory :cite:`cover2006elements` for three disjoint
sets of variables :math:`X`, :math:`Y` and :math:`Z`, from a single joint posterior :math:`P(X, Y, Z \mid e)` given by
the inference passed as argument (so under its evidence :math:`e`). All the logarithms are in base 2 (bits), with
:math:`0 \log 0 = 0`:
:math:`H(X) = -\sum_x p(x)\log_2 p(x)`, :math:`H(X \mid Y) = -\sum_{x,y} p(x,y)\log_2 p(x \mid y)`,
:math:`I(X;Y) = \sum_{x,y} p(x,y)\log_2 \frac{p(x,y)}{p(x)p(y)}` and
:math:`I(X;Y \mid Z) = \sum_{x,y,z} p(x,y,z)\log_2 \frac{p(z)\,p(x,y,z)}{p(x,z)\,p(y,z)}`.

The variation of information of :cite:`meila2007comparing` is
:math:`VI(X,Y) = H(X \mid Y) + H(Y \mid X) = H(X) + H(Y) - 2\,I(X;Y)`, a distance between the two sets of variables.
