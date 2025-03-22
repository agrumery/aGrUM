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