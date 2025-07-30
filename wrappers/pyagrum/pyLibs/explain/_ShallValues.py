# Imports
from abc import abstractmethod
from pyagrum.explain._Explainer import Explainer
from pyagrum.explain._Explanation import Explanation
from pyagrum.explain._FIFOCache import FIFOCache

# Calculations
import pandas as pd
import numpy as np

# aGrUM
import pyagrum as gum
# GL
import warnings


class ShallValues(Explainer):
    """
    Adaptation of SHAP values into SHALL values.
    """

    def __init__(self, bn: gum.BayesNet, background: tuple | None, sample_size:int=1000, log:bool=True):
        """
        params:
        ------
        :bn -> The Bayesian Network.
        :background -> If None then sample of size 'sample_size' would be generated from the Bayesian Network. Otherwise, should be 
            a tuple of a dataframe and a boolean = True if there are strings in a dataframe and False otherwise.
        :logit -> If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        :TypeError -> If bn is not a gum.BayesNet instance.
        :ValueError -> If target is not a valid node id in the Bayesian Network.
        :UserWarning -> If logit is not a boolean, a warning is issued.s
        """
        super().__init__(bn)       
        self.vars_ids = sorted(bn.ids(self.feat_names))

        # Processing background data
        if background is None:
            if not isinstance(sample_size, int):
                raise TypeError("When `data`=None, `sample_size` must be an integer, but got {}".format(type(sample_size)))
            else:
                if sample_size < 1:
                    raise ValueError("`sample_size` must be greater than 1, but got {}".format(sample_size))
                elif sample_size < 10:
                    warnings.warn("The sample size is small, which may lead to biased Shapley values.")
            data = gum.generateSample(self.bn, sample_size, with_labels=False)[0].reindex(columns=self.feat_names).to_numpy()
        else :
            if not isinstance(background, tuple) :
                raise TypeError(f'`background` must be a tuple (pd.DataFrame, bool).')
            data, with_labels = background
            if not isinstance(with_labels, bool) :
                warnings.warn(f'The second element of `background` should be a boolean, but got {type(with_labels)}. Unexpected calculations may occur.')
            if not isinstance(data, pd.DataFrame) :
                raise TypeError("The first element of `background` must be a pandas DataFrame, but got {}".format(type(data)))
            if data.shape[0] < 2 :
                warnings.warn("You are giving a single row as a background data, which will lead to biased Shapley values.")
            if data.shape[1] != self.M :
                raise ValueError("The number of columns in the background data must match the number of variables in the Bayesian network. Although values outside the Markov blanket, including the target, are unused, they are required for indexing purposes.")
            data = data.reindex(columns=self.feat_names).dropna(axis=0).to_numpy()
            if with_labels :
                data = self._labelToPos_df(data, self.vars_ids)

        self._N = len(data)
        if self._N == 0 :
            raise ValueError("Background data can't be empty.")
        
        self._data, self.counts = np.unique(data, axis=0, return_counts=True)
        self.func = self._log if log else self._identity        
        
        # For jointProbability
        self.inst = gum.Instantiation()
        for var in self.bn.ids(self.feat_names):
            self.inst.add(self.bn.variable(var))

    def _joint(self, row_values):
        self.inst.fromdict(row_values)
        return self.func(self.bn.jointProbability(self.inst))

    @abstractmethod
    def _shall_1dim(self, x, elements) :
        # Computes the Shall values for a single instance.
        # This method should be implemented in subclasses.
        raise NotImplementedError("This method should be implemented in subclasses.")
    
    @abstractmethod
    def _shall_ndim(self, x, elements) :
        # Computes the Shall values for multiple instances.
        # This method should be implemented in subclasses.
        raise NotImplementedError("This method should be implemented in subclasses.")

    def compute(self, data:tuple | None, N:int=100):
        if data is None :
            if not isinstance(N, int) :
                raise TypeError("Since df is None, N must be an integer, but got {}".format(type(N)))
            if N < 2 :
                raise ValueError("N must be greater than 1, but got {}".format(N))  
            y = gum.generateSample(self.bn, N, with_labels=False)[0].reindex(columns=self.feat_names).to_numpy()
            # Remove duplicate rows in generated data
            _, idx = np.unique(y, axis=0, return_index=True)
            y = y[idx, :]
            contributions = self._shall_ndim(y)
        else :
            if not isinstance(data, tuple) :
                raise TypeError(f'`data` must be a tuple (pd.DataFrame, bool).')
            df, with_labels = data
            if not isinstance(with_labels, bool) :
                warnings.warn(f'The second element of `data` should be a boolean, but got {type(with_labels)}. Unexpected calculations may occur.')
            dtype = object if with_labels else int

            if isinstance(df, pd.Series): # OK 
                # Here we are sure that df is a single instance (a Series).
                s = df.dropna()
                if np.setdiff1d(self.feat_names, s.index):
                    raise ValueError("For SHALL values, you must provide all variables used in the Bayesian Network; passing only a subset is not allowed.")

                x = s.reindex(self.feat_names).to_numpy()
                if x.size == 0:
                        raise ValueError("DataFrame is empty")

                y = self._labelToPos_row(x, self.vars_ids) if with_labels else x
                contributions = self._shall_1dim(y)

            elif isinstance(df, pd.DataFrame) :
                df_clean = df.dropna(axis=0)
                if np.setdiff1d(self.feat_names, df.columns):
                    raise ValueError("For SHALL values, you must provide all variables used in the Bayesian Network; passing only a subset is not allowed.")
                
                if len(df_clean) == 1 :
                    # Here we are sure that df is a single instance (a DataFrame with one row).
                    x = df_clean.reindex(columns=self.feat_names).dropna(axis=0).to_numpy()
                    if x.size == 0:
                        raise ValueError("DataFrame is empty")
                    y = self._labelToPos_df(x, self.vars_ids) if with_labels else x
                    contributions = self._shall_1dim(y)
            
                else :                
                    x = df_clean.reindex(columns=self.feat_names).dropna(axis=0).to_numpy()
                    if x.size == 0:
                        raise ValueError("DataFrame is empty")
                    y = self._labelToPos_df(x, self.vars_ids) if with_labels else x
                    _, idx = np.unique(y, axis=0, return_index=True)
                    y = y[idx, :]
                    contributions = self._shall_ndim(y)

            elif isinstance(df, dict) :    
                if np.setdiff1d(self.feat_names, df.keys()):
                    raise ValueError("For SHALL values, you must provide all variables used in the Bayesian Network; passing only a subset is not allowed.")
                               
                try :
                    N = len(list(df.values())[0])
                    if not isinstance(list(df.values())[0], (list, np.ndarray)) :
                        raise TypeError("Each value in the dictionary must be a list or a numpy array.") 
                    
                    x = np.empty((N, self.M), dtype=dtype)
                    for feat in df.keys() :
                        id = self.bn.idFromName(feat)
                        x[:, id] = df[feat]
                    mask = [all(x is not None and not (isinstance(x, float) and np.isnan(val)) for val in row) for row in x] # drop lines with None and np.nan
                    x = x[mask]
                    if x.size == 0:
                        raise ValueError("DataFrame is empty")
                    # Remove duplicate rows in x and unused columns.
                    y = self._labelToPos_df(x, self.vars_ids) if with_labels else x
                    _, idx = np.unique(y, axis=0, return_index=True)
                    y = y[idx, :]
                    contributions = self._shall_ndim(y)

                except TypeError :
                    # Here we are sure that df is a single instance (a dictionary with one row).
                    x = np.empty(self.M, dtype=dtype)
                    for feat in df.keys() :
                        if not(df[feat] is None) :
                            id = self.bn.idFromName(feat)
                            x[id] = df[feat]
                    if x.size == 0:
                        raise ValueError("DataFrame is empty")
                    y = self._labelToPos_row(x, self.vars_ids) if with_labels else x
                    contributions = self._shall_1dim(y)

            else :
                raise TypeError("The first element of `data` must be a pandas DataFrame, Series or a dictionary, but got {}".format(type(df)))

        if contributions.ndim == 1:
            values = {
                self.feat_names[i]: float(contributions[i])
                for i in self.vars_ids
            }
            importances = {
                self.feat_names[i]: abs(float(contributions[i]))
                for i in self.vars_ids

            }

            explanation = Explanation(values,
                                      importances,
                                      list(self.feat_names[self.vars_ids]),
                                      x[self.vars_ids],
                                      self.baseline,
                                      self.func.__name__,
                                      )
        else:
            values = {
                self.feat_names[i]: [float(v) for v in contributions[i, :]]
                for i in self.vars_ids

            }
            mean_abs = np.mean(np.abs(contributions), axis=1)
            importances = {
                self.feat_names[i]: abs(float(mean_abs[i]))
                for i in self.vars_ids

            }
            explanation = Explanation(values,
                                      importances,
                                      list(self.feat_names[self.vars_ids]),
                                      y[:, self.vars_ids],
                                      self.baseline,
                                      self.func.__name__,
                                      )
        return explanation
