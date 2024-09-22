# (c) Copyright 2020-2024 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

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

import pandas as pd
import numpy as np

from typing import Any

from sklearn.base import clone

from ._learners import learnerFromString

class DM:
    """
    A simple Differnce in Means (DM) estimator which computes the
    Average Causal Effect (ACE) under the ignorability assumption,
    in Randomized Controlled Trials (RCT).
    """

    def __init__(self) -> None:
        """
        Initialize an DM estimator.
        """

    def fit(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
        ) -> None:
        """
        Fit the inference model.

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The covariate matrix.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        """

    def predict(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series = None,
            y: np.ndarray | pd.Series = None,
        )-> np.ndarray:
        """
        Predict the Average Causal Effect (ACE).

        Note: This is a placeholder function for the API.

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The covariate matrix.
        treatment: np.ndarray or pd.Series, optional
            The vector of treatment assignments.
        y: np.ndarray or pd.Series, optional
            The vector of outcomes.

        Returns
        -------
        np.ndarray
            An array containing the predicted ACE.
        """

        return [y[treatment == 1].mean() - y[treatment == 0].mean()] * len(y)

    def estimate_ate(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame = None,
            treatment: np.ndarray | pd.Series = None,
            y: np.ndarray | pd.Series = None,
            w: np.matrix | np.ndarray | pd.DataFrame = None,
            pretrain: bool = True
        ) -> float:
        """
        Predicts the Average Causal Effect (ACE).
        (The term "Average Treatment Effect" (ATE) is used in
        the method name for compatibility purposes.)

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The covariate matrix.
        treatment: np.ndarray or pd.Series, optional
            The vector of treatment assignments.
        y: np.ndarray or pd.Series, optional
            The vector of outcomes.

        Returns
        -------
        float
            The value of the ACE.
        """

        return y[treatment == 1].mean() - y[treatment == 0].mean()