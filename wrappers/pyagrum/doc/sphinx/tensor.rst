Tensor and Instantiation
============================

:py:class:`pyagrum.Tensor` is a multi-dimensional array with a :py:class:`pyagrum.DiscreteVariable` associated to each dimension.
It is used to represent probabilities and utilities tables in aGrUMs' multidimensional (graphical) models with some conventions.

* The data are stored by iterating over each variable in the sequence.

.. code-block:: python

    >>> import pyagrum as gum
    >>> a=gum.RangeVariable("A","variable A",1,3)
    >>> b=gum.RangeVariable("B","variable B",1,2)
    >>> p=gum.Tensor().add(a).add(b).fillWith([1,2,3,4,5,6])
    >>> print(p)
          ||  A                          |
    B     ||1        |2        |3        |
    ------||---------|---------|---------|
    1     || 1.0000  | 2.0000  | 3.0000  |
    2     || 4.0000  | 5.0000  | 6.0000  |

* If a :py:class:`pyagrum.Tensor` with the sequence of :py:class:`pyagrum.DiscreteVariable` X,Y,Z represents a conditional probability Table (CPT), it will be P(X|Y,Z).


.. code-block:: python

    >>> print(p.normalizeAsCPT())
          ||  A                          |
    B     ||1        |2        |3        |
    ------||---------|---------|---------|
    1     || 0.1667  | 0.3333  | 0.5000  |
    2     || 0.2667  | 0.3333  | 0.4000  |


* For addressing and looping in a :py:class:`pyagrum.Tensor` structure, pyAgrum provides :py:class:`pyagrum.Instantiation` class which represents a multi-dimensionnal index.

.. code-block:: python

    >>> I=gum.Instantiation(p)
    >>> print(I)
    <A:1|B:1>
    >>> I.inc();print(I)
    <A:2|B:1>
    >>> I.inc();print(I)
    <A:3|B:1>
    >>> I.inc();print(I)
    <A:1|B:2>
    >>> I.setFirst();print(f"{I} -> {p.get(I)}")
    <A:1|B:1> -> 0.16666666666666666
    >>> I["B"]="2";print(f"{I} -> {p.get(I)}")
    <A:1|B:2> -> 0.26666666666666666

* :py:class:`pyagrum.Tensor` include tensor operators (see for instance this `notebook <https://pyagrum.readthedocs.io/en/latest/notebooks/93-Tools_tensors.html>`_).

.. code-block:: python

    >>> c=gum.RangeVariable("C","variable C",1,5)
    >>> q=gum.Tensor().add(a).add(c).fillWith(1)
    >>> print(p+q)
                 ||  A                          |
    C     |B     ||1        |2        |3        |
    ------|------||---------|---------|---------|
    1     |1     || 1.1667  | 1.3333  | 1.5000  |
    2     |1     || 1.1667  | 1.3333  | 1.5000  |
    3     |1     || 1.1667  | 1.3333  | 1.5000  |
    4     |1     || 1.1667  | 1.3333  | 1.5000  |
    5     |1     || 1.1667  | 1.3333  | 1.5000  |
    1     |2     || 1.2667  | 1.3333  | 1.4000  |
    2     |2     || 1.2667  | 1.3333  | 1.4000  |
    3     |2     || 1.2667  | 1.3333  | 1.4000  |
    4     |2     || 1.2667  | 1.3333  | 1.4000  |
    5     |2     || 1.2667  | 1.3333  | 1.4000  |
    >>> print((p*q).sumOut(["B","C"])) # marginalize p*q over B and C(using sum)
      A                          |
    1        |2        |3        |
    ---------|---------|---------|
     2.1667  | 3.3333  | 4.5000  |

Instantiation
-------------

.. autoclass:: pyagrum.Instantiation

Tensor
------

.. autoclass:: pyagrum.Tensor
		:exclude-members: populate
