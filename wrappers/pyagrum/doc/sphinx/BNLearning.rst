Learning
========
pyAgrum provides a complete framework for learning Bayesian networks from data. It includes various algorithms for structure learning, parameter learning, and model evaluation. The library supports both score-based and constraint-based approaches, allowing users to choose the method that best fits their needs.

pyAgrum brings together all Bayesian network learning processes in a single, easy-to-use class: :class:`pyagrum.BNLearner`. This class provides direct access to complete learning algorithms and their parameters (such as priors, scores, constraints, etc.), and also offers low-level functions that facilitate the development of new learning algorithms (for example, computing chi² or conditional likelihood on the dataset).

BNLearner allows to choose :

- the structure learning algorithm (MIIC, Greedy Hill Climbing, K2, etc.),
- the parameter learning method (including EM),
- the scoring function (BDeu, AIC, etc.) for score-based algorithms,
- the prior (smoothing, Dirichlet, etc.),
- the constraints (e.g., forbidding certain arcs, specifying a partial order among variables, etc.),
- the correction method (NML, etc.) for the MIIC algorithm,
- and many low-level functions, such as computing the chi², G² score, or the conditional likelihood on the dataset.

:class:`pyagrum.BNLearner` is able to learn a Bayesian network from a database (a pandas.DataFrame) or from a csv file.

Class BNLearner
---------------

.. list-table:: Methods for performing learning:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `fitParameters <#pyagrum.BNLearner.fitParameters>`_
      - `latentVariables <#pyagrum.BNLearner.latentVariables>`_
      - `learnBN <#pyagrum.BNLearner.learnBN>`_
    * - `learnDAG <#pyagrum.BNLearner.learnDAG>`_
      - `learnParameters <#pyagrum.BNLearner.learnParameters>`_
      - `learnPDAG <#pyagrum.BNLearner.learnPDAG>`_


.. list-table:: Structure learning algorithms:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `isConstraintBased <#pyagrum.BNLearner.isConstraintBased>`_
      - `isScoreBased <#pyagrum.BNLearner.isScoreBased>`_
      - `useGreedyHillClimbing <#pyagrum.BNLearner.useGreedyHillClimbing>`_
    * - `useK2 <#pyagrum.BNLearner.useK2>`_
      - `useLocalSearchWithTabuList <#pyagrum.BNLearner.useLocalSearchWithTabuList>`_
      - `useMIIC <#pyagrum.BNLearner.useMIIC>`_


.. list-table:: Managing structure learning constraints:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `addForbiddenArc <#pyagrum.BNLearner.addForbiddenArc>`_
      - `addMandatoryArc <#pyagrum.BNLearner.addMandatoryArc>`_
      - `addNoChildrenNode <#pyagrum.BNLearner.addNoChildrenNode>`_
    * - `addNoParentNode <#pyagrum.BNLearner.addNoParentNode>`_
      - `addPossibleEdge <#pyagrum.BNLearner.addPossibleEdge>`_
      - `eraseForbiddenArc <#pyagrum.BNLearner.eraseForbiddenArc>`_
    * - `eraseMandatoryArc <#pyagrum.BNLearner.eraseMandatoryArc>`_
      - `eraseNoChildrenNode <#pyagrum.BNLearner.eraseNoChildrenNode>`_
      - `eraseNoParentNode <#pyagrum.BNLearner.eraseNoParentNode>`_
    * - `erasePossibleEdge <#pyagrum.BNLearner.erasePossibleEdge>`_
      - `setForbiddenArcs <#pyagrum.BNLearner.setForbiddenArcs>`_
      - `setMandatoryArcs <#pyagrum.BNLearner.setMandatoryArcs>`_
    * - `setMaxIndegree <#pyagrum.BNLearner.setMaxIndegree>`_
      - `setSliceOrder <#pyagrum.BNLearner.setSliceOrder>`_
      -


.. list-table:: Scores and priors (for structure learning):
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `useBDeuPrior <#pyagrum.BNLearner.useBDeuPrior>`_
      - `useDirichletPrior <#pyagrum.BNLearner.useDirichletPrior>`_
      - `useSmoothingPrior <#pyagrum.BNLearner.useSmoothingPrior>`_
    * - `useScoreAIC <#pyagrum.BNLearner.useScoreAIC>`_
      - `useScoreBD <#pyagrum.BNLearner.useScoreBD>`_
      - `useScoreBDeu <#pyagrum.BNLearner.useScoreBDeu>`_
    * - `useScoreBIC <#pyagrum.BNLearner.useScoreBIC>`_
      - `useScoreK2 <#pyagrum.BNLearner.useScoreK2>`_
      - `useScoreLog2Likelihood <#pyagrum.BNLearner.useScoreLog2Likelihood>`_
    * - `useMDLCorrection <#pyagrum.BNLearner.useMDLCorrection>`_
      - `useNMLCorrection <#pyagrum.BNLearner.useNMLCorrection>`_
      - `useNoCorrection <#pyagrum.BNLearner.useNoCorrection>`_


.. list-table:: EM parameter learning:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `disableEMEpsilon <#pyagrum.BNLearner.disableEMEpsilon>`_
      - `disableEMMaxIter <#pyagrum.BNLearner.disableEMMaxIter>`_
      - `disableEMMaxTime <#pyagrum.BNLearner.disableEMMaxTime>`_
    * - `disableEMMinEpsilonRate <#pyagrum.BNLearner.disableEMMinEpsilonRate>`_
      - `EMEpsilon <#pyagrum.BNLearner.EMEpsilon>`_
      - `EMHistory <#pyagrum.BNLearner.EMHistory>`_
    * - `EMMaxIter <#pyagrum.BNLearner.EMMaxIter>`_
      - `EMMaxTime <#pyagrum.BNLearner.EMMaxTime>`_
      - `EMMinEpsilonRate <#pyagrum.BNLearner.EMMinEpsilonRate>`_
    * - `EMState <#pyagrum.BNLearner.EMState>`_
      - `EMStateApproximationScheme <#pyagrum.BNLearner.EMStateApproximationScheme>`_
      - `EMVerbosity <#pyagrum.BNLearner.EMVerbosity>`_
    * - `enableEMEpsilon <#pyagrum.BNLearner.enableEMEpsilon>`_
      - `enableEMMaxIter <#pyagrum.BNLearner.enableEMMaxIter>`_
      - `enableEMMaxTime <#pyagrum.BNLearner.enableEMMaxTime>`_
    * - `enableEMMinEpsilonRate <#pyagrum.BNLearner.enableEMMinEpsilonRate>`_
      - `forbidEM <#pyagrum.BNLearner.forbidEM>`_
      - `isEnabledEMEpsilon <#pyagrum.BNLearner.isEnabledEMEpsilon>`_
    * - `isEnabledEMMaxIter <#pyagrum.BNLearner.isEnabledEMMaxIter>`_
      - `isEnabledEMMaxTime <#pyagrum.BNLearner.isEnabledEMMaxTime>`_
      - `isEnabledEMMinEpsilonRate <#pyagrum.BNLearner.isEnabledEMMinEpsilonRate>`_
    * - `isUsingEM <#pyagrum.BNLearner.isUsingEM>`_
      - `nbrEMIterations <#pyagrum.BNLearner.nbrEMIterations>`_
      - `setEMEpsilon <#pyagrum.BNLearner.setEMEpsilon>`_
    * - `setEMMaxIter <#pyagrum.BNLearner.setEMMaxIter>`_
      - `setEMMaxTime <#pyagrum.BNLearner.setEMMaxTime>`_
      - `setEMMinEpsilonRate <#pyagrum.BNLearner.setEMMinEpsilonRate>`_
    * - `setEMVerbosity <#pyagrum.BNLearner.setEMVerbosity>`_
      - `useEM <#pyagrum.BNLearner.useEM>`_
      - `useEMWithDiffCriterion <#pyagrum.BNLearner.useEMWithDiffCriterion>`_
    * - `useEMWithRateCriterion <#pyagrum.BNLearner.useEMWithRateCriterion>`_
      -
      -


.. list-table:: Database inspection / direct requesting:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `chi2 <#pyagrum.BNLearner.chi2>`_
      - `correctedMutualInformation <#pyagrum.BNLearner.correctedMutualInformation>`_
      - `databaseWeight <#pyagrum.BNLearner.databaseWeight>`_
    * - `domainSize <#pyagrum.BNLearner.domainSize>`_
      - `G2 <#pyagrum.BNLearner.G2>`_
      - `idFromName <#pyagrum.BNLearner.idFromName>`_
    * - `nameFromId <#pyagrum.BNLearner.nameFromId>`_
      - `names <#pyagrum.BNLearner.names>`_
      - `nbCols <#pyagrum.BNLearner.nbCols>`_
    * - `nbRows <#pyagrum.BNLearner.nbRows>`_
      - `hasMissingValues <#pyagrum.BNLearner.hasMissingValues>`_
      - `logLikelihood <#pyagrum.BNLearner.logLikelihood>`_
    * - `mutualInformation <#pyagrum.BNLearner.mutualInformation>`_
      - `pseudoCount <#pyagrum.BNLearner.pseudoCount>`_
      - `rawPseudoCount <#pyagrum.BNLearner.rawPseudoCount>`_
    * - `recordWeight <#pyagrum.BNLearner.recordWeight>`_
      - `score <#pyagrum.BNLearner.score>`_
      - `setDatabaseWeight <#pyagrum.BNLearner.setDatabaseWeight>`_
    * - `setRecordWeight <#pyagrum.BNLearner.setRecordWeight>`_
      -
      -

.. list-table:: Fine-tuning the behavior of the BNLearner:
    :class: method-summary
    :align: left
    :widths: 10 10 10
    :header-rows: 0

    * - `copyState <#pyagrum.BNLearner.copyState>`_
      - `getNumberOfThreads <#pyagrum.BNLearner.getNumberOfThreads>`_
      - `isGumNumberOfThreadsOverriden <#pyagrum.BNLearner.isGumNumberOfThreadsOverriden>`_
    * - `setNumberOfThreads <#pyagrum.BNLearner.setNumberOfThreads>`_
      -
      -

.. autoclass:: pyagrum.BNLearner
  :exclude-members: asIApproximationSchemeConfiguration, thisown
