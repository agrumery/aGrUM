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

import pyAgrum.causal as csl

from ._utils import (
    MisspecifiedAdjustmentError,
    EmptyConditionError,
    InvalidConditionError,
    RCTError,
    BackdoorError,
    FrontdoorError,
    IVError,
    RCT,
    generalizedFrontDoor,
    instrumentalVariable,
    RCT_ESTIMATORS_LIST,
    BACKDOOR_ESTIMATORS_LIST,
    FRONTDOOR_ESTIMATORS_LIST,
    IV_ESTIMATORS_LIST
)

from ._causalBNEstimator import CausalBNEstimator

from ._RCTEstimators import (
    DM
)

from ._backdoorEstimators import (
    SLearner,
    TLearner,
    XLearner,
    PStratification,
    IPW
)
from ._frontdoorEstimators import (
    SimplePlugIn,
    GeneralizedPlugIn
)
from ._IVEstimators import (
    Wald,
    WaldIPW,
    NormalizedWaldIPW,
    TSLS
)

class CausalEffectEstimation:
    """
    Estimates causal effects using a dataset and a causal graph within
    the Neyman-Rubin Potential Outcomes framework.

    This class performs causal identification based on user-specified datasets
    and causal graphical models. It determines the appropriate adjustment method
    — suchas backdoor, front-door, or instrumental variables (IV) — to optimally
    estimate the causal effect (or treatment effect) between the intervention
    (treatment assignment) and the outcome.

    The class integrates domain-specific statistical estimators and recent
    advancements in machine learning techniques to estimate various causal
    effects, including the Average Causal Effect (ACE), Conditional Average
    Causal Effect (CACE), Individual Causal Effect (ICE), and Local Average
    Treatment Effect (LATE), among others.

    This module is inspired by the works of
    :cite:t:`wager2020stats` and :cite:t:`neal2020introduction`.

    Raises
    ------
    AssertionError
        If the input dataframe is empty, indicating that predictions
        cannot be made.
    ValueError
        If the provided estimator_string does not correspond to any
        supported estimator.
    """

    _RCT = "Randomized Controlled Trial"
    _BACKDOOR = "Backdoor"
    _FRONTDOOR = "Generalized Frontdoor"
    _IV = "Generalized Instrumental Variable"
    _UNKNOWN = "Unknown"

    def __init__(self, df: pd.DataFrame, causal_model: csl.CausalModel) -> None:
        """
        Initializes the causal estimator instance.

        Parameters
        ----------
        df: pd.DataFrame
            The dataset for causal effect estimation.
        causal_model: csl.CausalModel
            The causal model for causal effect identification.
        """

        self._df = df
        self._causal_model = causal_model
        self._adjustment = None
        self._T = None # str: Intervention (Treatement) variable
        self._y = None # str: Outcome variable
        self._w = None # str: Instrumental variable
        self._M = None # set[str]: Mediator variable set
        self._X = None # set[str]: Confounder/Covariate variable set
        self._estimator = None # Any: Causal estimator

    def __str__(self) -> str:
        """
        Return a string representation of the instance.

        Includes class/module info, DataFrame summary, and details about
        the causal model, adjustment variables, intervention, outcome,
        confounders, mediators, instrumental variables, and estimator.

        Returns
        -------
        str
            A formatted string describing the instance's attributes.
        """

        object_info = f"<{self.__class__.__module__}."\
            f"{self.__class__.__name__}"\
            f" object at {hex(id(self))}>"

        df_info = f"<{self._df.__class__.__module__}."\
            f"{self._df.__class__.__name__}"\
            f" object at {hex(id(self._df))}>"\
            f"\n\t- shape\t\t: {self._df.shape}"\
            f"\n\t- columns\t: {self._df.columns}"\
            f"\n\t- memory usage\t: {self._df.memory_usage().sum()/1e6} MB"

        causal_model_info = f"{self._causal_model}"\
            f"\n\t- names\t\t: {self._causal_model.names()}"\
            f"\n\t- causal BN\t: {self._causal_model.causalBN()}"\
            f"\n\t- observ. BN\t: {self._causal_model.observationalBN()}"

        res = f"{object_info}\n"\
            f"\n Dataframe\t: {df_info}"\
            f"\n Causal Model\t: {causal_model_info}"

        if self._adjustment is not None:
            res += f"\n Adjustment\t: {self._adjustment}"
        if self._T is not None:
            res += f"\n Intervention\t: {self._T}"
        if self._y is not None:
            res += f"\n Outcome\t: {self._y}"
        if self._X is not None:
            res += f"\n Confounders\t: {self._X}"
        if self._M is not None:
            res += f"\n Mediators\t: {self._M}"
        if self._w is not None:
            res += f"\n Instrument\t: {self._w}"
        if self._estimator is not None:
            res += f"\n Estimator\t: {self._estimator}"

        return res

    # Causal identification

    def useRCTAdjustment(
            self,
            intervention: str,
            outcome: str,
            confounders: set[str]
        ) -> None:
        """
        Specify the Randomized Controlled Trial (RCT) Adjustment.

        Note: This method does not verify if the specified adjustment is
        appropriate within the causal graph. If unsure, use
        `.identifyAdjustment()` to automatically determine the correct
        adjustment set.

        Parameters
        ----------
        intervention: str
            Intervention (or treatment) variable.
        outcome: str
            Outcome variable.
        confounders: set[str] or None
            Set of confounder variables (or covariates).
        """

        self._adjustment = self._RCT
        self._T = intervention
        self._y = outcome
        self._X = set() if confounders is None else confounders
        self._w = None
        self._M = None
        self._estimator = None

    def useBackdoorAdjustment(
            self,
            intervention: str,
            outcome: str,
            confounders: set[str]
        ) -> None:
        """
        Specify the Backdoor Adjustment.

        Note: This method does not verify if the specified adjustment is
        appropriate within the causal graph. If unsure, use
        `.identifyAdjustment()` to automatically determine the correct
        adjustment set.

        Parameters
        ----------
        intervention: str
            Intervention (or treatment) variable.
        outcome: str
            Outcome variable.
        confounders: set[str] or None
            Set of confounder variables (or covariates).
        """

        self._adjustment = self._BACKDOOR
        self._T = intervention
        self._y = outcome
        self._X = set() if confounders is None else confounders
        self._w = None
        self._M = None
        self._estimator = None

    def useFrontdoorAdjustment(
            self,
            intervention: str,
            outcome: str,
            mediators: set[str],
            confounders: set[str] | None = None
        ) -> None:
        """
        Specify the (General) Frontdoor Adjustment.
        :cite:t:`guo2023targeted`.

        Note: This method does not verify if the specified adjustment is
        appropriate within the causal graph. If unsure, use
        `.identifyAdjustment()` to automatically determine the correct
        adjustment set.

        Parameters
        ----------
        intervention: str
            Intervention (or treatment) variable.
        outcome: str
            Outcome variable.
        mediators: set[str]
            Mediator variables.
        confounders: set[str] or None, optional
            Set of confounder variables (or covariates).
        """

        self._adjustment = self._FRONTDOOR
        self._T = intervention
        self._y = outcome
        self._M = mediators
        self._X = set() if confounders is None else confounders
        self._w = None
        self._estimator = None

    def useIVAdjustment(
            self,
            intervention: str,
            outcome: str,
            instrument: str,
            confounders: set[str] | None = None
        ) -> None:
        """
        Specify the (Generalized) Instrumental Variable Adjustment.
        :cite:t:`brito2012generalized`,
        :cite:t:`van2015efficiently`.


        Note: This method does not verify if the specified adjustment is
        appropriate within the causal graph. If unsure, use
        `.identifyAdjustment()` to automatically determine the correct
        adjustment set.

        Parameters
        ----------
        intervention: str
            Intervention (or treatment) variable.
        outcome: str
            Outcome variable.
        instruments: str
            Instrumental variable.
        confounders: set[str] or None, optional
            Set of confounder variables (or covariates).
        """

        self._adjustment = self._IV
        self._T = intervention
        self._y = outcome
        self._w = instrument
        self._X = set() if confounders is None else confounders
        self._estimator = None

    def useUnknownAdjustment(
            self,
            intervention: str,
            outcome: str,
        ) -> None:
        """
        Specify an Unknown Adjustment.

        Note: This method does not verify if the specified adjustment is
        appropriate within the causal graph. If unsure, use
        `.identifyAdjustment()` to automatically determine the correct
        adjustment set.

        Parameters
        ----------
        intervention: str
            Intervention (or treatment) variable.
        outcome: str
            Outcome variable.
        """

        self._adjustment = self._UNKNOWN
        self._T = intervention
        self._y = outcome

    def identifyAdjustmentSet(
            self,
            intervention: str,
            outcome: str,
            verbose: bool = True
        ) -> None:
        """
        Identify the sufficent adjustment set of covariates.

        Parameters
        ----------
        intervention: str
            Intervention (treatment) variable.
        outcome: str
            Outcome variable.
        verbose: bool
            If True, prints the estimators that can be used using
            the found adjustment. Default is True.

        Raises
        ------
        ValueError
            The tratment isn't binary or no adjustment set was found.
        """

        if set(self._df[intervention].unique()) != {0,1}:
            raise ValueError(
                "Treatment must be binary with values 0 and 1.\n"\
                "Please make sure that the datatype is `int` with "\
                "the positivity assumption satisfied "
                "(i.e. there is at least one occurence of both 0 and 1)."
            )

        rct = RCT(
            self._causal_model, intervention, outcome)
        backdoor = self._causal_model.backDoor(
            intervention, outcome)
        frontdoor, fd_covariates = generalizedFrontDoor(
            self._causal_model, intervention, outcome)
        instrumental_variable, iv_covariates = instrumentalVariable(
            self._causal_model, intervention, outcome)

        suggestion_text = ""

        if rct is not None:
            self.useRCTAdjustment(
                intervention,
                outcome,
                rct
            )
            suggestion_text += self._RCT \
            + " adjustment found. \n\n"\
            + "Supported estimators include:"\
            + RCT_ESTIMATORS_LIST \
            + "\nIf the outcome variable is a cause of other covariates "\
            "in the causal graph,\nBackdoor estimators may also be used."

        elif backdoor is not None:
            self.useBackdoorAdjustment(
                intervention,
                outcome,
                backdoor
            )
            suggestion_text += self._BACKDOOR \
            + " adjustment found. \n\n"\
            + "Supported estimators include:"\
            + BACKDOOR_ESTIMATORS_LIST

        elif frontdoor is not None:
            self.useFrontdoorAdjustment(
                intervention,
                outcome,
                frontdoor,
                fd_covariates
            )
            suggestion_text += self._FRONTDOOR \
            +" adjustment found. \n\n"\
            + "Supported estimators include:"\
            + FRONTDOOR_ESTIMATORS_LIST

        elif instrumental_variable is not None:
            self.useIVAdjustment(
                intervention,
                outcome,
                instrumental_variable,
                iv_covariates
            )
            suggestion_text += self._IV \
            + " adjustment found. \n\n"\
            + "Supported estimators include:"\
            + IV_ESTIMATORS_LIST

        else:
            self.useUnknownAdjustment(
                intervention,
                outcome
            )
            suggestion_text = "No adjustment set found among: "\
                "RCT, Backdoor, Generalized Frontdoor, or Generalized IV.\n\n"\
                "The only supported estimator without a known adjustment is "\
                "the Causal Bayesian Network Estimator, which can estimate "\
                "the causal effect if identifiable using do-Calculus.\n"\
                "Use `.fitCausalBNEstimator()` to apply this estimator."

        if verbose:
            print(suggestion_text)

        return self._adjustment

    # Model fitting

    def _fitEstimator(self, **fit_params) -> None:
        """
        Fits the specified causal effect estimator to the data.

        Parameters
        ----------
        estimator: str or Any
            The estimator to be used. Can be a string identifier for built-in
            estimators or a causalML object.

        estimator_params: dict[str, Any], optional
            Parameters to initialize the estimator. Keys are parameter names,
            values are the corresponding parameter values. Default is None.

        fit_params: dict[str, Any], optional
            Additional parameters passed to the fit method of the estimator.
            Keys are parameter names, values are the corresponding parameter
            values. Default is None.

        Raises
        ------
        ValueError
            No adjustment have been selected before fitting an estimator.
        """

        match self._adjustment:

            case self._IV:
                try:
                    return self._estimator.fit(
                        X = self._df[[*self._X]],
                        treatment = self._df[self._T],
                        y = self._df[self._y],
                        w = self._df[[*self._w]],
                        **fit_params
                    )
                except TypeError:
                    return self._estimator.fit(
                        X = self._df[[*self._X]],
                        treatment = self._df[self._T],
                        y = self._df[self._y],
                        assignment = self._df[[*self._w]],
                        **fit_params
                    )

            case self._FRONTDOOR:
                return self._estimator.fit(
                    X = self._df[[*self._X]],
                    treatment = self._df[self._T],
                    y = self._df[self._y],
                    M = self._df[[*self._M]],
                    **fit_params
                )

            case self._BACKDOOR:
                return self._estimator.fit(
                    X = self._df[[*self._X]],
                    treatment = self._df[self._T],
                    y = self._df[self._y],
                    **fit_params
                )

            case self._RCT:
                return self._estimator.fit(
                    X = self._df[[*self._X]],
                    treatment = self._df[self._T],
                    y = self._df[self._y],
                    **fit_params
                )

            case _:
                raise MisspecifiedAdjustmentError("fitting an estimator")

    # Custom estimators

    def fitCausalBNEstimator(self) -> Any:
        """
        Fit the Causal Bayesian Network Estimator.

        This class utilizes do-calculus identification and lazy propagation
        inference, implemented via the pyAgrum library's causal module,
        to determine the causal effect within Bayesian Networks.

        Note: In the case of instrumental variables, the causal effect is
        estimated using heuristic methods, as this adjustment is not
        identifiable through do-calculus.
        """

        self._estimator = CausalBNEstimator(
            self._causal_model,
            self._T,
            self._y,
            self._w
        )

        self._estimator.fit(self._df)

    def fitCustomEstimator(self, estimator: Any) -> Any:
        """
        Fits the specified `estimator` object, which must implement
        `.fit()`, `.predict()`, and `.estimate_ate()` methods consistent
        with CausalML estimators.
        :cite:t:`chen2020causalml`.

        Note: Compatibility with the current adjustment is not guarenteed.

        Parameters
        ----------
        estimator: Any
            The estimator object to be fitted, adhering to the CausalML
            method declarations.
        """

        self._estimator = estimator
        self._fitEstimator()

    # RCT

    def fitDM(self) -> Any:
        """
        Fits the Difference in Means (DM) Estimator.

        The DM estimator computes the Average Causal Effect (ACE)
        under the ignorability assumption in Randomized
        Controlled Trials (RCT) by taking the difference of the mean values
        among the treated and untreated population.
        """

        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("DM")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("DM")
        if self._adjustment is self._IV:
            raise IVError("DM")

        self._estimator = DM()
        self._fitEstimator()

    # Backdoor

    def fitSLearner(self, **estimator_params) -> Any:
        """
        Fit the S-Learner Estimator.

        A basic implementation of the S-learner based on Kunzel et al. (2018)
        :cite:t:`kunzel2019metalearners`.

        Parameters
        ----------
        learner: str or Any, optional
            Base estimator for all learners.
            If not provided, defaults to LinearRegression.
        """

        if self._adjustment is self._IV:
            raise IVError("SLearner")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("SLearner")
        if self._X is None or len(self._X) == 0:
            raise RCTError("SLearner")

        self._estimator = SLearner(**estimator_params)
        self._fitEstimator()

    def fitTLearner(self, **estimator_params) -> Any:
        """
        Fit the T-Learner Estimator.

        A basic implementation of the T-learner based on Kunzel et al. (2018)
        :cite:t:`kunzel2019metalearners`.

        Parameters
        ----------
        learner: str or Any, optional
            Base estimator for all learners.
            If not provided, defaults to LinearRegression.
        control_learner: str or Any, optional
            Estimator for control group outcome.
            Overrides `learner` if specified.
        treatment_learner: str or Any, optional
            Estimator for treatment group outcome.
            Overrides `learner` if specified.
        """

        if self._adjustment is self._IV:
            raise IVError("TLearner")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("TLearner")
        if self._X is None or len(self._X) == 0:
            raise RCTError("TLearner")

        self._estimator = TLearner(**estimator_params)
        self._fitEstimator()

    def fitXLearner(self, **estimator_params) -> Any:
        """
        Fit the X-Learner Estimator.

        A basic implementation of the X-learner based on Kunzel et al. (2018)
        :cite:t:`kunzel2019metalearners`.

        Parameters
        ----------
        learner: str or Any, optional
            Base estimator for all learners.
            If not provided, defaults to LinearRegression.
        control_outcome_learner: str or Any, optional
            Estimator for control group outcome.
            Overrides `learner` if specified.
        treatment_outcome_learner: str or Any, optional
            Estimator for treatment group outcome.
            Overrides `learner` if specified.
        control_effect_learner: str or Any, optional
            Estimator for control group effect.
            Overrides `learner` if specified.
        treatment_effect_learner: str or Any, optional
            Estimator for treatment group effect.
            Overrides `learner` if specified.
        propensity_score_learner: str or Any, optional
            Estimator for propensity score.
            If not provided, defaults to LogisticRegression.
        """

        if self._adjustment is self._IV:
            raise IVError("XLearner")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("XLearner")
        if self._X is None or len(self._X) == 0:
            raise RCTError("XLearner")

        self._estimator = XLearner(**estimator_params)
        self._fitEstimator()

    def fitPStratification(self, **estimator_params) -> Any:
        """
        Fit the Propensity score Stratification Estimator.

        A basic implementation of Propensity Stratification estimator
        based on Lunceford et al. (2004)
        :cite:t:`lunceford2004stratification`.

        Parameters
        ----------
        propensity_score_learner: str or Any, optional
            Estimator for propensity score.
            If not provided, defaults to LogisticRegression.
        num_strata: int, optional
            The number of strata.
            Default is 100.
        """

        if self._adjustment is self._IV:
            raise IVError("PStratification")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("PStratification")
        if self._X is None or len(self._X) == 0:
            raise RCTError("PStratification")

        self._estimator = PStratification(**estimator_params)
        self._fitEstimator()

    def fitIPW(self, **estimator_params) -> Any:
        """
        Fit the Inverse Propensity score Weighting Estimator.

        A basic implementation of the Inverse Propensity Score Weighting (IPW)
        estimator based on Lunceford et al. (2004)
        :cite:t:`lunceford2004stratification`.

        Parameters
        ----------
        propensity_score_learner: str or Any, optional
            Estimator for propensity score.
            If not provided, defaults to LogisticRegression.
        """

        if self._adjustment is self._IV:
            raise IVError("IPW")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("IPW")
        if self._X is None or len(self._X) == 0:
            raise RCTError("IPW")

        self._estimator = IPW(**estimator_params)
        self._fitEstimator()

    # Frontdoor

    def fitSimplePlugIn(self, **estimator_params) -> Any:
        """
        Fit the Plug-in Estimator.

        Uses the (original) Frontdoor Adjustment Formula to derive
        the plug-in estimator. Does not account for covariates.
        Inspired by Guo et al. (2023).
        :cite:t:`fulcher2020robust`,
        :cite:t:`guo2023targeted`.

        Parameters
        ----------
        learner: str or object, optional
            Estimator for outcome variable.
            If not provided, defaults to LinearRegression.
        propensity_learner: str |or object, optional
            Estimator for treatment proability.
            If not provided, defaults to LogisticRegression.
        """

        if self._adjustment is self._RCT:
            raise RCTError("PlugIn")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("PlugIn")
        if self._adjustment is self._IV:
            raise IVError("PlugIn")

        self._estimator = SimplePlugIn(**estimator_params)
        self._fitEstimator()

    def fitGeneralizedPlugIn(self, **estimator_params) -> Any:
        """
        Fit the Generalized plug-in Estimator.

        Basic implementation of the second plug-in estimator in
        Guo et al. (2023). Must provide covariates.
        :cite:t:`fulcher2020robust`,
        :cite:t:`guo2023targeted`.

        Parameters
        ----------
        estimator_params: Any
            The parameters of the estimator.
        """

        if self._adjustment is self._RCT:
            raise RCTError("GeneralizedPlugIn")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("GeneralizedPlugIn")
        if self._adjustment is self._IV:
            raise IVError("GeneralizedPlugIn")

        self._estimator = GeneralizedPlugIn(**estimator_params)
        self._fitEstimator()

    # Instrumental Variable

    def fitWald(self) -> Any:
        """
        Fit the Wald Estimator.

        An implementation of the Wald estimator which computes the
        Local Average Causal Effect (LACE), also know as the Local Average
        Treatment Effect (LATE).
        Only Supports binary instruments.
        """

        if self._adjustment is self._RCT:
            raise RCTError("Wald")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("Wald")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("Wald")

        self._estimator = Wald()
        self._fitEstimator()

    def fitWaldIPW(self, **estimator_params) -> Any:
        """
        Fit the Wald Inverse Probability Weighting Estimator.

        A basic implementation of the Wald estimand with Inverse Propensity
        Score Weighting which computes the Local Average Causal Effect (LACE).
        Only Supports binary instruments.
        :cite:t:`choi2021instrumental`.

        Parameters
        ----------
        iv_probability_learner: str or Any, optional
            Estimator for instrumental variable probability.
            If not provided, defaults to LogisticRegression.
        """

        if self._adjustment is self._RCT:
            raise RCTError("WaldIPW")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("WaldIPW")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("WaldIPW")

        self._estimator = WaldIPW(**estimator_params)
        self._fitEstimator()

    def fitNormalizedWaldIPW(self, **estimator_params) -> Any:
        """
        Fit the Normalized Wald Inverse Probability Weighting Estimator.

        A basic implementation of the normalized Wald estimator with Inverse Propensity
        Score Weighting which computes the Local Average Causal Effect (LACE).
        Only Supports binary instruments.
        :cite:t:`choi2021instrumental`.

        Parameters
        ----------
        iv_probability_learner: str or Any, optional
            Estimator for instrumental variable probability.
            If not provided, defaults to LogisticRegression.
        """

        if self._adjustment is self._RCT:
            raise RCTError("NormalizedWaldIPW")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("NormalizedWaldIPW")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("NormalizedWaldIPW")

        self._estimator = NormalizedWaldIPW(**estimator_params)
        self._fitEstimator()

    def fitTSLS(self, **estimator_params) -> Any:
        """
        Fit the Two Stage Least Squares Estimator.

        A basic implementation of the Two Stage Least-Squares Estimator.
        Only supports Linear Models, must have `.coef_` attribute.
        :cite:t:`angrist1995two`.

        Parameters
        ----------
        learner: str or Any, optional
            Base estimator for all learners.
            If not provided, defaults to LinearRegression.
        treatment_learner: str or Any, optional
            Estimator for treatment assignment.
            Overrides `learner` if specified.
        outcome_learner: str or Any, optional
            Estimator for outcome.
            Overrides `learner` if specified.
        """

        if self._adjustment is self._RCT:
            raise RCTError("TSLS")
        if self._adjustment is self._BACKDOOR:
            raise BackdoorError("TSLS")
        if self._adjustment is self._FRONTDOOR:
            raise FrontdoorError("TSLS")

        self._estimator = TSLS(**estimator_params)
        self._fitEstimator()

    # Estimation

    def estimateCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect based on the initialized data.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
            Specifies conditions for estimating causal effects.
            - If `pd.DataFrame`, estimates the Individual Causal Effect (ICE)
                for each row.
            - If `str`, estimates the Conditional Average Causal Effect (CACE).
                The string must be a valid pandas query.
            - If `None`, estimates the Average Causal Effect (ACE).
            Default is `None`.
        estimation_params: dict of str to Any, optional
            Additional parameters for the estimation method.
            Keys are parameter names, and values are the corresponding
            parameter values. Default is `None`.

        Returns
        -------
        float or np.ndarray
            For the `return_ci` argument in `estimation_params`:
            - If `return_ci` is `False`, returns the estimated causal effect
                as a float.
            - If `return_ci` is `True`, returns a tuple containing:True
            - The estimated causal effect (float)
            - The lower and upper bounds of the confidence interval
                (tuple of floats)

        Raises
        ------
        ValueError
            No adjustment have been selected before making the estimate.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        match self._adjustment:
            case self._IV:
                return self._estimateIVCausalEffect(
                    conditional,
                    **estimation_params
                )
            case self._FRONTDOOR:
                return self._estimateFrontdoorCausalEffect(
                    conditional,
                    **estimation_params
                )
            case self._BACKDOOR:
                return self._estimateBackdoorCausalEffect(
                    conditional,
                    **estimation_params
                )
            case self._RCT:
                return self._estimateRCTCausalEffect(
                    conditional,
                    **estimation_params
                )
            case self._UNKNOWN:
                return self._estimateUnknownCausalEffect(
                    conditional,
                    **estimation_params
                )
            case _:
                raise MisspecifiedAdjustmentError("making an estimate")

    def _estimateRCTCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect using RCT adjustment.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
            Conditions for estimating causal effects.
        estimation_params: dict[str, Any], optional
            Additional parameters for the estimation method.

        Returns
        -------
        float or np.ndarray
            The estimated causal effect.

        Raises
        ------
        ValueError
            The inputed conditional is invalid.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        if estimation_params is None:
            estimation_params = dict()
        # ICE
        if isinstance(conditional, pd.DataFrame):
            conditional = pd.DataFrame(conditional)
            return self._estimator.predict(
                X = conditional[[*self._X]],
                treatment = conditional[self._T],
                y = conditional[self._y],
                **estimation_params
            )
        # CACE
        elif isinstance(conditional, str):
            cond_df = self._df.query(conditional)
            if len(cond_df) == 0:
                raise EmptyConditionError()
            predictions = self._estimator.predict(
                X = cond_df[[*self._X]],
                treatment = cond_df[self._T],
                y = cond_df[self._y],
                **estimation_params
            )
            return predictions.mean()
        # ACE
        elif conditional is None:
            return self._estimator.estimate_ate(
                X = self._df[[*self._X]],
                treatment = self._df[self._T],
                y = self._df[self._y],
                **estimation_params
            )
        else:
            raise InvalidConditionError()

    def _estimateIVCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect using instrumental
        variable adjustment.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
            Conditions for estimating causal effects.
        estimation_params: dict[str, Any], optional
            Additional parameters for the estimation method.

        Returns
        -------
        float or np.ndarray
            The estimated causal effect.

        Raises
        ------
        ValueError
            The inputed conditional is invalid.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        if estimation_params is None:
            estimation_params = dict()
        # ICE
        if isinstance(conditional, pd.DataFrame):
            conditional = pd.DataFrame(conditional)
            return self._estimator.predict(
                X = conditional[[*self._X]],
                w = conditional[[*self._w]],
                treatment = conditional[self._T],
                y = conditional[self._y],
                **estimation_params
            )
        # CACE
        elif isinstance(conditional, str):
            cond_df = self._df.query(conditional)
            if len(cond_df) == 0:
                raise EmptyConditionError()
            predictions = self._estimator.predict(
                X = cond_df[[*self._X]],
                w = cond_df[[*self._w]],
                treatment = cond_df[self._T],
                y = cond_df[self._y],
                **estimation_params
            )
            return predictions.mean()
        # ACE
        elif conditional is None:
            return self._estimator.estimate_ate(
                X = self._df[[*self._X]],
                w = self._df[[*self._w]],
                treatment = self._df[self._T],
                y = self._df[self._y],
                pretrain = True,
                **estimation_params
            )
        else:
            raise InvalidConditionError()

    def _estimateFrontdoorCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect using generalized
        frontdoor adjustment.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
           Conditions for estimating treatment effects.
        estimation_params: dict[str, Any], optional
            Additional parameters for the estimation method.

        Returns
        -------
        float or np.ndarray
            The estimated treatment effect.

        Raises
        ------
        ValueError
            The inputed conditional is invalid.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        if estimation_params is None:
            estimation_params = dict()
        # ICE
        if isinstance(conditional, pd.DataFrame):
            conditional = pd.DataFrame(conditional)
            return self._estimator.predict(
                X = conditional[[*self._X]],
                treatment = conditional[self._T],
                y = conditional[self._y],
                M = conditional[[*self._M]],
                **estimation_params
            )
        # CACE
        elif isinstance(conditional, str):
            cond_df = self._df.query(conditional)
            if len(cond_df) == 0:
                raise EmptyConditionError()
            predictions = self._estimator.predict(
                X = cond_df[[*self._X]],
                treatment = cond_df[self._T],
                y = cond_df[self._y],
                M = cond_df[[*self._M]],
                **estimation_params
            )
            return predictions.mean()
        # ACE
        elif conditional is None:
            return self._estimator.estimate_ate(
                X = self._df[[*self._X]],
                treatment = self._df[self._T],
                y = self._df[self._y],
                M = self._df[[*self._M]],
                pretrain = True,
                **estimation_params
            )
        else:
            raise InvalidConditionError()

    def _estimateBackdoorCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect using backdoor adjustment.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
            Specifies conditions for estimating treatment effects.
        estimation_params: dict[str, Any], optional
            Additional parameters for the estimation method.

        Returns
        -------
        float or np.ndarray
            The estimated treatment effect.

        Raises
        ------
        ValueError
            The inputed conditional is invalid.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        if estimation_params is None:
            estimation_params = dict()
        # ICE
        if isinstance(conditional, pd.DataFrame):
            conditional = pd.DataFrame(conditional)
            return self._estimator.predict(
                X = conditional[[*self._X]],
                treatment = conditional[self._T],
                y = conditional[self._y],
                **estimation_params
            )
        # CACE
        elif isinstance(conditional, str):
            cond_df = self._df.query(conditional)
            if len(cond_df) == 0:
                raise EmptyConditionError()
            predictions = self._estimator.predict(
                X = cond_df[[*self._X]],
                treatment = cond_df[self._T],
                y = cond_df[self._y],
                **estimation_params
            )
            return predictions.mean()
        # ACE
        elif conditional is None:
            return self._estimator.estimate_ate(
                X = self._df[[*self._X]],
                treatment = self._df[self._T],
                y = self._df[self._y],
                pretrain = True,
                **estimation_params
            )
        else:
            raise InvalidConditionError()

    def _estimateUnknownCausalEffect(
            self,
            conditional: pd.DataFrame | str | None = None,
            **estimation_params: Any
        ) -> float | np.ndarray:
        """
        Estimate the causal or treatment effect using unknown adjustment.

        Parameters
        ----------
        conditional: pd.DataFrame, str, or None, optional
            Specifies conditions for estimating treatment effects.
        estimation_params: dict[str, Any], optional
            Additional parameters for the estimation method.

        Returns
        -------
        float or np.ndarray
            The estimated treatment effect.

        Raises
        ------
        ValueError
            The inputed conditional is invalid.
        """

        assert self._estimator is not None, \
            "Please fit an estimator before attempting to make an estimate."

        if estimation_params is None:
            estimation_params = dict()
        # ICE
        if isinstance(conditional, pd.DataFrame):
            conditional = pd.DataFrame(conditional)
            return self._estimator.predict(
                treatment = conditional[self._T],
                y = conditional[self._y],
                **estimation_params
            )
        # CACE
        elif isinstance(conditional, str):
            cond_df = self._df.query(conditional)
            if len(cond_df) == 0:
                raise EmptyConditionError()
            predictions = self._estimator.predict(
                treatment = cond_df[self._T],
                y = cond_df[self._y],
                **estimation_params
            )
            return predictions.mean()
        # ACE
        elif conditional is None:
            return self._estimator.estimate_ate(
                treatment = self._df[self._T],
                y = self._df[self._y],
                **estimation_params
            )
        else:
            raise InvalidConditionError()