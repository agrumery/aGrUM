Learning a CLG
^^^^^^^^^^^^^^

One of the main features of this library is the possibility to learn a CLG.

More precisely what can be learned is :
    - The dependency graph of a CLG
    - The parameters of a CLG: the mu and sigma of each variable, the coefficients of the arcs

**Learning the graph**

To learn the graph of a CLG (ie the dependence between variables) we use a modified PC algorithm based on the workof Diego Colombo, Marloes H. Maathuis: Order-Independent Constraint-Based Causal Structure Learning(2014).

The independence test used is based on the work of Dario Simionato, Fabio Vandin: Bounding the Family-Wise Error Rate in Local Causal Discovery using Rademacher Averages(2022).

.. autoclass:: pyagrum.clg.learning.CLGLearner
