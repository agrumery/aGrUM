Continuous Linear Gaussian Bayesian Networks
============================================

.. figure:: _static/clg.jpg
    :align: center
    :alt: A CLG model

A Continuous Linear Gaussian (CLG) model is a graphical probabilistic model, often employed for modeling continuous-valued variables in a system. The CLG model assumes that the relationships between variables can be represented as linear equations, and the variables are subject to Gaussian noise.

In a CLG model, each variable is represented as a linear function of its parent variables, with an additional Gaussian noise term. This linear relationship is expressed using regression coefficients, which determine the strength and direction of the influence of the parent variables on the target variable. The Gaussian noise term captures the inherent uncertainty or variability in the system.

The CLG model assumes that the noise terms associated with different variables are independent and normally distributed. This assumption allows for tractable inference and learning procedures. Given observed data, the model parameters, such as regression coefficients and noise variances, can be estimated using methods like maximum likelihood estimation or Bayesian inference.

Structural Equation Modeling (SEM) is a statistical technique used to analyze the relationships among variables. In SEM, the relationships between variables are represented using structural equations, which specify how the variables interact with each other. These equations can be derived from theoretical considerations or based on empirical evidence.

SEM combines elements of factor analysis and path analysis, enabling researchers to simultaneously examine the measurement properties of variables (measurement model) and the causal relationships between variables (structural model). It allows for testing of hypotheses, model comparison, and evaluation of the overall model fit.

SEM provides estimates of the strength and significance of the relationships between variables, as well as the ability to assess model fit, investigate mediation and moderation effects, and conduct various types of model modifications. It is commonly used in social sciences, psychology, economics, and other fields to explore complex relationships and test theoretical models.

**Tutorials**

* `Notebooks on CLGs in pyAgrum <notebooks/27_Models_clg.ipynb>`_.

**Reference**

.. toctree::
   :maxdepth: 3

   clgGaussianVariables
   clgModel
   clgInference
   clgGraphical
   clgLearner
