Potential and Instantiation
============================

:py:class:`pyAgrum.Potential` is a multi-dimensional array with a :py:class:`pyAgrum.DiscreteVariable` associated to each dimension.
It is used to represent probabilities and utilities tables in aGrUMs' multidimensional (graphical) models with some conventions.

* The data are stored by iterating over each variable in the sequence.

.. code-block:: python

    >>> a=gum.RangeVariable("A","variable A",1,3)
    >>> b=gum.RangeVariable("B","variable B",1,2)
    >>> p=gum.:py:class:`pyAgrum.Potential`().add(a).add(b).fillWith([1,2,3,4,5,6]);
    >>> print(p)
    <A:1|B:1> :: 1 /<A:2|B:1> :: 2 /<A:3|B:1> :: 3 /<A:1|B:2> :: 4 /<A:2|B:2> :: 5 /<A:3|B:2> :: 6

* If a :py:class:`pyAgrum.Potential` with the sequence of :py:class:`pyAgrum.DiscreteVariable` X,Y,Z represents a conditional probability Table (CPT), it will be P(X|Y,Z).


.. code-block:: python

    >>> print(p.normalizeAsCPT())
    <A:1|B:1> :: 0.166667 /<A:2|B:1> :: 0.333333 /<A:3|B:1> :: 0.5 /<A:1|B:2> :: 0.266667 /<A:2|B:2> :: 0.333333 /<A:3|B:2> :: 0.4


* For addressing and looping in a :py:class:`pyAgrum.Potential` structurel, pyAgrum provides :py:class:`Instantiation` class which represents a multi-dimensionnal index.

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
    >>> I.setFirst();print("{} -> {}".format(I,p.get(I)))
    <A:1|B:1> -> 0.16666666666666666
    >>> I["B"]="2";print("{} -> {}".format(I,p.get(I)))
    <A:1|B:2> -> 0.26666666666666666

* :py:class:`pyAgrum.Potential` include tensor operators (see for instance this `notebook <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/05-potentials.ipynb.html>`_).

.. code-block:: python

    >>> c=gum.RangeVariable("C","variable C",1,5)
    >>> q=gum.Potential().add(a).add(c).fillWith(1)
    >>> print(p+q)
    <A:1|C:1|B:1> :: 2 /<A:2|C:1|B:1> :: 3 /<A:3|C:1|B:1> :: 4 /<A:1|C:2|B:1> :: 2 /<A:2|C:2|B:1> :: 3 /<A:3|C:2|B:1> :: 4 /<A:1|C:3|B:1> :: 2 /<A:2|C:3|B:1> :: 3 /<A:3|C:3|B:1> :: 4 /<A:1|C:4|B:1> :: 2 /<A:2|C:4|B:1> :: 3 /<A:3|C:4|B:1> :: 4 /<A:1|C:5|B:1> :: 2 /<A:2|C:5|B:1> :: 3 /<A:3|C:5|B:1> :: 4 /<A:1|C:1|B:2> :: 5 /<A:2|C:1|B:2> :: 6 /<A:3|C:1|B:2> :: 7 /<A:1|C:2|B:2> :: 5 /<A:2|C:2|B:2> :: 6 /<A:3|C:2|B:2> :: 7 /<A:1|C:3|B:2> :: 5 /<A:2|C:3|B:2> :: 6 /<A:3|C:3|B:2> :: 7 /<A:1|C:4|B:2> :: 5 /<A:2|C:4|B:2> :: 6 /<A:3|C:4|B:2> :: 7 /<A:1|C:5|B:2> :: 5 /<A:2|C:5|B:2> :: 6 /<A:3|C:5|B:2> :: 7
    >>> print((p*q).margSumOut(["B","C"])) # marginalize p*q over B and C(using sum)
    <A:1> :: 25 /<A:2> :: 35 /<A:3> :: 45


Instantiation
-------------

.. autoclass:: pyAgrum.Instantiation

:py:class:`pyAgrum.Potential`
---------

.. autoclass:: pyAgrum.:py:class:`pyAgrum.Potential`
		:exclude-members: populate
