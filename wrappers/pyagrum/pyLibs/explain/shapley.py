# ShapValues
from pyagrum.explain import *
# Calculations
import pyagrum as gum
# Plots
import matplotlib.pyplot as plt
from pyagrum.explain.notebook import *
# GL
import warnings

class ShapValues :
    """
    Class to compute Shapley values for a target variable in a Bayesian network.
    """

    def __init__(self, bn, target, logit=True):
        """
        Parameters:
        ------
        bn : pyagrum.BayesNet
            The Bayesian Network.
        target : int | str
            The node id (or node name) of the target.
        background : Tuple(pandas.DataFrame, bool) | None
            A tuple containing a pandas DataFrame and a boolean indicating whether the DataFrame contains labels or positions.
        sample_size : int
            The size of the background sample to generate if `background` is None.
        logit : bool
            If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        TypeError : If bn is not a gum.BayesNet instance or target is not an integer or string.
        ValueError : If target is not a valid node id in the Bayesian Network.
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

    @staticmethod
    def _plot(explanation: Explanation, y: int, plot: bool, plot_importance: bool) :
        ndim = explanation.data.ndim
        n_figures = sum([plot, plot_importance])
        plot_index = 0
        # Local Explanation
        if n_figures > 0 :
            _, axs = plt.subplots(1, n_figures, figsize=(n_figures*6, 5))
            if plot :
                if ndim == 1 :
                    waterfall(explanation=explanation, y=y, ax=axs[plot_index])
                else :
                    beeswarm(explanation=explanation, y=y, ax=axs[plot_index])
                plot_index += 1
            if plot_importance :
                bar(explanation=explanation, y=y, ax=axs[plot_index])

            plt.tight_layout()
            plt.show()

    def conditional(self,
                    df,
                    y:int=1,
                    plot:bool=False,
                    plot_importance:bool=False,
                    percentage:bool=False) :
        """
        Computes the conditional Shapley values for each variable.

        Parameters:
        ----------
        df : pandas DataFrame
            The input data for which to compute the Shapley values.
        y : int, optional
            The target class for which to compute the Shapley values (default is 1).
        plot : bool, optional
            If True, plots the waterfall or beeswarm plot depending on the number of rows in df (default is False).
        plot_importance : bool, optional
            If True, plots the bar chart of feature importance (default is False).

        Returns:
        -------
        Dict[str, float]
            A dictionary containing the importances of each variable in the input data.
        """
        explainer = ConditionalShapValues(self.bn, self.target, self.logit)
        explanation = explainer.compute((df, True))
        self._plot(explanation, y, plot, plot_importance)
        return explanation.importances[y]


    def marginal(self,
                 df,
                 y=1,
                 sample_size=200,
                 plot=False,
                 plot_importance=False
                ) :
        """
        Computes the marginal Shapley values for each variable.

        Parameters:
        ----------
        df : pandas DataFrame
            The input data for which to compute the Shapley values.
        y : int, optional
            The target class for which to compute the Shapley values (default is 1).
        sample_size : int, optional
            The number of samples to use for the background data (default is 200).
        plot : bool, optional
            If True, plots the waterfall or beeswarm plot depending on the number of rows in df (default is False).
        plot_importance : bool, optional
            If True, plots the bar chart of feature importance (default is False).

        Returns:
        -------
        Dict[str, float]
            A dictionary containing the importances of each variable in the input data.
        """
        explainer = MarginalShapValues(self.bn, self.target, None, sample_size, self.logit)
        explanation = explainer.compute((df, True))
        self._plot(explanation, y, plot, plot_importance)
        return explanation.importances[y]

    def causal(self,
                 df,
                 y=1,
                 sample_size=200,
                 plot=False,
                 plot_importance=False
                ) :
        """
        Computes the causal Shapley values for each variable.

        Parameters:
        ----------
        df : pandas DataFrame
            The input data for which to compute the Shapley values.
        y : int, optional
            The target class for which to compute the Shapley values (default is 1).
        sample_size : int, optional
            The number of samples to use for the background data (default is 200).
        plot : bool, optional
            If True, plots the waterfall or beeswarm plot depending on the number of rows in df (default is False).
        plot_importance : bool, optional
            If True, plots the bar chart of feature importance (default is False).

        Returns:
        -------
        Dict[str, float]
            A dictionary containing the importances of each variable in the input data.
        """
        explainer = CausalShapValues(self.bn, self.target, None, sample_size, self.logit)
        explanation = explainer.compute((df, True))
        self._plot(explanation, y, plot, plot_importance)
        return explanation.importances[y]
