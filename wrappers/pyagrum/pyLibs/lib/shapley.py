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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

# The imports
from pyagrum.lib._conditional import ConditionalShapValues

import pyagrum as gum
import pandas as pd
import numpy as np

import warnings
    
class ShapValues :
    """
    Class to compute Shapley values for a target variable in a Bayesian network.
    """

    def __init__(self, bn, target, logit=True):
        """
        Initialize the Shapley value calculator.

        :param bn: The Bayesian network.
        :param target: The target variable for which to compute Shapley values.
        :param _logit: If True, use the logit function; otherwise, use the identity function.
        """
        if not isinstance(bn, gum.BayesNet) :
            raise TypeError("bn must be a gum.BayesNet instance, but got {}".format(type(bn)))
        if isinstance(target, str) :
            if target not in bn.names() :
                raise ValueError("Target node name '{}' not found in the Bayesian Network.".format(target))
            target = bn.idFromName(target) # Convert node name to ID.
        elif isinstance(target, int) :
            if target not in bn.nodes() :
                raise ValueError("Target node ID {} not found in the Bayesian Network.".format(target))
        else :
            raise TypeError("Target must be a node ID (int) or a node name (str), but got {}".format(type(target)))
        if not isinstance(logit, bool) :
            warnings.warn("logit should be a boolean, unexpected calculation may occur.", UserWarning)

        # Class attributes.
        self.bn = bn
        self.target = target
        self.logit = logit

    def conditional(self, df, y=1, N=100, plot=False, plot_importance=False) :
        """
        Compute conditional Shapley values for a given dataset.
        
        params:
        ------
        df: pandas DataFrame or dict containing the dataset to explain.
            - If df is None a sample of N rows will be generated from the Bayesian network and used to compute the Shapley values.
            - If df has one row, the Shapley values computed will be local to this row and a waterfall plot will be generated if the parameter plot is set to True.
            - Else, the Shapley values computed will be global (the mean of the absolute contributions) and a beeswarm plot will be generated if the parameter plot is set to True and a bar plot will be generated 
            if the parameter plot_importance is set to True.
            
            pandas Series containing the instance to explain.
            - The Shapley values computed will be local to this instance and a waterfall plot will be generated if the parameter plot is set to True.

        y: int, default=1
            The value of the target variable for which to compute the Shapley values.
            If y is None, the Shapley values will be computed for all values of the target variable then the plots will not be generated.
            If a global explanation is wanted then y is not optional.
        
        N: int, default=100
            The number of rows to sample from the Bayesian network if df is None.
            If df is not None, this parameter is ignored.

        plot: bool, default=False
            If True, generate a waterfall plot if df has one row, or a beeswarm plot if df has more than one row.

        plot_importance: bool, default=False
            If True, generate a bar plot of the absolute contributions of each feature.
            This parameter is ignored if df has one row.
        """

        explainer = ConditionalShapValues(self.bn, self.target, self.logit)

        if df is None :
            # If df is None, we sample N rows from the Bayesian network. 
            if not isinstance(y, int) :
                raise TypeError("y is not optional for global explanations, it must be an integer.")
            if not isinstance(N, int) :
                raise TypeError("Since df is None, N must be an integer, but got {}".format(type(N)))
            if N < 1 :
                raise ValueError("N must be greater than 0, but got {}".format(N))
            
            x = gum.generateSample(self.bn, N)[0].astype(int).reindex(columns=explainer.feat_names).to_numpy()
            elements = [i for i in range(explainer.M) if i != explainer.target]

            result = explainer.globalShap(x, sorted(elements), y=y, beeswarm_plot=plot, bar_plot=plot_importance)

        elif isinstance(df, pd.Series) :
            # Here we are sure that df is a single instance (a Series).
            s = df.dropna()
            x = np.empty(explainer.M, dtype=np.int64)
            elements = []
            for feat in s.index :
                id = self.bn.idFromName(feat)
                x[id] = s[feat]
                if id != self.target :
                    elements.append(id)
            result = explainer.localShap(x, sorted(elements), y=y, waterfall_plot=plot)

        elif isinstance(df, pd.DataFrame) :
            df_clean = df.dropna(axis=1)
            if len(df_clean) == 1 :
                # Here we are sure that df is a single instance (a DataFrame with one row).
                x = np.empty(explainer.M, dtype=np.int64)
                elements = []
                for feat in df_clean.columns :
                    id = self.bn.idFromName(feat)
                    x[id] = df_clean[feat].values[0]
                    if id != self.target :
                        elements.append(id)
                result = explainer.localShap(x, sorted(elements), y=y, waterfall_plot=plot)
        
            else :
                # Here we are sure that df is a DataFrame with more than one row.
                if not isinstance(y, int) :
                    raise TypeError("y is not optional for global explanations, it must be an integer.")
                
                x = np.empty((len(df_clean), explainer.M), dtype=np.int64)
                elements = []
                for feat in df_clean.columns :
                    id = self.bn.idFromName(feat)
                    x[:, id] = df_clean[feat].values
                    if id != self.target :
                        elements.append(id)
                result = explainer.globalShap(x, sorted(elements), y=y, beeswarm_plot=plot, bar_plot=plot_importance)
            
        elif isinstance(df, dict) :
            try :
                N = len(list(df.values())[0])
                if not isinstance(list(df.values())[0], (list, np.ndarray)) :
                    raise TypeError("Each value in the dictionary must be a list or a numpy array.")  
                # Here we are sure that df is a dictionary with more than one row.
                if not isinstance(y, int) :
                    raise TypeError("y is not optional for global explanations, it must be an integer.") 
                elements = []
                x = np.empty((N, explainer.M), dtype=np.int64)
                for feat in df.keys() :
                    if all(not(x is None) and not(isinstance(x, float) and np.isnan(x)) for x in df[feat]) :
                        id = self.bn.idFromName(feat)
                        x[:, id] = df[feat]
                        if id != self.target :
                            elements.append(id)
                result = explainer.globalShap(x, sorted(elements), y=y, beeswarm_plot=plot, bar_plot=plot_importance)

            except TypeError :
                # Here we are sure that df is a single instance (a dictionary with one row).
                x = np.empty(explainer.M, dtype=np.int64)
                elements = []
                for feat in df.keys() :
                    if not(df[feat] is None) and not(isinstance(df[feat], float)) :
                        id = self.bn.idFromName(feat)
                        x[id] = df[feat]
                        if id != self.target :
                            elements.append(id)
                result = explainer.localShap(x, sorted(elements), y=y, waterfall_plot=plot)
        
        else :
            raise TypeError("df must be a pandas DataFrame, Series or a dictionary, but got {}".format(type(df)))
        
        return result