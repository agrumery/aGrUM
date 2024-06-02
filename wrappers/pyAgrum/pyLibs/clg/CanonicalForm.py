# -*- coding: utf-8 -*-
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
This module implements the canonical form of a CLG density.
"""

import numpy as np
from numpy.linalg import inv
from numpy.linalg import det


class CanonicalForm:
  def __init__(self, scope=[], K=[], h=[], g=0):
    # Expecting a list of int corresponding to the id of the variables in the CLG.
    self._scope = scope
    self._size = len(self._scope)
    self._K = np.reshape(K, (len(K), len(K)))
    self._h = np.reshape(h, (len(h), 1))
    self._g = g

    # Canonical forms are sorted by increasing
    # variable index (in the CLG or with a user defined order).
    # This order gives the order of matrix and vector 
    # row/columns which is important for the different
    # operations on canonical forms.
    self._sort()

  def __str__(self):
    return "scope = {0}, K = {1}, h = {2}, g = {3}".format(self._scope,
                                                           self._K.tolist(),
                                                           self._h.tolist(),
                                                           self._g)

  def __repr__(self):
    return str(self)

  def __contains__(self, item):
    return item in self._scope

  def scope(self):
    """
    Return the scope of the canonical form.

    Returns
    ------
    List[int]
            The scope of the canonical form.
    """
    return self._scope

  @classmethod
  def fromCLG(cls, variable, parents, mu, sigma, B):
    """
    Factory to construct a canonical form from a conditional linear gaussian.

    Parameters
    ----------
    variable : int
      The variable to which the clg density is associated.
    parents : List[int]
      Parents of the variable in the graph.
    mu : float
      The intercept of the linear regression.
    sigma : float
      The variance of the clg density.
    B : List[float]
      The weights of the linear regression.

    Returns
    -------
    CanonicalForm
      The canonical form corresponding to the clg density.

    References
    ----------
    The formulas are given in "Propagation of Probabilities, Means, and
    Variances in Mixed Graphical Association Models, S. Lauritzen (1992)".
    """
    gamma = sigma ** 2  # The variance
    if len(parents) == 0:  # Unconditional case (no parents)
      scope = [variable]
      h = [mu / gamma]
      K = [[1 / gamma]]
    else:  # General case
      scope = [variable] + parents

      B = np.reshape(B, (len(B), 1))
      h = mu / gamma * np.insert(-B, 0, 1, axis=0)
      K = 1 / gamma * np.block([[1, -B.T], [-B, np.dot(B, B.T)]])

    g = - (mu ** 2 / gamma + np.log(2 * np.pi * gamma)) / 2

    cf = cls(scope, K, h, g)
    # Canonical form should always be sorted when created !
    cf._sort()

    return cf

  def toGaussian(self):
    """
    Gives the parameters of the gaussian associated to the canonical form.
    This operation is only well defined if K is invertible.

    Returns
    -------
    List[int]
      Paremeters of the Gaussian associated to the canonical form.
    """
    Sigma = inv(self._K)
    mu = np.dot(Sigma, self._h)
    # c = np.exp(self._g +
    # 0.5 * np.dot( np.dot(np.transpose(self._h), Sigma), self._h)
    # )
    return self._scope, mu, Sigma

  @staticmethod
  def _permute_matrix(matrix, permutation):
    """
    Permutes a matrix given a permutation.

    Parameters
    ----------
    matrix : np.array
      The matrix to permute.
    permutation : List[int]
      The permutation to use.

    Returns
    -------
    np.array
      The permuted matrix.
    """
    matrix[:, :] = matrix[permutation, :]  # Permutation of rows
    matrix[:, :] = matrix[:, permutation]  # Permutation of columns

    return matrix

  @staticmethod
  def _permute_vector(vector, permutation):
    """
    Permutes a vector given a permutation.

    Parameters
    ----------
    vector : np.array
      The vector to permute.
    permutation : List[int]
      The permutation to use.

    Returns
    -------
    np.array
      The permuted vector.
    """
    vector[:, :] = vector[permutation, :]

    return vector

  def _sort(self):
    """
    Sort the variable of the scope according to their indices in the CLG.    
    """
    sorted_scope = sorted(self._scope)
    if self._scope != sorted_scope:
      permutation = [self._scope.index(e) for e in sorted_scope]
      self._K = CanonicalForm._permute_matrix(self._K, permutation)
      self._h = CanonicalForm._permute_vector(self._h, permutation)
      self._scope = sorted_scope
    return

  def augment(self, variables):
    """
    Augment the scope of the canonical form with variables.

    Parameters
    ----------
    variables : List[int]
            The variables to add to the scope.

    Returns
    -------
    CanonicalForm
            The augmented canonical form.
    """
    new_scope = list(set(self._scope + variables))
    new_scope.sort()

    new_variables = list(set(new_scope) - set(self._scope))
    new_variables.sort()

    if len(self._scope) == 0:  # Case where the canonical form is empty
      augmented_K = np.zeros((len(new_variables), len(new_variables)))
      augmented_h = np.zeros((len(new_variables), 1))
    else:
      augmented_K = self._K
      augmented_h = self._h
      positions = [new_scope.index(v) for v in new_variables]
      # Position of first variable in new scope
      fv_new_position = new_scope.index(self._scope[0])
      for position in positions:
        # Avoid using insert when not necessary
        if position < fv_new_position:
          augmented_K = np.concatenate((np.zeros((len(augmented_K), 1)),
                                        augmented_K),
                                       axis=1)
          augmented_K = np.concatenate((np.zeros((1, len(augmented_K) + 1)),
                                        augmented_K),
                                       axis=0)
          augmented_h = np.concatenate(([[0]], augmented_h), axis=0)
        elif position < len(augmented_K):
          # Adding a new column of zeros at position in K
          augmented_K = np.insert(augmented_K,
                                  [position],
                                  np.zeros((len(augmented_K), 1)),
                                  axis=1)
          # Adding a new row of zeros at position in K
          augmented_K = np.insert(augmented_K,
                                  [position],
                                  np.zeros((1, len(augmented_K) + 1)),
                                  axis=0)
          # Adding a new zero at position in h
          augmented_h = np.insert(augmented_h, position, 0, axis=0)
        else:
          augmented_K = np.concatenate((augmented_K,
                                        np.zeros((len(augmented_K), 1))),
                                       axis=1)
          augmented_K = np.concatenate((augmented_K,
                                        np.zeros((1, len(augmented_K) + 1))),
                                       axis=0)
          augmented_h = np.concatenate((augmented_h, [[0]]), axis=0)

    return CanonicalForm(new_scope, augmented_K, augmented_h, self._g)

  def __mul__(self, other):
    """
    Multiplies two canonical forms.

    Parameters
    ----------
    other : CanonicalForm
      The canonical form to multiply with the current canonical form.

    Returns
    -------
    CanonicalForm
      The product.
    """
    scope = list(set(self._scope + other._scope))
    scope.sort()

    # Each canonical form is augmented with the scope of the other
    augmented_self = self.augment(other._scope)
    augmented_other = other.augment(self._scope)

    # Definition of the multiplication of two canonical forms
    K = augmented_self._K + augmented_other._K
    h = augmented_self._h + augmented_other._h
    g = augmented_self._g + augmented_other._g

    return CanonicalForm(scope, K, h, g)

  def __truediv__(self, other):
    """
    Divides two canonical forms.

    Parameters
    ----------
    other : CanonicalForm
      The canonical form to divide the current canonical form by.

    Returns
    -------
    CanonicalForm
      The ratio.
    """
    scope = list(set(self._scope + other._scope))
    scope.sort()

    # Each canonical form is augmented with the scope of the other
    augmented_self = self.augment(other._scope)
    augmented_other = other.augment(self._scope)

    K = augmented_self._K - augmented_other._K
    h = augmented_self._h - augmented_other._h
    g = augmented_self._g - augmented_other._g

    return CanonicalForm(scope, K, h, g)

  def __eq__(self, other):
    if isinstance(other, CanonicalForm):
      return (self._scope == other._scope and
              np.allclose(self._K, other._K, 1e-7) and
              np.allclose(self._h, other._h, 1e-7) and
              np.allclose(self._g, other._g, 1e-7))

    return False

  def marginalize(self, variables):
    """
    Marginalize the variables from the canonical form.
    
    Parameters
    ----------
    variables : List[int]
      The variables to marginalize.
    
    Returns
    -------
    CanonicalForm
      The marginalized canonical form.
    """
    if len(variables) == 0:
      return self

    if not all(variable in self._scope for variable in variables):
      raise ValueError("All the variables to marginalize are not in the "
                       "scope of the canonical form")

    new_scope = list(set(self._scope) - set(variables))
    variables.sort()
    new_scope.sort()

    ns_positions = [self._scope.index(v) for v in new_scope]
    v_positions = [self._scope.index(v) for v in variables]

    K_xx = self._K[np.ix_(ns_positions, ns_positions)]
    K_yy = self._K[np.ix_(v_positions, v_positions)]
    K_xy = self._K[np.ix_(ns_positions, v_positions)]
    K_yx = self._K[np.ix_(v_positions, ns_positions)]

    h_x = self._h[ns_positions]
    h_y = self._h[v_positions]

    K = K_xx - np.dot(np.dot(K_xy, inv(K_yy)), K_yx)
    h = h_x - np.dot(np.dot(K_xy, inv(K_yy)), h_y)
    g = self._g + 0.5 * (len(variables) * np.log(2 * np.pi) - np.log(det(K_yy)) + \
                         np.dot(np.dot(np.transpose(h_y), inv(K_yy)), h_y))

    return CanonicalForm(new_scope, K, h, g[0][0])

  def reduce(self, evidences):
    """
    Reduce the canonical form given a set of evidences.

    Parameters
    ----------
    evidences : Dict[int, float]
      The evidences.
    
    Returns
    -------
    CanonicalForm
      The reduced canonical form.
    """
    # Removing observed variables that are not in the scope of the CF
    evidences = {var: evidences[var] for var in self._scope if var in evidences}

    if len(evidences) == 0:
      return self

    variables = list(evidences.keys())
    values = list(evidences.values())

    # One liner to sort variables and values according to variables
    variables, values = map(list, zip(*sorted(zip(variables, values))))
    values = np.reshape(values, (len(values), 1))

    new_scope = list(set(self._scope) - set(variables))
    new_scope.sort()

    ns_positions = [self._scope.index(v) for v in new_scope]
    v_positions = [self._scope.index(v) for v in variables]

    K_xx = self._K[np.ix_(ns_positions, ns_positions)]
    K_yy = self._K[np.ix_(v_positions, v_positions)]
    K_xy = self._K[np.ix_(ns_positions, v_positions)]

    h_x = self._h[ns_positions]
    h_y = self._h[v_positions]

    h = h_x - np.dot(K_xy, values)

    g = self._g + np.dot(np.transpose(h_y), values) \
        - 0.5 * np.dot(np.dot(np.transpose(values), K_yy), values)

    return CanonicalForm(new_scope, K_xx, h, g[0][0])
