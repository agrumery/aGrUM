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

import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causal as csl

class CausalBNEstimator:
    """
    A Causal Bayesian Network Estimator.

    This class utilizes do-calculus identification and lazy propagation
    inference, implemented via the pyAgrum library's causal module,
    to determine the causal effect within Bayesian Networks.

    Note: In the case of instrumental variables, the causal effect is
    estimated using heuristic methods, as this adjustment is not
    identifiable through do-calculus.
    """

    def _useCausalStructure(
            self,
            cm_clone: csl.CausalModel,
            causal_model: csl.CausalModel
        ) -> None:
        """
        Use the causal structure given by `cm_clone` on `causal_model`.

        Parameters
        ----------
        cm_clone: csl.CausalStructure
            The model recieving the causal structure.
        causal_model: csl.CausalStructure
            The model giving the causal structure.
        """

        for id in causal_model.latentVariablesIds():
            childrens = causal_model.causalBN().children(id)
            childrens = {
                causal_model.causalBN().variable(c).name() for c in childrens
            }
            cm_clone.addLatentVariable(
                causal_model.causalBN().variable(id).name(), tuple(childrens)
            )
        for x, y in causal_model.arcs():
            if not cm_clone.existsArc(x, y):
                cm_clone.addCausalArc(x, y)

    def __init__(
            self,
            causal_model: csl.CausalModel,
            treatment: str,
            outcome: str,
            instrument: str | None = None
        ) -> None:
        """
        Initialize an Causal Model estimator.

        Parameters
        ----------
        causal_model: csl.CausalModel
            The causal graph.
        treatment: str
            The treatment variable.
        outcome: str
            The outcome variable.
        instrument: str, optional
            The instrumental variable
        """

        if isinstance(causal_model, csl.CausalModel):
            self.causal_model = causal_model.clone()
            self._useCausalStructure(
                self.causal_model,
                causal_model
            )
        else:
            raise ValueError(
                "`causal_model` must be instance of "\
                "`pyagrum.causal.CausalModel`."
            )

        self.treatment = treatment
        self.outcome = outcome
        self.instrument = instrument

    def fit(
            self,
            df: pd.DataFrame,
            smoothing_prior: float = 1e-9
        ) -> None:
        """
        Fit the inference model.

        Parameters
        ----------
        df: pd.DataFrame
            The observations.
        smoothing_prior (Optional): float
            The uniform prior distribution. Default is 1e-9.
        """

        parameter_learner = gum.BNLearner(df,
            self.causal_model.observationalBN())
        parameter_learner.useNMLCorrection()
        parameter_learner.useSmoothingPrior(smoothing_prior)

        bn = gum.BayesNet(self.causal_model.observationalBN())

        parameter_learner.fitParameters(bn)

        causal_model = csl.CausalModel(bn)
        self._useCausalStructure(causal_model, self.causal_model)
        self.causal_model = causal_model

        return self.causal_model

    def _getIntervalIndex(self, x: float, var: str) -> int:
        """
        Gets the domain index of the variable.

        Parameters
        ---------
        x: float
            The conditional.
        var: str
            The variable label string.

        Returns
        -------
        int
            The index of the conditional in the variable domain.
        """

        splits = list()
        accumulator = ""
        for letter in \
            self.causal_model.causalBN().variable(var).domain():
            if letter in ["-", ".", "0", "1", "2", "3", \
                          "4", "5", "6", "7", "8", "9"]:
                accumulator += letter
            elif len(accumulator) > 0:
                split = float(accumulator)
                if len(splits) > 0 and splits[-1] == split:
                    splits.pop()
                splits.append(split)
                accumulator = ""

        for i in range(len(splits)):
            if x < splits[i]:
                return i

        return len(splits)-1

    def _predictRow(
            self,
            X: pd.Series
        )-> float:
        """
        Predict the Individual Causal Effect (ICE) of a single row.

        Parameters
        ----------
        X: pd.Series
            The of covariates.

        Returns
        -------
        float
            The predicted ICE.
        """

        keys = X.index.to_list()
        values = list()

        for covar in X.index:
            values.append(self._getIntervalIndex(X[covar], covar))

        if self.instrument is not None :
            ie = gum.LazyPropagation(self.causal_model.observationalBN())
            ie.setEvidence(dict(zip(keys+[self.instrument], values+[0])))
            cptY0 = ie.posterior(self.outcome)
            cptT0 = ie.posterior(self.treatment)
            ie.setEvidence(dict(zip(keys+[self.instrument], values+[1])))
            cptY1 = ie.posterior(self.outcome)
            cptT1 = ie.posterior(self.treatment)

            diffY = cptY1 - cptY0
            diffT = cptT1 - cptT0

            return diffY.expectedValue(
                lambda d: diffY.variable(0).numerical(
                    d[diffY.variable(0).name()]
                )
            ) / diffT.expectedValue(
                lambda d: diffT.variable(0).numerical(
                    d[diffT.variable(0).name()]
                )
            )

        else:
            _, cpt0, _ = csl.causalImpact(
                cm=self.causal_model,
                on=self.outcome,
                doing=self.treatment,
                knowing=set(X.index),
                values=dict(zip(keys+[self.treatment], values+[0]))
            )

            _, cpt1, _ = csl.causalImpact(
                cm=self.causal_model,
                on=self.outcome,
                doing=self.treatment,
                knowing=set(X.index),
                values=dict(zip(keys+[self.treatment], values+[1]))
            )

            if cpt0 is None:
                raise csl._exceptions.HedgeException(
                    "Causal effect is unidentifiable using do-calculus."
                )

            diff = cpt1 - cpt0
            return diff.expectedValue(
                lambda d: diff.variable(0).numerical(
                    d[diff.variable(0).name()]
                )
            )

    def predict(
            self,
            w: np.matrix | np.ndarray | pd.DataFrame = None,
            X: np.matrix | np.ndarray | pd.DataFrame = None,
            M: np.matrix | np.ndarray | pd.DataFrame = None,
            treatment: np.ndarray | pd.Series | None = None,
            y: np.ndarray | pd.Series | None = None,
        )-> np.ndarray:
        """
        Predict the Individual Causal Effect (ICE),
        also referd to as the Individual Treatment Effect (ITE).

        Parameters
        ----------
        w: np.matrix or np.ndarray or pd.DataFrame
            The instrument variable.
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series or None, optional
            The vector of treatment assignments.
        y: np.ndarray or pd.Series, optional
            The vector of outcomes.

        Returns
        -------
        np.ndarray
            An array containing the predicted ICE.
        """

        if X is not None:
            return X.apply(self._predictRow, axis=1).to_numpy()
        else:
            return M.apply(self._predictRow, axis=1).to_numpy()


    def estimate_ate(
            self,
            w: np.matrix | np.ndarray | pd.DataFrame = None,
            X: np.matrix | np.ndarray | pd.DataFrame = None,
            M: np.matrix | np.ndarray | pd.DataFrame = None,
            treatment: np.ndarray | pd.Series | None = None,
            y: np.ndarray | pd.Series | None = None,
            pretrain: bool = True
        ) -> float:
        """
        Predicts the Average Causal Effect (ACE),
        also refered to as the Average Treatment Effect (ATE).
        (The term ATE is used in the method name for compatibility purposes.)

        Parameters
        ----------
        w: np.matrix or np.ndarray or pd.DataFrame
            The instrument variable.
        X: np.matrix or np.ndarray or pd.DataFrame
            The matrix of covariates.
        treatment: np.ndarray or pd.Series or None, optional
            The vector of treatment assignments.
        y: np.ndarray or pd.Series, optional
            The vector of outcomes.

        Returns
        -------
        float
            The value of the ACE.
        """

        if self.instrument is not None :
            ie = gum.LazyPropagation(self.causal_model.observationalBN())
            ie.setEvidence({self.instrument:0})
            cptY0 = ie.posterior(self.outcome)
            cptT0 = ie.posterior(self.treatment)
            ie.setEvidence({self.instrument: 1})
            cptY1 = ie.posterior(self.outcome)
            cptT1 = ie.posterior(self.treatment)

            diffY = cptY1 - cptY0
            diffT = cptT1 - cptT0

            return diffY.expectedValue(
                lambda d: diffY.variable(0).numerical(
                    d[diffY.variable(0).name()]
                )
            ) / diffT.expectedValue(
                lambda d: diffT.variable(0).numerical(
                    d[diffT.variable(0).name()]
                )
            )

        else:
            _, cpt0, _ = csl.causalImpact(
                self.causal_model,
                on=self.outcome,
                doing=self.treatment,
                values={self.treatment:0}
            )

            _, cpt1, _ = csl.causalImpact(
                self.causal_model,
                on=self.outcome,
                doing=self.treatment,
                values={self.treatment:1}
            )

            if cpt0 is None:
                raise csl._exceptions.HedgeException(
                    "Causal effect is unidentifiable using do-calculus."
                )

            difference = cpt1 - cpt0
            return difference.expectedValue(
                lambda d: difference.variable(0).numerical(
                    d[difference.variable(0).name()]
                )
            )
