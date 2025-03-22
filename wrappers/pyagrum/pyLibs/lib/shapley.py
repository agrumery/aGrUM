############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
tools for BN qualitative analysis and explainability
"""

import math
from typing import Dict
import itertools

import numpy as np
import pandas as pd

import matplotlib as mpl
import matplotlib.pyplot as plt

from copy import deepcopy
from mpl_toolkits.axes_grid1 import make_axes_locatable
import matplotlib.colors as mcolors

import pyagrum as gum

_cdict = {
  'red': ((0.0, 0.1, 0.3),
          (1.0, 0.6, 1.0)),
  'green': ((0.0, 0.0, 0.0),
            (1.0, 0.6, 0.8)),
  'blue': ((0.0, 0.0, 0.0),
           (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)


class ShapValues:
  """
  The ShapValue class implements the calculation of Shap values in Bayesian networks.

  The main implementation is based on Conditional Shap values [3]_, but the Interventional calculation method proposed in [2]_ is also present. In addition, a new causal method, based on [1]_, is implemented which is well suited for Bayesian networks.

.. [1] Heskes, T., Sijben, E., Bucur, I., & Claassen, T. (2020). Causal Shapley Values: Exploiting Causal Knowledge. 34th Conference on Neural Information Processing Systems. Vancouver, Canada.

.. [2] Janzing, D., Minorics, L., & Blöbaum, P. (2019). Feature relevance quantification in explainable AI: A causality problem. arXiv: Machine Learning. Retrieved 6 24, 2021, from https://arxiv.org/abs/1910.13413

.. [3] Lundberg, S. M., & Su-In, L. (2017). A Unified Approach to Interpreting Model. 31st Conference on Neural Information Processing Systems. Long Beach, CA, USA.
  """

  @staticmethod
  def _logit(p):
    return np.log(p / (1 - p))

  @staticmethod
  def _comb(n, k):
    return math.comb(n, k)

  @staticmethod
  def _fact(n):
    return math.factorial(n)

  def __init__(self, bn, target):
    self.bn = bn
    self.target = target
    self.feats_names = self._get_feats_name(bn, target)
    self.results = None

  ################################## VARIABLES ##################################

  @staticmethod
  def _get_feats_name(bn, target):
    list_feats_name = list(bn.names())
    list_feats_name.remove(target)
    return list_feats_name

  def _get_list_names_order(self):
    ### Return a list of BN's nodes names
    list_node_names = [None] * len(self.bn.names())
    for name in self.bn.names():
      list_node_names[self.bn.idFromName(name)] = name
    return list_node_names

  @staticmethod
  def _coal_encoding(convert, coal):
    ### Convert a list of nodes : ['X', 'Z'] to an array of 0 and 1.
    ### ['X', 'Z'] -> [1,0,1,0]
    temp = np.zeros(len(convert), dtype=int)
    for var in coal:
      i = convert.index(var)
      temp[i] = 1
    return temp

  def _get_markov_blanket(self):
    feats_markov_blanket = []
    for i in gum.MarkovBlanket(self.bn, self.target).nodes():
      convert = self._get_list_names_order()
      feats_markov_blanket.append(convert[i])
    feats_markov_blanket.remove(self.target)
    return feats_markov_blanket

  ################################## Get All Combinations ##################################

  def _get_all_coal_compress(self):
    ### Return : all coalitions with the feature
    return (list(itertools.compress(self.feats_names, mask)) for mask in
            itertools.product(*[[0, 1]] * (len(self.feats_names))))

  ################################## PREDICTION ##################################
  def _filtrage(self, df, conditions):
    ### Return : a selected part of DataFrame based on conditions, conditions must be in a dict :
    ### The key is the name of the features and the value is the value that the features should take
    ### Example : {'X1':0, 'X2':1}
    if conditions == {}:
      return df

    first = next(iter(conditions))
    new_df = df[df[first] == conditions[first]]
    conditions.pop(first)
    return self._filtrage(new_df, conditions)

  def _init_Inference(self):
    ie = gum.LazyPropagation(self.bn)
    ie.addTarget(self.target)
    for name in self.feats_names:
      ie.addEvidence(name, 0)
    return ie

  ##### Prediction fonctions ####

  def _pred_markov_blanket(self, df, ie, markov_blanket):
    unique = df.groupby(markov_blanket).agg(freq=(self.target, 'count')).reset_index()
    result = 0
    for i in range(len(unique)):
      for name in markov_blanket:
        ie.chgEvidence(name, str(unique[name].iloc[i]))
      ie.makeInference()
      predicted = ie.posterior(self.target).toarray()[1]
      result = result + self._logit(predicted) * unique['freq'].iloc[i] / len(df)
    return result

  def _pred_markov_blanket_logit(self, df, ie, markov_blanket):
    unique = df.groupby(markov_blanket).agg(freq=('Y', 'count')).reset_index()
    result = 0
    for i in range(len(unique)):
      for name in markov_blanket:
        ie.chgEvidence(name, str(unique[name].iloc[i]))
      ie.makeInference()
      predicted = ie.posterior(self.target).toarray()[1]
      result = result + predicted * unique['freq'].iloc[i] / len(df)
    return self._logit(result)

  def _evidenceImpact(self, condi, ie):
    ie.eraseAllEvidence()
    for key in condi.keys():
      ie.addEvidence(key, str(condi[key]))
    ie.makeInference()
    return self._logit(ie.posterior(self.target).toarray()[1])

  ############################## MARGINAL#########################################

  def _predict_marginal(self, df, ie):
    result = []
    for i in range(len(df)):
      for name in self.feats_names:
        ie.chgEvidence(name, str(df[name].iloc[i]))
      ie.makeInference()
      result.append(ie.posterior(self.target).toarray()[1])
    return np.array(result)

  ################################## COMPUTE SHAP ##################################

  def _compute_SHAP_i(self, S_U_i, S, v, size_S):
    size_all_features = len(self.bn.nodes()) - 1
    diff = v[f'{S_U_i}'] - v[f'{S}']
    return diff / self._invcoeff_shap(size_S, size_all_features)

  def _invcoeff_shap(self, S_size, len_features):
    return (len_features - S_size) * self._comb(len_features, S_size)

  ################################## Get the two coalitions to substract S_U_i and S #################################

  @staticmethod
  def _gen_coalitions2(list_feats):
    ### !!!! THE FEATURE i HAVE TO BE THE LAST IN THE LIST OF FEATURES !!!
    ### Return : all coalitions with the feature
    for mask in itertools.product(*[[0, 1]] * (len(list_feats) - 1)):
      S_U_i = itertools.compress(list_feats, mask + (1,))
      S = itertools.compress(list_feats, mask + (0,))
      yield S_U_i, S

  ################################## Function to Compute CONDITIONNAL SHAP Value ##################################
  def _conditional(self, train: pd.DataFrame) -> Dict[str, float]:
    """
    Compute the conditional Shap Values for each variables.

    Parameters
    ----------
    train :pandas.DataFrame
      the database

    Returns
    -------
      a dictionary Dict[str,float]
    """

    ie = self._init_Inference()

    v = train.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()

    convert = self._get_list_names_order()

    for i in range(len(v)):
      v['Baseline'] = self._evidenceImpact({}, ie)
      for coal in self._get_all_coal_compress():
        S = list(coal)
        condi = {}
        for var in S:
          condi[var] = v.loc[i, var]
        col_arr_name = self._coal_encoding(convert, coal)
        v.loc[i, f'{col_arr_name}'] = self._evidenceImpact(condi, ie)
    df = pd.DataFrame()
    for feat in self.feats_names:
      list_i_last = self.feats_names.copy()
      index_i = list_i_last.index(feat)
      list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[len(list_i_last) - 1]

      somme = 0
      for coal1, coal2 in self._gen_coalitions2(list_i_last):
        S_U_i = self._coal_encoding(convert, list(coal1))
        S = self._coal_encoding(convert, list(coal2))
        size_S = sum(S)
        somme = somme + self._compute_SHAP_i(S_U_i, S, v, size_S)
      df[feat] = somme
    self.results = df
    return df, v

  def conditional(self, train, plot=False, plot_importance=False, percentage=False, filename=None):
    """
    Compute the conditional Shap Values for each variables.

    Parameters
    ----------
    train :pandas.DataFrame
      the database
    plot: bool
      if True, plot the violin graph of the shap values
    plot_importance: bool
      if True, plot the importance plot
    percentage: bool
      if True, the importance plot is shown in percent.
    filename: str
      if not None, save the plot in the file

    Returns
    -------
      a dictionary Dict[str,float]
    """
    results, v = self._conditional(train)
    res = {}
    for col in results.columns:
      res[col] = abs(results[col]).mean()

    self._plotResults(results, v, plot, plot_importance, percentage)

    if plot or plot_importance:
      if filename is not None:
        plt.savefig(filename)
      else:
        plt.show()
      plt.close()

    return res

  ################################## Function to Compute MARGINAL SHAP Value ##################################
  def _marginal(self, df, size_sample_df):

    ie = self._init_Inference()
    convert = self._get_list_names_order()
    test = df[:size_sample_df]
    v = df.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()
    df = pd.DataFrame()

    for i in range(len(v)):
      for coal in self._get_all_coal_compress():
        intervention = test.copy()
        for var in coal:
          intervention[var] = v.loc[i, var]
        col_arr_name = self._coal_encoding(convert, coal)
        v.loc[i, f'{col_arr_name}'] = np.mean(self._logit(self._predict_marginal(intervention, ie)))

    for feat in self.feats_names:
      list_i_last = self.feats_names.copy()
      index_i = list_i_last.index(feat)
      list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[len(list_i_last) - 1]

      somme = 0
      for coal1, coal2 in self._gen_coalitions2(list_i_last):
        S_U_i = self._coal_encoding(convert, list(coal1))
        S = self._coal_encoding(convert, list(coal2))
        size_S = sum(S)
        somme = somme + self._compute_SHAP_i(S_U_i, S, v, size_S)

      df[feat] = somme

    self.results = df
    return df, v

  def marginal(self, train, sample_size=200, plot=False, plot_importance=False, percentage=False, filename=None):
    """
    Compute the marginal Shap Values for each variables.

    Parameters
    ----------
    train :pandas.DataFrame
      the database
    sample_size : int
      The computation of marginal ShapValue is very slow. The parameter allow to compute only on a fragment of the database.
    plot: bool
      if True, plot the violin graph of the shap values
    plot_importance: bool
      if True, plot the importance plot
    percentage: bool
      if True, the importance plot is shown in percent.
    filename: str
      if not None, save the plot in the file

    Returns
    -------
      a dictionary Dict[str,float]
    """
    results, v = self._marginal(train, sample_size)
    res = {}
    for col in results.columns:
      res[col] = abs(results[col]).mean()

    self._plotResults(results, v, plot, plot_importance, percentage)

    if plot or plot_importance:
      if filename is not None:
        plt.savefig(filename)
      else:
        plt.show()
      plt.close()

    return res

    ################################## MUTILATION ######################################

  @staticmethod
  def _mutilation_Inference(bn, feats_name, target):
    ie = gum.LazyPropagation(bn)
    ie.addTarget(target)
    for name in feats_name:
      ie.addEvidence(name, 0)
    return ie

  def _causal(self, train):
    v = train.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()
    ie = self._init_Inference()

    convert = self._get_list_names_order()
    df = pd.DataFrame()

    v['Baseline'] = self._evidenceImpact({}, ie)

    for coal in self._get_all_coal_compress():
      for i in range(len(v)):
        bn_temp = gum.BayesNet(self.bn)
        S = list(coal)
        condi = {}
        for var in S:
          condi[var] = v.loc[i, var]
          for parent in bn_temp.parents(var):
            bn_temp.eraseArc(parent, bn_temp.idFromName(var))
          ie = self._mutilation_Inference(bn_temp, self.feats_names, self.target)
        col_arr_name = self._coal_encoding(convert, coal)
        v.loc[i, f'{col_arr_name}'] = self._evidenceImpact(condi, ie)
    for feat in self.feats_names:
      list_i_last = self.feats_names.copy()
      index_i = list_i_last.index(feat)
      list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[len(list_i_last) - 1]

      somme = 0
      for coal1, coal2 in self._gen_coalitions2(list_i_last):
        S_U_i = self._coal_encoding(convert, list(coal1))
        S = self._coal_encoding(convert, list(coal2))
        size_S = sum(S)
        somme = somme + self._compute_SHAP_i(S_U_i, S, v, size_S)
      df[feat] = somme

    self.results = df
    return df, v

  def causal(self, train, plot=False, plot_importance=False, percentage=False, filename=None):
    """
    Compute the causal Shap Values for each variables.

    Parameters
    ----------
    train :pandas.DataFrame
      the database
    plot: bool
      if True, plot the violin graph of the shap values
    plot_importance: bool
      if True, plot the importance plot
    percentage: bool
      if True, the importance plot is shown in percent.
    filename: str
      if not None, save the plot in the file

    Returns
    -------
      a dictionary Dict[str,float]
    """
    results, v = self._causal(train)

    res = {}
    for col in results.columns:
      res[col] = abs(results[col]).mean()

    self._plotResults(results, v, plot, plot_importance, percentage)

    if plot or plot_importance:
      if filename is not None:
        plt.savefig(filename)
      else:
        plt.show()
      plt.close()

    return res

    ################################## PLOT SHAP Value ##################################

  def _plotResults(self, results, v, plot=True, plot_importance=False, percentage=False):
    ax1 = ax2 = None
    if plot and plot_importance:
      fig = plt.figure(figsize=(15, 0.5 * len(results.columns)))
      ax1 = fig.add_subplot(1, 2, 1)
      ax2 = fig.add_subplot(1, 2, 2)
    if plot:
      shap_dict = results.to_dict(orient='list')
      sorted_dict = dict(sorted(shap_dict.items(), key=lambda x: sum(abs(i) for i in x[1]) / len(x[1])))
      data = np.array([sorted_dict[key] for key in sorted_dict])
      features = sorted_dict.keys()
      v = v[features]
      colors = v.transpose().to_numpy()
      self._plot_beeswarm_(data, colors, 250, 1.5, features, ax=ax1)
    if plot_importance:
      self._plot_importance(results, percentage, ax=ax2)

  @staticmethod
  def _plot_beeswarm_(data, colors, N, K, features, cmap=None, ax=None):
    """
    returns a beeswarm plot (or stripplot) from a given data.

    Parameters
    ----------
    data: list of numpy array.
    Each elements of the list is a numpy array containing the shapley values for the feature to be displayed for a category.

    colors: list of numpy array.
    Each elements of the list is a numpy array containing the values of the data point to be displayed for a category.

    Returns
    -------
    matplotlib.pyplot.scatter
    """
    min_value = np.min(data, axis=(0, 1))
    max_value = np.max(data, axis=(0, 1))
    bin_size = (max_value - min_value) / N
    if bin_size == 0:
        bin_size = 1
    horiz_shift = K * bin_size

    if ax is None:
      _, ax = plt.subplots()
    if cmap is None:
      # Set Color Map
      ## Define the hex colors
      color1 = gum.config["notebook", "tensor_color_0"]
      color2 = gum.config["notebook", "tensor_color_1"]

      ## Create the custom colormap
      cmap = mcolors.LinearSegmentedColormap.from_list("", [color1, color2])

    for n, d in enumerate(data):
      pos = n + 1

      d_shifted = d + np.random.normal(0, horiz_shift, len(d))
      # Sorting values
      d_sort = np.sort(d_shifted)

      # Creation of bins
      d_x = bin_size
      if (np.max(d_sort) - np.min(d_sort)) % d_x == 0:
        nb_bins = (np.max(d_sort) - np.min(d_sort)) // d_x
      else:
        nb_bins = (np.max(d_sort) - np.min(d_sort)) // d_x + 1
      bins = [np.min(d_sort) + i * d_x for i in range(int(nb_bins) + 1)]

      # Group by Bins
      subarr = []
      for k in range(1, len(bins)):
        group = d_sort[np.logical_and(d_sort >= bins[k - 1], d_sort < bins[k])]
        subarr.append(group)

      # For each bin compute the d_y (vertical shift)
      d_y = 0.025
      subarr_jitter = deepcopy(subarr)
      for i in range(len(subarr)):
        L = subarr[i].size
        if L > 0:
          for j in range(L):
            shift = d_y * (L - 1) / 2 - d_y * j
            subarr_jitter[i][j] = shift

      jitter = np.concatenate(subarr_jitter)

      sc = ax.scatter(d_shifted, pos + jitter, s=10, c=colors[n], cmap=cmap, alpha=0.7)

    ## Create the colorbar
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.1)

    cbar = plt.colorbar(sc, cax=cax, aspect=80)
    cbar.set_label('Data Point Value')

    ## Add text above and below the colorbar
    cax.text(0.5, 1.025, 'High', transform=cax.transAxes, ha='center', va='center', fontsize=10)
    cax.text(0.5, -0.025, 'Low', transform=cax.transAxes, ha='center', va='center', fontsize=10)

    ## Add x-axis tick labels
    ax.set_yticks([i + 1 for i in range(len(features))])
    ax.set_yticklabels(features)

    ## Set axis labels and title
    ax.set_ylabel('Features')
    ax.set_xlabel('Impact on output')
    ax.set_title('Shapley value (impact on model output)')

    # Show plot
    return ax.get_figure()

  @staticmethod
  def _plot_importance(results, percentage=False, ax=None):
    series = pd.DataFrame(abs(results).mean(), columns=['value'])
    series['feat'] = series.index

    if ax is None:
      _, ax = plt.subplots()

    if percentage:
      series['value'] = series['value'].div(series['value'].sum(axis=0)).multiply(100)
      series = series.sort_values('value', ascending=True)
      ax.barh(series.feat, series.value, align='center')
      ax.set_xlabel('Mean(|SHAP value|)')
      ax.set_title('Feature Importance in %')
    else:
      series = series.sort_values('value', ascending=True)
      ax.barh(series.feat, series.value, align='center')
      ax.set_xlabel('Mean(|SHAP value|)')
      ax.set_title('Feature Importance')

    return ax.get_figure()

  @staticmethod
  def _plot_scatter(results, ax=None):
    if ax is None:
      _, ax = plt.subplots()

    res = {}
    for col in results.columns:
      res[col] = results[col].to_numpy()
    names = list(res.keys())
    values = list(res.values())
    for xe, ye in zip(names, values):
      ax.scatter(ye, [xe] * len(ye))
    ax.set_title('Shapley value (impact on model output)')
    return ax.get_figure()

  @staticmethod
  def _plot_violin(results, ax=None):
    data = []
    pos = []
    label = []
    series = pd.DataFrame(abs(results).mean(), columns=['value'])
    series = series.sort_values('value', ascending=True)
    series['feat'] = series.index
    for i, col in enumerate(series.feat):
      data.append(results[col].to_numpy())
      pos.append(i)
      label.append(col)
    if ax is None:
      _, ax = plt.subplots()
    ax.violinplot(data, pos, vert=False)
    ax.set_yticks(pos)
    ax.set_yticklabels(label)
    ax.set_title('Shapley value (impact on model output)')
    return ax.get_figure()


def getShapValues(bn,shaps, cmap='plasma'):
  """
  Just a wrapper around BN2dot to easily show the Shap values

  Parameters
  ----------
  bn : pyagrum.BayesNet
    The Bayesian network
  shaps: dict[str,float]
    The (Shap) values associates to each variable
  cmap: Matplotlib.ColorMap
    The colormap used for colouring the nodes

  Returns
  -------
    a pydot.graph
  """
  from pyagrum.lib.bn2graph import BN2dot
  norm_color = {}
  raw = list(shaps.values())
  norm = [float(i) / sum(raw) for i in raw]
  for i, feat in enumerate(list(shaps.keys())):
    norm_color[feat] = norm[i]
  cm = plt.get_cmap(cmap)
  g = BN2dot(bn,
             nodeColor=norm_color,
             cmapNode=cm
             )
  return g
