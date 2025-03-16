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

class SimplePlugIn:
    """
    Uses the (original) Frontdoor Adjustment Formula to derive
    the plug-in estimator. Does not account for covariates.
    Based on Guo et al. (2023).
    (see https://www.jstor.org/stable/2337329).
    """

    def __init__(
            self,
            learner: str | Any | None = None,
            propensity_learner: str | Any | None = None
        ) -> None:
        """
        Initialize the Frontdoor Adjustment estimator.

        Parameters
        ----------
        learner: str or object, optional
            Estimator for outcome variable.
            If not provided, defaults to LinearRegression.
        propensity_learner: str |or object, optional
            Estimator for treatment proability.
            If not provided, defaults to LogisticRegression.
        """

        if learner is None:
            self.learner = learnerFromString("LinearRegression")
        elif isinstance(learner, str):
            self.learner = learnerFromString(learner)
        else:
            self.learner = clone(learner)

        if propensity_learner is None:
            self.propensity_learner = learnerFromString("LogisticRegression")
        elif isinstance(propensity_learner, str):
            self.propensity_learner = learnerFromString(propensity_learner)
        else:
            self.propensity_learner = clone(propensity_learner)

        self.treatment_probability = None

    def fit(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
        ) -> None:
        """
        Fit the inference model.

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.
        """

        self.learner.fit(
            X=pd.concat(
                [pd.DataFrame(M), pd.DataFrame(treatment)],
                axis=1
            ),
            y=y
        )

        self.propensity_learner.fit(
            X=pd.DataFrame(M),
            y=treatment
        )

        self.treatment_probability = treatment.sum() / treatment.count()

    def predict(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
        )-> np.ndarray:
        """
        Predict the Idividual Causal Effect (ICE),
        also referd to as the Individual Treatment Effect (ITE).

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.

        Returns
        -------
        np.ndarray
            An array containing the predicted ICE.
        """

        M_control = pd.concat(
            [
                pd.DataFrame(M),
                pd.DataFrame(
                    {
                        self.learner.feature_names_in_[-1]: \
                            np.zeros(len(M))
                    },
                    index=pd.DataFrame(M).index
                )
            ], axis=1
        )

        M_treatment = pd.concat(
            [
                pd.DataFrame(M),
                pd.DataFrame(
                    {
                        self.learner.feature_names_in_[-1]: \
                            np.ones(len(M))
                    },
                    index=pd.DataFrame(M).index
                )
            ], axis=1
        )

        mu0 = self.learner.predict(X=M_control)
        mu1 = self.learner.predict(X=M_treatment)

        e = self.propensity_learner.predict_proba(X=M)[:,1]
        p = self.treatment_probability

        return (e/p - (1-e)/(1-p)) * (mu1*p + mu0*(1-p))

    def estimate_ate(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
            pretrain: bool = True
        ) -> float:
        """
        Predicts the Average Causal Effect (ACE),
        also refered to as the Average Treatment Effect (ATE).
        (The term ATE is used in the method name for compatibility purposes.)

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.

        Returns
        -------
        float
            The value of the ACE.
        """

        return self.predict(X, treatment, y, M).mean()

class GeneralizedPlugIn:
    """
    Basic implementation of the second plug-in TMLE estimator.
    Must provide covariates.
    Based on Guo et al. (2023).
    (see https://arxiv.org/abs/2312.10234).
    """

    def __init__(
            self,
            learner: str | Any | None = None,
            conditional_outcome_learner: str | Any | None = None,
            propensity_score_learner: str | Any | None = None,
            pseudo_control_outcome_learner: str | Any | None = None,
            pseudo_treatment_outcome_learner: str | Any | None = None
        ) -> None:
        """
        Initialize the Frontdoor Adjustment estimator.

        Parameters
        ----------
        learner: str or Any, optional
            Estimator for outcome variable.
            If not provided, defaults to LinearRegression.
        propensity_learner: str or Any, optional
            Estimator for treatment proability.
            If not provided, defaults to LogisticRegression.
        """

        if learner is None:
            self.conditional_outcome_learner = learnerFromString(
                "LinearRegression")
            self.pseudo_outcome_learner = learnerFromString(
                "LinearRegression")
            self.propensity_score_learner = learnerFromString(
                "LogisticRegression")
        elif isinstance(learner, str):
            self.conditional_outcome_learner = learnerFromString(learner)
            self.pseudo_outcome_learner = learnerFromString(learner)
            self.propensity_score_learner = learnerFromString(learner)
        else:
            self.conditional_outcome_learner = clone(learner)
            self.pseudo_outcome_learner = clone(learner)
            self.propensity_score_learner = clone(learner)

        if conditional_outcome_learner is None:
            self.conditional_outcome_learner = learnerFromString(
                "LinearRegression")
        elif isinstance(conditional_outcome_learner, str):
            self.conditional_outcome_learner = learnerFromString(
                conditional_outcome_learner)
        else:
            self.conditional_outcome_learner = clone(
                conditional_outcome_learner)

        if pseudo_control_outcome_learner is None:
            self.pseudo_control_outcome_learner = learnerFromString(
                "LinearRegression")
        elif isinstance(pseudo_control_outcome_learner, str):
            self.pseudo_control_outcome_learner = learnerFromString(
                pseudo_control_outcome_learner)
        else:
            self.pseudo_control_outcome_learner = clone(
                pseudo_control_outcome_learner)

        if pseudo_treatment_outcome_learner is None:
            self.pseudo_treatment_outcome_learner = learnerFromString(
                "LinearRegression")
        elif isinstance(pseudo_treatment_outcome_learner, str):
            self.pseudo_treatment_outcome_learner = learnerFromString(
                pseudo_treatment_outcome_learner)
        else:
            self.pseudo_treatment_outcome_learner = clone(
                pseudo_treatment_outcome_learner)

        if propensity_score_learner is None:
            self.propensity_score_learner = learnerFromString(
                "LogisticRegression")
        elif isinstance(propensity_score_learner, str):
            self.propensity_score_learner = learnerFromString(
                propensity_score_learner)
        else:
            self.propensity_score_learner = clone(
                propensity_score_learner)

    def fit(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
        ) -> None:
        """
        Fit the inference model.

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.
        """

        self.conditional_outcome_learner.fit(
            X=pd.concat(
                [pd.DataFrame(M), pd.DataFrame(X), pd.DataFrame(treatment)],
                axis=1
            ),
            y=y
        )

        self.propensity_score_learner.fit(
            X=pd.DataFrame(X),
            y=treatment
        )

    def predict(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
        )-> np.ndarray:
        """
        Predict the Idividual Causal Effect (ICE),
        also referd to as the Individual Treatment Effect (ITE).

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.

        Returns
        -------
        np.ndarray
            An array containing the predicted ICE.
        """

        def xi(m, x):

            mu = self.conditional_outcome_learner.predict
            pi = self.propensity_score_learner.predict_proba

            MX_control = pd.concat(
                [
                    pd.DataFrame(m),
                    pd.DataFrame(x),
                    pd.DataFrame(
                        {self.conditional_outcome_learner.feature_names_in_[-1]:
                        np.zeros(len(m))},
                        index=pd.DataFrame(m).index
                    )
                ], axis=1
            )

            MX_treatment = pd.concat(
                [
                    pd.DataFrame(m),
                    pd.DataFrame(x),
                    pd.DataFrame(
                        {self.conditional_outcome_learner.feature_names_in_[-1]:
                        np.ones(len(m))},
                        index=pd.DataFrame(m).index
                    )
                ], axis=1
            )
            return mu(MX_control)*pi(x)[:,0] + mu(MX_treatment)*pi(x)[:,1]


        MX_control_empirical = pd.concat(
            [pd.DataFrame(M), pd.DataFrame(X)],
            axis=1
        )

        self.pseudo_control_outcome_learner.fit(
            X=X[treatment == 0],
            y=xi(M[treatment == 0], X[treatment == 0])
        )

        self.pseudo_treatment_outcome_learner.fit(
            X=X[treatment == 1],
            y=xi(M[treatment == 1], X[treatment == 1])
        )

        gamma0 = self.pseudo_control_outcome_learner.predict(X)
        gamma1 = self.pseudo_treatment_outcome_learner.predict(X)

        return gamma1 - gamma0

    def estimate_ate(
            self,
            X: np.matrix | np.ndarray | pd.DataFrame,
            treatment: np.ndarray | pd.Series,
            y: np.ndarray | pd.Series,
            M: np.matrix | np.ndarray | pd.DataFrame,
            pretrain: bool = True
        ) -> float:
        """
        Predicts the Average Causal Effect (ACE),
        also refered to as the Average Treatment Effect (ATE).
        (The term ATE is used in the method name for compatibility purposes.)

        Parameters
        ----------
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series
            The treatment assignment vector.
        y: np.ndarray or pd.Series,
            The outcome vector.
        M: np.matrix or np.ndarray or pd.DataFrame
            The mediator matrix.

        Returns
        -------
        float
            The value of the ACE.
        """

        return self.predict(X, treatment, y, M).mean()