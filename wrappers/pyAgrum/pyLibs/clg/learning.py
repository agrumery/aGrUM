# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
Using Rademacher Average to guarantee FWER (Family Wise Error Rate) in the independency test for Local Causal Discovery problem or for PC algorithm.
(see "Bounding the Family-Wise Error Rate in Local Causal Discover using Rademacher Averages", Dario Simionato, Fabio Vandin, 2022)
"""
import warnings

import pandas as pd
import numpy as np
import itertools
from sklearn.linear_model import LinearRegression
from typing import Dict, List, Set, Tuple, FrozenSet

from .constants import NodeId
from .CLG import CLG
from .GaussianVariable import GaussianVariable


class CLGLearner:
  """
  Using Rademacher Average to guarantee FWER(Family Wise Error Rate) in independency test.
  (see "Bounding the Family-Wise Error Rate in Local Causal Discover using Rademacher Averages", Dario Simionato, Fabio Vandin, 2022)
  """
  _model: CLG
  id2samples: Dict[NodeId, List]
  _df: pd.DataFrame
  sepset: Dict[Tuple[NodeId, NodeId], Set[NodeId]]
  _SD: float
  _V: Set[int]
  _N: int
  r_XYZ: Dict[Tuple[FrozenSet[NodeId], FrozenSet[NodeId]], List[float]]

  def __init__(self, filename: str, *, n_sample: int = 15, fwer_delta: float = 0.05):
    """
    Parameters
    ----------
    filename : str
      The path of the data file.
    n_sample : int
      amount of samples for Monte-Carlo Empirical Rademacher Average
    fwer_delta : float ∈ (0,1]
      Family-Wise Error Rate.
    """
    self._model = CLG()  # the CLG model
    self.id2samples = {}
    self.sepset = {}

    self._df = pd.read_csv(filename)
    # add all the variables to CLG model
    for name in self._df.columns:
      self._model.add(GaussianVariable(name, np.mean(self._df[name]), np.std(self._df[name])))

    # collect the samples to a dict whose keys are NodeID
    for node in self._model.nodes():
      self.id2samples[node] = self._df[self._model.name(node)].tolist()

    self._V = set(self._model.nodes())  # set of NodeId
    L = len(self._V)  # the degree of NodeId set
    self._N = L * (L - 1) * (2 ** (L - 3))  # the maximum number of hypotheses that coulbe be tested is N
    self.r_XYZ = {}

    # lazy computation of supremum deviation : if needed and _SD is None, then call suppremum_deviation
    self._n_sample = n_sample
    self._fwer_delta = fwer_delta
    self._SD = None

  def Pearson_coeff(self, X, Y, Z):
    """
    Estimate Pearson's linear correlation(using linear regression when Z is not empty).

    Parmeters
    ---------
    X : NodeId
      id of the first variable tested.
    Y : NodeId
      id of the second variable tested.
    Z : Set[NodeId]
      The conditioned variable's id set.
    """
    K = len(self.id2samples[X])  # number of samples

    r = np.zeros(K)
    if Z == set():  # if Z is an empty set
      # x and y are observations for X and Y
      x = self.id2samples[X]
      y = self.id2samples[Y]

    else:  # if Z isn't empty
      feature_name = [self._model.name(z) for z in Z]
      sample_Z = self._df[feature_name]
      # x represent the residuals of the linear regression of the observations of the variables in Z on the ones in X
      regressor_x = LinearRegression()
      sample_X = self._df[self._model.name(X)]
      regressor_x.fit(sample_Z, sample_X)
      x = sample_X - regressor_x.predict(sample_Z)
      # y represent the residuals of the linear regression of the observations of the variables in Z on the ones in Y
      regressor_y = LinearRegression()
      sample_Y = self._df[self._model.name(Y)]
      regressor_y.fit(sample_Z, sample_Y)
      y = sample_Y - regressor_y.predict(sample_Z)

    # all vectors have been centered around 0
    x -= np.mean(x)
    y -= np.mean(y)
    x /= np.std(x)
    y /= np.std(y)
    for i in range(K):
      r[i] = (x[i] * y[i] * K) / (K - 1)

    self.r_XYZ[tuple((frozenset({X, Y}), frozenset(Z)))] = r

  @staticmethod
  def generate_XYZ(l):
    """
    Find all the possible combinations of X, Y and Z.

    Returns
    -------
    List[Tuple[Set[NodeId], Set[NodeId]]]
      All the possible combinations of X, Y and Z.
    """
    s = set(l)
    # find all possible pairs(without considering the order), ie XY
    for X in s:
      for Y in s:
        if X < Y:
          for Z in CLGLearner.generate_subsets(s - {X, Y}):
            yield X, Y, Z

  def supremum_deviation(self, n_sample: int, fwer_delta: float):
    """
    Use n-MCERA to get supremum deviation.

    Parameters
    ----------
    n_sample : int
      The MC number n in n-MCERA.
    fwer_delta : float ∈ (0,1]
      Threshold.

    Returns
    -------
    SD : float
      The supremum deviation.
    """
    K = len(self.id2samples[0])  # number of samples

    # create sigma: the n_sample × K matrix of i.i.d. Rademacher random variables
    sigma = np.zeros((n_sample, K))
    for j in range(n_sample):
      for i in range(K):
        rademacher = np.random.randint(-1, 1)
        if rademacher == 0:
          rademacher = 1
        sigma[j][i] = rademacher

    # iteration over combinations of each pair of variables X,Y and set of conditioned variables Z
    # calcul r_XYZ for every element X,Y,Z
    for X, Y, Z in CLGLearner.generate_XYZ(self._model.nodes()):
      self.Pearson_coeff(X, Y, Z)

    # [a, b] is range of F
    b = np.mean(list(self.r_XYZ.values())[0])
    a = np.mean(list(self.r_XYZ.values())[0])
    for r in self.r_XYZ.values():
      if np.mean(r) >= b:
        b = np.mean(r)
      if np.mean(r) <= a:
        a = np.mean(r)

    # calcul n-MCERA: n-samples Monte-Carlo Empirical Rademacher Average
    R = 0
    for j in range(n_sample):
      sup = -1 * np.inf
      for r in self.r_XYZ.values():
        temp = 0
        for i in range(K):
          temp += sigma[j][i] * r[i]
        temp /= K
        if sup < temp:
          sup = temp
      R += sup
    R /= n_sample

    # calcul the final SD: supremum_deviation
    z = max(np.abs(a), np.abs(b))
    c = np.abs(b - a)

    temp = np.log(4 / (fwer_delta / self._N))
    R_hat = R + 2 * z * np.sqrt(temp / (2 * n_sample * K))
    SD = 2 * R_hat
    SD = SD + np.sqrt(c * (4 * K * R_hat + c * temp) * temp) / K
    SD = SD + c * temp / K
    SD = SD + c * np.sqrt(temp / (2 * K))

    self._n_sample = n_sample
    self._fwer_delta = fwer_delta
    self._SD = SD

    return SD

  def test_indep(self, X, Y, Z):
    """
    Perform a standard statistical test and use Bonferroni correction to correct for multiple hypothesis testing.

    Parameters
    ----------
    X : NodeId
      The id of the first variable tested.
    Y : NodeId
      The id of the second variable tested.
    Z : Set[NodeId]
      The conditioned variable's id set.

    Returns
    -------
    bool
      True if X and Y are indep given Z, False if not indep.
    """
    if self._SD is None:  # lazy supremum_deviation computation
      self.supremum_deviation(self._n_sample, self._fwer_delta)
    SD = self._SD
    r_XYZ = self.r_XYZ[tuple((frozenset({X, Y}), frozenset(Z)))].mean()

    if (r_XYZ - SD <= 0) and (r_XYZ + SD >= 0):
      return True  # X and Y are indep
    else:
      return False  # X and Y are dep

  @staticmethod
  def generate_subsets(S: Set[NodeId]):
    """
    Generator that iterates on all all the subsets of S (from the smallest to the biggest).

    Parameters
    ----------
    S : Set[NodeId]
      The set of variables.
    """
    l = list(S)
    for i in range(len(l) + 1):
      for z in itertools.combinations(l, i):
        yield set(z)

  def RAveL_PC(self, T):
    """
    Find the Parent-Children of variable T with FWER lower than Delta.

    Parameters
    ----------
    T : NodeId
      The id of the target variable T.

    Returns
    -------
    Set[NodeId]
      The Parent-Children of variable T with FWER lower than Delta.
    """
    PC = self._V - {T}

    for X in self._V - {T}:
      for Z in self.generate_subsets(self._V - {X, T}):
        if self.test_indep(T, X, Z):
          PC = PC - {X}
    return PC

  def RAveL_MB(self, T: NodeId) -> Set[NodeId]:
    """
    Find the Markov Boundary of variable T with FWER lower than Delta.

    Parameters
    ----------
    T : NodeId
      The id of the target variable T.

    Returns
    -------
    MB : Set[NodeId]
      The Markov Boundary of variable T with FWER lower than Delta.
    """
    # find PC(T)
    MB = self.RAveL_PC(T)

    # Add Spouse(T) to PC(T) in order to get MB(T)
    for X in list(MB):
      for Y in self.RAveL_PC(X):
        if (Y not in MB) and (Y != T):
          if not self.test_indep(T, Y, self._V - {Y, T}):
            MB.add(Y)

    return MB

  def Repeat_II(self, order, C, l, verbose=False):
    """
    This function is the second part of the Step1 of PC algorithm.

    Parameters
    ----------
    order : List[NodeId]
      The order of the variables.
    C : Dict[NodeId, Set[NodeId]]
      The temporary skeleton.
    l : int
      The size of the sepset
    verbose : bool
      Whether to print.

    Returns
    -------
    found_edge : bool
      True if a new edge is found, False if not.
    """
    found_edge = False
    V = list(self._V)  # set of NodeId
    # Select a (new) ordered pair of vertices (Xi, Xj) that are adjacent in C and satisfy |C[Xi]\{Xj}| ≥ l, using order(V)
    for i in range(len(V)):
      Xi = order[i]
      for j in range(len(V)):
        Xj = order[j]
        if i == j:  # if i == j, skip
          continue
        if Xi not in C[Xj] or Xj not in C[Xi]:  # if Xi and Xj are not adjacent in C
          continue
        if len(C[Xi] - {Xj}) < l:  # if |C[Xi]\{Xj}| < l
          continue

        # III: Repeat
        # Choose a (new) set S ⊆ C[Xi]\{Xj} with |S| = l
        for S in itertools.combinations(C[Xi] - {Xj}, l):
          # Check if Xi and Xj are conditionally independent given S
          if self.test_indep(Xi, Xj, set(S)):
            # Delete edge Xi − Xj from C
            if verbose: warnings.warn("{0} and {1} are conditionally independent given {2}".format(Xi, Xj, S))
            C[Xi].remove(Xj)
            C[Xj].remove(Xi)
            # Let sepset(Xi,Xj) = sepset(Xj,Xi) = S
            self.sepset[(Xi, Xj)] = set(S)
            self.sepset[(Xj, Xi)] = set(S)
            # III: Until Xi and Xj are no longer adjacent in C or all S ⊆ C[Xi]\{Xj} with |S| = l have been considered
            found_edge = True
            return found_edge

    return found_edge

  def Adjacency_search(self, order, verbose=False):
    """
    This function is the first step of PC-algo: Adjacency Search.
    Apply indep_test() to the first step of PC-algo for Adjacency Search.

    Parameters
    ----------
    order : List[NodeId]
      A particular order of the Nodes.
    verbose : bool
      Whether to print the process of Adjacency Search.

    Returns
    -------
    C : Dict[NodeId, Set[NodeId]]
      The temporary skeleton.
    sepset : Dict[Tuple[NodeId, NodeId], Set[NodeId]]
      Sepset(which will be used in Step2&3 of PC-Algo).
    """

    def all_satisfied(V, order, C, l):
      """
      Check if all pairs of adjacent vertices (Xi,Xj) in C satisfy that the size of the neighbours of Xi (except Xj) is less or equal than l.

      Parameters
      ----------
      V : List[NodeId]
        The list of NodeId.
      order : List[NodeId]
        A particular order of the Nodes.
      C : Dict[NodeId, Set[NodeId]]
        The temporary skeleton.
      l : int
        The size of the sepset.

      Returns
      -------
      all_satisfied : bool
        Whether all pairs of adjacent vertices (Xi,Xj) in C satisfy the constraint.
      """
      all_satisfied = True
      for i in range(len(V)):
        Xi = order[i]
        for j in range(len(V)):
          if i == j:  # if i == j, skip
            continue
          Xj = order[j]
          if Xi in C[Xj] and Xj in C[Xi]:  # if Xi and Xj are adjacent in C
            if len(C[Xi] - {Xj}) > l:
              all_satisfied = False
              return all_satisfied
      return all_satisfied

    # Form the complete undirected graph C on the vertex set V
    V = list(self._V)  # set of NodeId
    C = {v: set() for v in V}  # C is shown by a Adjacency List
    for i in range(len(V) - 1):
      for j in range(i + 1, len(V)):
        C[V[i]].add(V[j])
        C[V[j]].add(V[i])

    l = -1
    # I: Repeat
    while True:
      l += 1

      # II: Repeat
      while True:
        found_edge = self.Repeat_II(order, C, l, verbose)
        # II: Until all ordered pairs of adjacent vertices (Xi,Xj) in C with C[Xi]\{Xj}| ≥ l have been considered
        if not found_edge:
          break

      # I: Until all pairs of adjacent vertices (Xi,Xj) in C satisfy |C[Xi]\{Xj}| ≤ l
      if all_satisfied(V, order, C, l):
        return C, self.sepset

  def three_rules(self, C, verbose=False):
    """
    This function is the third step of PC-algo.
    Orient as many of the remaining undirected edges as possible by repeatedly application of the three rules.

    Parameters
    ----------
    C : Dict[NodeId, Set[NodeId]]
      The temporary skeleton.
    verbose : bool
      Whether to print the process of this function.

    Returns
    -------
    C : Dict[NodeId, Set[NodeId]]
      The final skeleton (of Step3).
    """
    while True:
      new_oriented = False

      # Rule 1: Orient Xj − Xk into Xj -> Xk whenever there is a directed edge Xi -> Xj such that Xi and Xk are not adjacent
      # (otherwise a new v-structure would be created)
      for Xj in self._V:
        for Xk in C[Xj]:
          if Xj in C[Xk]:  # Xj - Xk
            for Xi in self._V - {Xk, Xj}:
              if (Xj in C[Xi] and Xi not in C[Xj]) and (
                 Xi not in C[Xk] and Xk not in C[Xi]):  # Xi -> Xj - Xk such that Xi and Xk are not adjacent
                # Orient Xj -> Xk
                if verbose: warnings.warn('Rule 1 applied:{0}->{1}'.format(Xj, Xk))
                C[Xk].remove(Xj)
                new_oriented = True
                break
          # Check other Xk
          if new_oriented:
            break

      # Rule 2: Orient Xi − Xj into Xi -> Xj whenever there is a chain Xi -> Xk -> Xj
      # (otherwise a directed cycle is created)
      for Xi in self._V:
        for Xj in C[Xi]:
          if Xi in C[Xj]:  # Xi - Xj
            for Xk in C[Xi] - {Xj}:
              if (Xi not in C[Xk]) and (Xj in C[Xk] and Xk not in C[Xj]):  # Xi -> Xk -> Xj
                # Orient Xi -> Xj
                if verbose: warnings.warn('Rule 2 applied:{0}->{1}'.format(Xi, Xj))
                C[Xj].remove(Xi)
                new_oriented = True
                break
          # Check other Xj
          if new_oriented:
            break

      # Rule 3: Orient Xi − Xj into Xi → Xj whenever there are two chains Xi − Xk → Xj and Xi − Xl → Xj such that Xk and Xl are not adjacent
      # (otherwise a new v-structure or a directed cycle is created)
      for Xi in self._V:
        for Xj in C[Xi]:
          if Xi in C[Xj]:  # Xi - Xj
            for Xk, Xl in itertools.combinations(C[Xi] - {Xj}, 2):  # Xk and Xl are not adjacent
              if Xi in C[Xk] and Xi in C[Xl]:  # Xi - Xk and Xi - Xl
                if (Xj in C[Xk] and Xk not in C[Xj]) and (Xj in C[Xl] and Xl not in C[Xj]):  # Xk -> Xj and Xl -> Xj
                  # Orient Xi -> Xj
                  if verbose: warnings.warn('Rule 3 applied:{0}->{1}'.format(Xi, Xj))
                  C[Xj].remove(Xi)
                  new_oriented = True
                  break
          # Check other Xj
          if new_oriented:
            break

      # Stop if no more edges can be oriented
      if not new_oriented:
        break

    return C

  def Step4(self, C, verbose=False):
    """
    This function is the fourth step of PC-algo.
    Orient the remaining undirected edge by comparing variances of two nodes.

    Parameters
    ----------
    C : Dict[NodeId, Set[NodeId]]
      The temporary skeleton.
    verbose : bool
      Whether to print the process of Step4.

    Returns
    -------
    C : Dict[NodeId, Set[NodeId]]
      The final skeleton (of Step4).
    new_oriented : bool
      Whether there is a new edge oriented in the fourth step.
    """
    new_oriented = False
    # Rule 0: Orient Xi − Xj into Xi -> Xj whenever Var(Xi) <= Var(Xj) (but only once)
    for Xi in self._V:
      for Xj in C[Xi]:
        if Xi in C[Xj]:  # Xi - Xj
          if np.std(self.id2samples[Xi]) <= np.std(self.id2samples[Xj]):
            # Orient Xi -> Xj
            if verbose: warnings.warn('Rule 0 applied:{0}->{1}'.format(Xi, Xj))
            C[Xj].remove(Xi)
            new_oriented = True
            # We only apply Rule 0 once
            return C, new_oriented

    return C, new_oriented

  def PC_algorithm(self, order, verbose=False):
    """
    This function is an advanced version of PC-algo.
    We use Indep_test_Rademacher() to replace indep_test() in PC-algo.
    And we orient the undirected edges in the skeleton C by comparing the variances of the two nodes.

    Parameters
    ----------
    order : List[NodeId]
      A particular order of the Nodes.
    verbose : bool
      Whether to print the process of the PC algorithm.

    Returns
    -------
    C : Dict[NodeId, Set[NodeId]]
      A directed graph DAG representing the causal structure.
    """
    # Step 1: Apply Adjacency_search() to obtain a skeleton C and a set of sepsets
    if verbose: warnings.warn("Step 1: Apply Adjacency_search() to obtain a skeleton C and a set of sepsets")
    C, sepset = self.Adjacency_search(order, verbose)

    # Step 2: Find the v-structures
    if verbose: warnings.warn("Step 2: Find the v-structures")
    for Xk in self._V:
      for Xi, Xj in itertools.combinations(C[Xk], 2):
        if (Xi, Xj) in sepset and Xk not in sepset[(Xi, Xj)]:
          if Xi not in C[Xj] and Xj not in C[Xi]:
            # Orient Xi -> Xk <- Xj
            if (Xi in C[Xk] and Xk in C[Xi]) and (Xj in C[Xk] and Xk in C[Xj]):
              if verbose: warnings.warn('V-structure found:{0}->{1}<-{2}'.format(Xi, Xk, Xj))
              C[Xk].remove(Xi)
              C[Xk].remove(Xj)
            elif (Xi in C[Xk] and Xk in C[Xi]) and (Xj not in C[Xk] and Xk in C[Xj]):
              if verbose: warnings.warn('V-structure found:{0}->{1}<-{2}'.format(Xi, Xk, Xj))
              C[Xk].remove(Xi)
            elif (Xi not in C[Xk] and Xk in C[Xi]) and (Xj in C[Xk] and Xk in C[Xj]):
              if verbose: warnings.warn('V-structure found:{0}->{1}<-{2}'.format(Xi, Xk, Xj))
              C[Xk].remove(Xj)

    # Repeat the following steps until no more edges can be oriented by Step 4
    while True:
      # Step 3: Orient as many of the remaining undirected edges as possible by repeatedly application of the following three rules
      if verbose: warnings.warn(
        "Step 3: Orient as many of the remaining undirected edges as possible by repeatedly application of the three rules")
      C = self.three_rules(C, verbose)

      # Step 4: Orient the remaining undirected edge by comparing variances of two nodes
      if verbose: warnings.warn("Step 4: Orient one remaining undirected edge by comparing variances of the two nodes")
      C, new_oriented = self.Step4(C, verbose)

      # Stop if no more edges can be oriented by Step 4
      if not new_oriented:
        break

    # Return the final DAG skeleton
    return C

  def estimate_parameters(self, C):
    """
    This function is used to estimate the parameters of the CLG model.

    Parameters
    ----------
    C : Dict[NodeId, Set[NodeId]]
      A directed graph DAG representing the causal structure.

    Returns
    -------
    id2mu : Dict[NodeId, float]
      The estimated mean of each node.
    id2sigma : Dict[NodeId, float]
      The estimated variance of each node.
    arc2coef : Dict[Tuple[NodeId, NodeId], float]
      The estimated coefficients of each arc.
    """
    # Initialization
    parents = {}
    to_estimate = []
    id2mu = {}
    id2sigma = {}
    arc2coef = {}

    # Find the parents of each node
    for Xi in self._V:
      parents[Xi] = set()
      for Xj in self._V - {Xi}:
        if Xi in C[Xj]:
          parents[Xi].add(Xj)

    # Find the root of the DAG
    for Xi in self._V:
      if len(parents[Xi]) == 0:
        to_estimate.append(Xi)

    # Loop the DAG from the root to the leaves
    while len(to_estimate) > 0:
      # Pop the first node in to_estimate as Xi
      Xi = to_estimate.pop(0)

      # Add sons of Xi to to_estimate
      for Xj in C[Xi]:
        to_estimate.append(Xj)

      # If Xi has no parent
      if len(parents[Xi]) == 0:
        # Estimate the mean of Xi
        id2mu[Xi] = np.mean(self.id2samples[Xi])
        # Estimate the variance of Xi
        id2sigma[Xi] = np.std(self.id2samples[Xi])

      # If Xi has parents
      if len(parents[Xi]) != 0:
        # Create the feature matrix X
        X = np.zeros((len(self.id2samples[Xi]), len(parents[Xi])))
        for i in range(len(self.id2samples[Xi])):
          for j, Xj in enumerate(parents[Xi]):
            X[i][j] = self.id2samples[Xj][i]

        # Create the target vector y
        y = np.array(self.id2samples[Xi])

        # Estimate the coefficients of the arcs from Xi's parents to Xi
        linear = LinearRegression()
        linear.fit(X, y)
        for j, Xj in enumerate(parents[Xi]):
          arc2coef[(Xj, Xi)] = linear.coef_[j]

        # Estimate the mean of Xi
        id2mu[Xi] = linear.intercept_

        # Estimate the variance of Xi
        id2sigma[Xi] = np.std(y - linear.predict(X))

    return id2mu, id2sigma, arc2coef

  def learnCLG(self):
    """
    First use PC algorithm to learn the skeleton of the CLG model.
    Then estimate the parameters of the CLG model.
    Finally create a CLG model and return it.

    Returns
    -------
    learned_clg : CLG
      The learned CLG model.
    """
    learned_clg = CLG()

    # Use PC algorithm to learn the structure of the CLG model
    C = self.PC_algorithm(order=self._model.nodes(), verbose=False)

    # Estimate the parameters of the CLG model
    id2mu, id2sigma, arc2coef = self.estimate_parameters(C)

    # Add the nodes to the CLG model
    for node in self._model.nodes():
      learned_clg.add(GaussianVariable(name=self._model.variable(node).name(), mu=float(f"{id2mu[node]:.3f}"),
                                       sigma=float(f"{id2sigma[node]:.3f}")))
    # Add the arcs to the CLG model
    for arc in arc2coef.keys():
      learned_clg.addArc(arc[0], arc[1], float(f"{arc2coef[arc]:.2f}"))

    return learned_clg

  def fitParameters(self, clg):
    """
    In this function, we fit the parameters of the CLG model.

    Parameters
    ----------
    clg : CLG
      The CLG model to be changed its parameters.
    """
    # Get the DAG of the CLG model
    C = clg.dag2dict()

    # Estimate the parameters of the CLG model
    id2mu, id2sigma, arc2coef = self.estimate_parameters(C)

    # Change the parameters of the CLG model
    for node in clg.nodes():
      clg.setMu(node, float(f"{id2mu[node]:.3f}"))
      clg.setSigma(node, float(f"{id2sigma[node]:.3f}"))

    for arc in arc2coef.keys():
      clg.setCoef(arc[0], arc[1], float(f"{arc2coef[arc]:.2f}"))
