Learning a CLG
^^^^^^^^^^^^^^^

One of the main features of this library is the possibility to learn a CLG.

More precisely what can be learned is :
    - The dependency graph of a CLG
    - The parameters of a CLG: the mu and sigma of each variable, the coefficients of the arcs

**Learning the graph**

To learn the graph of a CLG (ie the dependence between variables) we use a modified PC algorithm based on the work
of Diego Colombo, Marloes H. Maathuis: Order-Independent Constraint-Based Causal Structure Learning(2014).
The independence test used is based on the work 
of Dario Simionato, Fabio Vandin: Bounding the Family-Wise Error Rate in Local Causal Discovery using Rademacher Averages(2022).

.. autofunction:: pyAgrum.clg.learning.supremum_deviation
 
.. autofunction:: pyAgrum.clg.learning.test_indep

.. autofunction:: pyAgrum.clg.learning.test_indep.RAveL_PC  

.. autofunction:: pyAgrum.clg.learning.test_indep.RAveL_MB

.. autofunction:: pyAgrum.clg.learning.test_indep.PC_algorithm



**Learning the parameters**

To learn the parameters of a CLG, we use linear regression.

.. autofunction:: pyAgrum.clg.learning.fitParameters
