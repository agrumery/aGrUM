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