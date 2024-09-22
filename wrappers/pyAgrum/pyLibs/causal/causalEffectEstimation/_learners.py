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

from typing import Any

from ._utils import MisspecifiedLearnerError

found_sklearn = True
found_xgboost = True

try:
    import sklearn.linear_model
    import sklearn.tree
    import sklearn.ensemble
    import sklearn.svm
    import sklearn.neighbors
except ImportError as sklearn_import_error:
    found_sklearn = False

try:
    import xgboost
except ImportError as xgboost_import_error:
    found_xgboost = False

def learnerFromString(learner_string: Any) -> Any:
    """
    Retrieve a scikit-learn learner based on a string specification.

    Parameters
    ----------
    learner_string: str
        The string specifying a supported scikit-learn model.

    Returns
    -------
    sklearn.base.BaseEstimator
        An instance of a scikit-learn estimator corresponding to the
        specified string. This object will be used as the learner.
    """

    try:
        match learner_string:
            case "LinearRegression":
                return sklearn.linear_model.LinearRegression()
            case "LogisticRegression":
                return sklearn.linear_model.LogisticRegression()
            case "Ridge":
                return sklearn.linear_model.Ridge()
            case "Lasso":
                return sklearn.linear_model.Lasso()
            case "PoissonRegressor":
                return sklearn.linear_model.PoissonRegressor()
            case "HuberRegressor":
                return sklearn.linear_model.HuberRegressor()
            case "DecisionTreeRegressor":
                return sklearn.tree.DecisionTreeRegressor()
            case "RandomForestRegressor":
                return sklearn.ensemble.RandomForestRegressor()
            case "GradientBoostingRegressor":
                return sklearn.ensemble.GradientBoostingRegressor()
            case "AdaBoostRegressor":
                return sklearn.ensemble.AdaBoostRegressor()
            case "SVR":
                return sklearn.svm.SVR()
            case "KNeighborsRegressor":
                return sklearn.neighbors.KNeighborsRegressor()
            case "XGBRegressor":
                return xgboost.XGBRegressor()
            case "XGBClassifier":
                return xgboost.XGBClassifier()
            case _:
                raise MisspecifiedLearnerError(learner_string)

    except NameError as error:
        if error.name == "sklearn" and not found_sklearn:
            raise sklearn_import_error
        if error.name == "xgboost" and not found_xgboost:
            raise xgboost_import_error