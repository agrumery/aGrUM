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

# The imports
import numpy as np

import warnings

import math
from itertools import combinations

import matplotlib.pyplot as plt
from pyagrum.lib.plots._waterfall import waterfall
from pyagrum.lib.plots._beeswarm import beeswarm
from pyagrum.lib.plots._bar import bar

import pyagrum as gum

class CustomShapleyCache:
    def __init__(self, max_capacity: int = 1000):
        if max_capacity <= 0:
            raise ValueError("max_capacity must be a positive integer.")
        self._cache = {}  # Structure: {longueur_str: { (entier, str): valeur }}
        self._current_k_index = -1  # Pour suivre l'indice k actuel
        self._max_capacity = max_capacity # Capacité maximale du cache en nombre d'éléments
        self._current_size = 0 # Taille actuelle du cache

    #TODO: Improve the efficiency of this method.
    @staticmethod
    def generate_keys(bn, target, feat, nodes) :
        minimal = bn.minimalCondSet(target, nodes)
        key1 = tuple(nodes)
        key2 = tuple(n for n in nodes if n != feat)
        diff = next((n for n in nodes if n not in minimal), None)
        if diff is not None:
            key3 = tuple(n for n in nodes if n != diff)
        else:
            key3 = key1
        return key1, key2, key3

    def get(self, int_key: int, tuple_key: tuple):
        tuple_len = len(tuple_key)
        return self._cache[tuple_len].get((int_key, tuple_key), None)

    def set(self, int_key: int, tuple_key: tuple, value):
        tuple_len = len(tuple_key)
        if tuple_len not in self._cache:
            self._cache[tuple_len] = {}
            self._current_k_index = tuple_len

        # Si l'entrée n'existe pas déjà, on augmente la taille
        if (int_key, tuple_key) not in self._cache[tuple_len]:
            self._current_size += 1
        self._cache[tuple_len][(int_key, tuple_key)] = value

        # Vérifier si la capacité est dépassée et déclencher la purge
        if self._current_size > self._max_capacity:
            self._perform_purge()

    def _perform_purge(self):
        """
        Effectue la purge des entrées obsolètes si l'indice k a été avancé.
        """
        if self._current_k_index < 2: # Pas de longueur k-2 possible si k est trop petit
            return

        str_len_to_purge = self._current_k_index - 2

        # Purger toutes les longueurs de chaîne <= str_len_to_purge
        # On itère sur une copie des clés pour pouvoir modifier le dictionnaire pendant l'itération
        for length in list(self._cache.keys()):
            if length <= str_len_to_purge:
                self._current_size -= len(self._cache[length])
                del self._cache[length]
                

    def __len__(self):
        return self._current_size

    def __str__(self):
        # Affiche le cache par longueur de clé pour une meilleure visibilité
        items_by_length = {k: len(v) for k, v in self._cache.items()}
        return (f"Cache (k={self._current_k_index}, size={self._current_size}/{self._max_capacity}): "
                f"Contents by length: {items_by_length}")
    
class ConditionalShapValues :
    """
    The ConditionalShapValues class computes the conditional Shapley values for a given target node in a Bayesian Network.
    """
    #NOTE: How can we optimize this function? 
    @staticmethod
    def _logit(p):
        p = np.asarray(p)  # garantit que p est un tableau NumPy (même si scalaire)
        with np.errstate(divide='ignore', invalid='ignore'):
            result = np.log(p / (1 - p))
        result = np.where(p == 0, -np.inf, result)
        result = np.where(p == 1,  np.inf, result)
        return result
        
    @staticmethod
    def _identity(x):
        # Useful if the user wants to use the identity function instead of logit.
        return x

    def __init__(self, bn, target, logit=True) :
        """
        params:
        ------
        :bn -> The Bayesian Network.
        :target -> The node id of the target.
        :logit -> If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        :TypeError -> If bn is not a gum.BayesNet instance.
        :ValueError -> If target is not a valid node id in the Bayesian Network.
        :UserWarning -> If logit is not a boolean, a warning is issued.
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
        self.M = len( bn.nodes() ) # Total number of nodes in the Bayesian Network.
        self.target = target # ID of the target node.
        self.feat_names = np.empty(self.M, dtype=object) # Array to store feature names by their node ID.
        for name in self.bn.names() :
            # Populating feat_names with node names based on their IDs.
            self.feat_names[self.bn.idFromName(name)] = name
        self.target_name = self.feat_names[self.target]

        self.ie = gum.LazyPropagation(self.bn) # Inference engine for the Bayesian Network.
        self.ie.addTarget(self.target) # Setting the target for inference.
        self.base_line = self.ie.posterior(self.target).toarray()

        self.func = self._logit if logit else self._identity

    def _coalitions(self, elements_for_coalitions) :
        all_coalitions = []
        # Parcourt toutes les tailles possibles de coalitions, de 0 (ensemble vide)
        # jusqu'à la taille maximale des éléments disponibles.
        for r in range(1, len(elements_for_coalitions) + 1):
            for combo in combinations(elements_for_coalitions, r):
                all_coalitions.append(list(combo))
        return all_coalitions

    def _invcoeff_shap(self, m, s) :
        return (m - s) * math.comb(m, s)

    # TODO: Improve the efficiency of this method.
    def _coalition_contribution(self, k, ex, feature, nodes_id, nodes_vals, cache) :
        key1, key2, key_m = cache.generate_keys(self.bn, self.target, feature, nodes_id)
        if k == 0 :
            self.ie.eraseAllEvidence() 
            if( len(key1) == len(key_m) ) :
                for k, v in zip(nodes_id, nodes_vals) :
                    try :
                        self.ie.addEvidence(int(k), int(v))
                    except ValueError :
                        self.ie.addEvidence(int(k), v) 
                   
                cache.set(ex, key1, self.func(self.ie.posterior(self.target).toarray()))
            else :
                if len(key1) > 1 :
                    cache.set(ex, key1, cache.get(ex, key_m))
                else :
                    cache.set(ex, key1, cache.get(-1, ''))

        posterior_prob_with = cache.get(ex, key1)
        posterior_prob_without = cache.get(ex, key2) if len( key1 ) > 1 else cache.get(-1, '')
        return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(self.M - 1, len(nodes_id) - 1)

    def _conditional_1dim(self, data, elements) :
        contributions = np.zeros( (self.M, self.bn.variable(self.target).domainSize()) )

        self.ie.eraseAllEvidence() # Clears all evidence from the inference engine.

        # Cache management...
        size = 5000 # Cache size.
        cache = CustomShapleyCache(size) # Initializes the custom cache.
        # Sets the baseline probability in the cache.
        cache.set(-1, '', self.func( self.base_line ))

        coalitions = self._coalitions(elements) # Generates coalitions.
        for nodes_id in coalitions :
            nodes_vals = data[nodes_id]
            # Iterates through each feature in the current coalition.
            for k, feature in enumerate(nodes_id) :
                # Accumulates the contribution for each feature.
                contributions[feature] += self._coalition_contribution(k, 0, int(feature), nodes_id, nodes_vals, cache)
        return contributions

    def _conditional_ndim(self, data, elements) :
        contributions = np.zeros( (self.M, len(data), self.bn.variable(self.target).domainSize()) )

        self.ie.eraseAllEvidence() # Clears all evidence.
        # Cache management...
        size = 5000
        cache = CustomShapleyCache(size)
        # Sets the baseline probability in the cache.
        cache.set(-1, '', self.func( self.base_line ))

        coalitions = self._coalitions(elements)
    
        for nodes_id in coalitions :
            data_vals = data[:, nodes_id]
            for ex, nodes_vals in enumerate(data_vals) :
                for k, feature in enumerate( nodes_id ) :
                    contributions[feature, ex] += self._coalition_contribution(k, ex, int(feature), nodes_id, nodes_vals, cache)
        
        return contributions

    def localShap(self, x, elements, y, waterfall_plot=False) :
        # Calculates local Shapley contributions for a single instance 'x'.
        shap_contributions = self._conditional_1dim(x, elements)
        if waterfall_plot :
            # Determines the target value (most probable class) based on baseline and summed contributions.
            y = np.argmax( self.func( self.base_line ) + np.sum(shap_contributions, axis=0) ) if y is None else y
            try :
                y = int(y)  # Ensures y is an integer.
            except ValueError :
                raise ValueError("Target value 'y' must be an integer representing the class index, but got {}".format(y))
            
            feature_names = self.feat_names[elements] # Feature names (excluding target).
            feature_values = x[elements] # Values of these features from the input instance.
            # Calls the waterfall plot function from ShapleyPlots.
            waterfall(base= self.func( self.base_line )[y],
                                    shap_values=shap_contributions[elements, y],
                                    feature_values=feature_values,
                                    feature_names=feature_names,
                                    target_name=self.target_name,
                                    function=self.func.__name__,
                                    target_value=y,
                                )
            
        return {feat : float(shap_contributions[self.bn.idFromName(feat), y])
                            for feat in self.feat_names[elements]
            }
    
    # TODO: The function calculates contributions for all targets but it is not necessary since
    #       y is indispensable for this function. (We have to improve this ...)

    def globalShap(self, 
                   x,
                   elements,
                   y,
                   N=100,
                   beeswarm_plot=False,
                   bar_plot=False,
                   ) :
        # Remove duplicate rows in x and unused columns.
        mask_cols = [i for i in range(self.M) if i not in elements]
        _, idx = np.unique(x[:, elements], axis=0, return_index=True)
        x = x[idx, :]
        x[:, mask_cols] = 0
        
        # Calculates the conditional Shapley contributions for multiple examples.
        shap_contributions = self._conditional_ndim(x, elements)
        
        # Handles different plot types.
        feature_names = self.feat_names[elements]
        features = x[:, elements]
        shap_data = shap_contributions[elements, :, y].T
        # Creation of a figure for the plots.
        n_figures = sum([beeswarm_plot, bar_plot])
        if n_figures > 0 :            
            _, axs = plt.subplots(1, n_figures, figsize=(n_figures * 7, 5))
            if n_figures == 1 :
                axs = [axs]
            
            plot_index = 0
            if beeswarm_plot :
                beeswarm(shap_contributions[elements, :, y].T, features, feature_names, ax=axs[plot_index])
                plot_index += 1
            if bar_plot :
                bar(shap_contributions[elements, :, y].T, feature_names, ax=axs[plot_index])
                plot_index += 1
                
            plt.tight_layout()
            plt.show()
        
        impotances = np.mean(np.abs(shap_contributions), axis=1)
        
        return {feat : float(impotances[self.bn.idFromName(feat), y])
                          for feat in self.feat_names[elements]
            }