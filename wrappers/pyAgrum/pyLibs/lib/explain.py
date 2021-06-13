# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
# (pierre-henri.wuillemin@lip6.fr)
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
tools for BN qualitative analysis
"""

import math
from typing import Dict

import numpy as np
import pandas as pd
from itertools import compress, product
import matplotlib as mpl
import matplotlib.pyplot as plt

import pyAgrum as gum
from pyAgrum.lib.bn2graph import BN2dot


class ShapValues:
    @staticmethod
    def self._logit(p):
        return np.log(p / (1 - p))

    def __init__(self, bn, target):
        self.bn = bn
        self.target = target
        self.feats_names = self._get_feats_name(bn, target)

    ################################## VARIABLES ##################################

    def _get_feats_name(self, bn, target):
        list_feats_name = list(bn.names())
        list_feats_name.remove(target)
        return list_feats_name

    def _get_list_names_order(self):
        ### Return a list of BN's nodes names
        list_node_names = [None] * len(self.bn.names())
        for name in self.bn.names():
            list_node_names[self.bn.idFromName(name)] = name
        return list_node_names

    def _coal_encoding(self, convert, coal):
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
        return (list(compress(self.feats_names, mask)) for mask in product(*[[0, 1]] * (len(self.feats_names))))
        ### Yield ( set(compress(self.feats_names, mask)) for mask in product(*[[0,1]]*(len(self.feats_names))) )

    ################################## PREDICTION ##################################
    def _filtrage(self, df, conditions):
        ### Return : a selected part of DataFrame based on conditions, conditions must be in a dict :
        ### The key is the name of the features and the value is the value that the features should take
        ### Example : {'X1':0, 'X2':1}
        if conditions == {}:
            return df
        else:
            first = next(iter(conditions))
            new_df = df[df[first] == conditions[first]]
            conditions.pop(first)
            return filtrage(new_df, conditions)

    def _init_Inference(self):
        ie = gum.LazyPropagation(self.bn)
        ie.addTarget(self.target)
        for name in self.feats_names:
            ie.addEvidence(name, 0)
        return ie

    ##### Prediction fonctions ####

    def _pred_markov_blanket(self, df, ie, markov_blanket):
        unique = df.groupby(markov_blanket).agg(freq=('Y', 'count')).reset_index()
        result = 0
        for i in range(len(unique)):
            for name in markov_blanket:
                ie.chgEvidence(name, str(unique[name].iloc[i]))
            ie.makeInference()
            predicted = ie.posterior('Y').toarray()[1]
            result = result + self._logit(predicted) * unique['freq'].iloc[i] / len(df)
        return result

    def _pred_markov_blanket_logit(self, df, ie, markov_blanket):
        unique = df.groupby(markov_blanket).agg(freq=('Y', 'count')).reset_index()
        result = 0
        for i in range(len(unique)):
            for name in markov_blanket:
                ie.chgEvidence(name, str(unique[name].iloc[i]))
            ie.makeInference()
            predicted = ie.posterior('Y').toarray()[1]
            result = result + predicted * unique['freq'].iloc[i] / len(df)
        return self._logit(result)

    def _evidenceImpact(self, condi, ie):
        ie.eraseAllEvidence()
        for key in condi.keys():
            ie.addEvidence(key, str(condi[key]))
        ie.makeInference()
        return self._logit(ie.posterior('Y').toarray()[1])

    ############################## MARGINAL#########################################

    def _predict_marginal(self, df, ie):
        result = []
        for i in range(len(df)):
            for name in self.feats_names:
                ie.chgEvidence(name, str(df[name].iloc[i]))
            ie.makeInference()
            result.append(ie.posterior('Y').toarray()[1])
        return np.array(result)

    ################################## COMPUTE SHAP ##################################

    def _compute_SHAP_i(self, S_U_i, S, v, size_S):
        size_all_features = len(self.bn.nodes()) - 1
        return self._coeff(size_S, size_all_features) * (v[f'{S_U_i}'] - v[f'{S}'])

    def _coeff(self, S_size, len_features):
        ## coal : list of features in the coalition
        ## features : number of features
        M = len_features
        return (fact(S_size) * fact(M - S_size - 1)) / (fact(M))

    ################################## Get the two coalitions to substract S_U_i and S #################################

    def _gen_coalitions2(self, list_feats):
        ### !!!! THE FEATURE i HAVE TO BE THE LAST IN THE LIST OF FEATURES !!!
        ### Return : all coalitions with the feature
        for mask in product(*[[0, 1]] * (len(list_feats) - 1)):
            S_U_i = compress(list_feats, mask + (1,))
            S = compress(list_feats, mask + (0,))
            yield S_U_i, S

    ################################## Function to Compute CONDITIONNAL SHAP Value ##################################
    def conditional(self, train:pd.DataFrame)->Dict[str,float]:
        """
        return the conditional ShapValues for each variable

        :param train: using this dataframe
        :return: a dictionary (name of variable,shapvalue)
        """
        markov_blanket = self._get_markov_blanket()

        ie = self._init_Inference()

        v = train.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()

        convert = self._get_list_names_order()
        v_cache = v.copy()

        for i in range(len(v)):
            v_cache['Baseline'] = self._evidenceImpact({}, ie)
            for coal in self._get_all_coal_compress():
                S = list(coal)
                condi = {}
                for var in S:
                    condi[var] = v.loc[i, var]
                col_arr_name = self._coal_encoding(convert, coal)
                v_cache.loc[i, f'{col_arr_name}'] = self._evidenceImpact(condi, ie)

        for feat in self.feats_names:
            list_i_last = self.feats_names.copy()
            index_i = list_i_last.index(feat)
            list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[
                len(list_i_last) - 1]

            somme = 0
            for coal1, coal2 in self._gen_coalitions2(list_i_last):
                S_U_i = self._coal_encoding(convert, list(coal1))
                S = self._coal_encoding(convert, list(coal2))
                size_S = sum(S)
                somme = somme + self._compute_SHAP_i(S_U_i, S, v_cache, size_S)
            v[f'SHAP({feat})'] = somme
        return v

    def conditionalMarkovBlanket(self, train, pred='mean_predict_markov_blanket'):
        markov_blanket = self._get_markov_blanket()

        func = {
            '_pred_markov_blanket': _pred_markov_blanket,
            '_pred_markov_blanket_logit': _pred_markov_blanket_logit
        }

        ie = self._init_Inference()

        v = train.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()

        convert = self._get_list_names_order()
        v_cache = v.copy()

        v_cache['Baseline'] = func[pred](train, ie, markov_blanket)
        for i in range(len(v)):
            for coal in get_all_coal_compress():
                S = list(coal)
                condi = {}
                for var in S:
                    condi[var] = v.loc[i, var]
                    ### bn.minimal
                col_arr_name = self._coal_encoding(convert, coal)
                v_cache.loc[i, f'{col_arr_name}'] = func[pred](self._filtrage(train, condi), ie, markov_blanket)

        for feat in self.feats_names:
            list_i_last = self.feats_names.copy()
            index_i = list_i_last.index(feat)
            list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[
                len(list_i_last) - 1]

            somme = 0
            for coal1, coal2 in self._gen_coalitions2(list_i_last):
                S_U_i = self._coal_encoding(convert, list(coal1))
                S = self._coal_encoding(convert, list(coal2))
                size_S = sum(S)
                somme = somme + self._compute_SHAP_i(S_U_i, S, v_cache, size_S)

            v[f'SHAP({feat})'] = somme

            return v

    ################################## Function to Compute MARGINAL SHAP Value ##################################

    def marginal(self, df, size_sample_df):

        ie = self._init_Inference()
        convert = self._get_list_names_order()
        test = df[:size_sample_df]
        v = df.groupby(self.feats_names).agg(freq=(self.feats_names[0], 'count')).reset_index()
        markov_blanket = self._get_markov_blanket()

        v_cache = v.copy()
        for i in range(len(v)):
            for coal in self._get_all_coal_compress():
                intervention = test.copy()
                for var in coal:
                    intervention[var] = v_cache.loc[i, var]
                col_arr_name = self.coal_encoding(convert, coal)
                v_cache.loc[i, f'{col_arr_name}'] = np.mean(_logit(self.predict_marginal(intervention, ie)))

        for feat in self.feats_names:
            list_i_last = self.feats_names.copy()
            index_i = list_i_last.index(feat)
            list_i_last[len(list_i_last) - 1], list_i_last[index_i] = list_i_last[index_i], list_i_last[
                len(list_i_last) - 1]

            somme = 0
            for coal1, coal2 in self._gen_coalitions2(list_i_last):
                S_U_i = self._coal_encoding(convert, list(coal1))
                S = self._coal_encoding(convert, list(coal2))
                size_S = sum(S)
                somme = somme + self._compute_SHAP_i(S_U_i, S, v_cache, size_S)

            v[f'SHAP({feat})'] = somme

        return v

    ################################## PLOT SHAP Value ##################################

    def plot_importance(self, results, n_feats, percentage=False):
        series = pd.DataFrame(abs(results.iloc[:, n_feats + 1:]).mean(), columns=['value'])
        series['feat'] = series.index

        fig, ax = plt.subplots()
        plt.style.use('fivethirtyeight')

        if percentage == True:
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

        plt.show()

    def showShapValues(self, results, feats, cmap='plasma'):
        n_feats = len(feats)
        node_color = {}
        for node in feats:
            node_color[node] = abs(results.iloc[:, n_feats + 1:]).mean()[f'SHAP({node})']

        cm = plt.get_cmap(cmap)
        g = BN2dot(self.bn,
                   nodeColor=node_color,
                   cmapNode=cm)
        return g


_cdict = {
    'red': ((0.0, 0.1, 0.3),
            (1.0, 0.6, 1.0)),
    'green': ((0.0, 0.0, 0.0),
              (1.0, 0.6, 0.8)),
    'blue': ((0.0, 0.0, 0.0),
             (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)


def independenceListForPairs(bn):
    """
    returns a list of triples `(i,j,k)` for each non arc `(i,j)` such that `i` is independent of `j` given `k`.
  
    Parameters
    ----------
    bn: gum.BayesNet
      the Bayesian Network
  
    Returns
    -------
    List[(str,str,List[str])]
      the list of independence found in the structure of BN.
    """
    import itertools

    def powerset(iterable):
        xs = list(iterable)
        # note we return an iterator rather than a list
        return itertools.chain.from_iterable(itertools.combinations(xs, n) for n in range(len(xs) + 1))

    # testing every d-separation
    l = []
    nams = sorted(bn.names())
    for i in sorted(bn.names()):
        nams.remove(i)
        for j in nams:
            if not (bn.existsArc(i, j) or bn.existsArc(j, i)):
                for k in powerset(sorted(bn.names() - {i, j})):
                    if bn.isIndependent(i, j, k):
                        l.append((i, j, k))
                        break
    return l


def plotIndependenceListForPairs(bn, filename, alphabetic=False):
    """
    plot the p-value of the chi2 test of a (as simple as possible) independence proposition for every non arc.
  
    Parameters
    ----------
    bn : gum.BayesNet
      the Bayesian network
  
    filename : str
      the name of the csv database
  
    alphabetic : bool
      if True, the list is alphabetically sorted else it is sorted by the p-value
  
    Returns
    -------
      matplotlib.Figure
    """
    import pylab

    learner = gum.BNLearner(filename, bn)
    vals = {}
    for indep in independenceListForPairs(bn):
        key = "$" + indep[0] + " \\perp " + indep[1]
        if len(indep[2]) > 0:
            key += " \\mid " + ",".join(indep[2])
        key += "$"
        vals[key] = learner.chi2(*indep)[1]

    if not alphabetic:
        sortedkeys = sorted(vals, key=vals.__getitem__, reverse=False)
    else:
        sortedkeys = list(vals.keys())

    fig = pylab.figure(figsize=(10, 0.25 * len(vals)))
    ax = fig.add_subplot(1, 1, 1)
    ax.plot([vals[k] for k in sortedkeys], sortedkeys, "o")
    ax.grid(True)
    ax.vlines(x=0.05, ymin=0, ymax=len(vals) - 1, colors='purple')
    ax.add_patch(mpl.patches.Rectangle((0, 0), 0.05, len(vals) - 1, color="yellow"))
    return fig


def _normalizeVals(vals, hilightExtrema=False):
    """
    normalisation if vals is not a proba (max>1)
    """
    ma = float(max(vals.values()))
    mi = float(min(vals.values()))
    if ma == mi:
        return None
    else:
        if not hilightExtrema:
            vmi = 0.01
            vma = 0.99
        else:
            vmi = 0
            vma = 1

        try:
            items = vals.items()
        except AttributeError:
            items = vals.iteritems()

        return {name: vmi + (val - mi) * (vma - vmi) / (ma - mi) for name, val in items}


def _reprInformation(bn, evs, size, cmap, asString):
    """
    repr a bn annoted with results from inference : Information and mutual informations
  
    :param bn: the BN
    :param evs: map of evidence
    :param size:  size of the graph
    :param cmap: colour map used
    :return: the HTML string
    """
    # fix DeprecationWarning of base64.encodestring()
    try:
        from base64 import encodebytes
    except ImportError:  # 3+
        from base64 import encodestring as encodebytes

    import IPython.display
    import IPython.core.pylabtools
    from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

    ie = gum.LazyPropagation(bn)
    ie.setEvidence(evs)
    ie.makeInference()

    idEvs = {bn.idFromName(name) for name in evs}
    nodevals = {bn.variable(n).name(): ie.H(n)
                for n in bn.nodes() if not n in idEvs}
    arcvals = {(x, y): ie.I(x, y) for x, y in bn.arcs()}
    gr = BN2dot(bn, size, nodeColor=_normalizeVals(nodevals, hilightExtrema=False), arcWidth=arcvals, cmapNode=cmap,
                cmapArc=cmap,
                showMsg=nodevals)

    mi = min(nodevals.values())
    ma = max(nodevals.values())

    fig = mpl.figure.Figure(figsize=(8, 3))
    canvas = fc(fig)
    ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
    norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
    cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                    norm=norm,
                                    orientation='horizontal')
    cb1.set_label('Entropy')
    png = IPython.core.pylabtools.print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
    png_legend = "<img style='vertical-align:middle' src='data:image/png;base64,%s'>" % encodebytes(png).decode(
        'ascii')

    gsvg = IPython.display.SVG(gr.create_svg())

    sss = "<div align='center'>" + gsvg.data + "</div>"
    sss += "<div align='center'>"
    sss += "<font color='" + \
           gum._proba2bgcolor(0.01, cmap) + "'>" + str(mi) + "</font>"
    sss += png_legend
    sss += "<font color='" + \
           gum._proba2bgcolor(0.99, cmap) + "'>" + str(ma) + "</font>"
    sss += "</div>"

    if asString:
        return sss
    else:
        return IPython.display.display(IPython.display.HTML(sss))


def getInformation(bn, evs=None, size=None, cmap=_INFOcmap):
    """
    get a HTML string for a bn annotated with results from inference : entropy and mutual information
  
    :param bn: the BN
    :param evs: map of evidence
    :param size:  size of the graph
    :param cmap: colour map used
    :return: the HTML string
    """
    if size is None:
        size = gum.config["notebook", "default_graph_size"]

    if evs is None:
        evs = {}

    return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs=None, size=None, cmap=_INFOcmap):
    """
    show a bn annotated with results from inference : entropy and mutual information
  
    :param bn: the BN
    :param evs: map of evidence
    :param size:  size of the graph
    :param cmap: colour map used
    :return: the graph
    """
    if evs is None:
        evs = {}

    if size is None:
        size = gum.config["notebook", "default_graph_size"]
    return _reprInformation(bn, evs, size, cmap, asString=False)
