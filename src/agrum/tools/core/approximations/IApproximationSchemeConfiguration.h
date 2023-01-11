/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief This file contains getters and setters defintion for
 * ApproximationSchem settings
 *
 * @see ApproximationScheme
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
#define GUM_APPROXIMATION_SCHEME_CONFIGURATION_H

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/signal/signalers.h>
#include <agrum/tools/core/timer.h>

namespace gum {

  /**
   * @class IApproximationSchemeConfiguration
   * @brief Approximation Scheme.
   * @ingroup approximationpolicy_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/core/approximations/IApproximationSchemeConfiguration.h>
   * @endcode
   *
   * An interface for configuration of approximation scheme.
   */
  class IApproximationSchemeConfiguration {
    public:
    /// Progression, error and time.
    Signaler3< Size, double, double > onProgress;

    /// Criteria messageApproximationScheme
    Signaler1< const std::string& > onStop;

    /// The different state of an approximation scheme.
    enum class ApproximationSchemeSTATE : char {
      Undefined,
      Continue,
      Epsilon,
      Rate,
      Limit,
      TimeLimit,
      Stopped
    };

    // ======================================================================
    /// @name Constructor and destructor
    // ======================================================================
    /// @{
    /**
     * @brief Class constructors.
     */
    IApproximationSchemeConfiguration();

    /**
     * @brief Class destructor.
     */
    virtual ~IApproximationSchemeConfiguration();
    /// @}

    // ======================================================================
    /// @name Getters and setters
    // ======================================================================
    /// @{

    /**
     * @brief Returns the approximation scheme message.
     * @return Returns the approximation scheme message.
     */
    std::string messageApproximationScheme() const;

    /**
     * @brief Given that we approximate f(t), stopping criterion on
     * |f(t+1)-f(t)|.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param eps The new epsilon value.
     * @throw OutOfBounds Raised if eps < 0.
     */
    virtual void setEpsilon(double eps) = 0;

    /**
     * @brief Returns the value of epsilon.
     * @return Returns the value of epsilon.
     */
    virtual double epsilon() const = 0;

    /**
     * @brief Disable stopping criterion on epsilon.
     */
    virtual void disableEpsilon() = 0;

    /**
     * @brief Enable stopping criterion on epsilon.
     */
    virtual void enableEpsilon() = 0;

    /**
     * @brief Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     * @return Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     */
    virtual bool isEnabledEpsilon() const = 0;

    /**
     * @brief Given that we approximate f(t), stopping criterion on
     * d/dt(|f(t+1)-f(t)|).
     *
     * If the criterion was disabled it will be enabled
     *
     * @param rate The minimal epsilon rate.
     * @throw OutOfBounds if rate<0
     */
    virtual void setMinEpsilonRate(double rate) = 0;

    /**
     * @brief Returns the value of the minimal epsilon rate.
     * @return Returns the value of the minimal epsilon rate.
     */
    virtual double minEpsilonRate() const = 0;

    /**
     * @brief Disable stopping criterion on epsilon rate.
     */
    virtual void disableMinEpsilonRate() = 0;

    /**
     * @brief Enable stopping criterion on epsilon rate.
     */
    virtual void enableMinEpsilonRate() = 0;

    /**
     * @brief Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     */
    virtual bool isEnabledMinEpsilonRate() const = 0;

    /**
     * @brief Stopping criterion on number of iterations.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param max The maximum number of iterations.
     * @throw OutOfBounds Raised if max <= 1.
     */
    virtual void setMaxIter(Size max) = 0;

    /**
     * @brief Returns the criterion on number of iterations.
     * @return Returns the criterion on number of iterations.
     */
    virtual Size maxIter() const = 0;

    /**
     * @brief Disable stopping criterion on max iterations.
     */
    virtual void disableMaxIter() = 0;

    /**
     * @brief Enable stopping criterion on max iterations.
     */
    virtual void enableMaxIter() = 0;

    /**
     * @brief Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     */
    virtual bool isEnabledMaxIter() const = 0;

    /**
     * @brief Stopping criterion on timeout.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param timeout The timeout value in seconds.
     * @throw OutOfBounds Raised if timeout <= 0.0.
     */
    virtual void setMaxTime(double timeout) = 0;

    /**
     * @brief Returns the timeout (in seconds).
     * @return Returns the timeout (in seconds).
     */
    virtual double maxTime() const = 0;

    /**
     * @brief Returns the current running time in second.
     * @return Returns the current running time in second.
     */
    virtual double currentTime() const = 0;

    /**
     * @brief Disable stopping criterion on timeout.
     * @return Disable stopping criterion on timeout.
     */
    virtual void disableMaxTime() = 0;

    /**
     * @brief Enable stopping criterion on timeout.
     */
    virtual void enableMaxTime() = 0;

    /**
     * @brief Returns true if stopping criterion on timeout is enabled, false
     * otherwise.
     * @return Returns true if stopping criterion on timeout is enabled, false
     * otherwise.
     */
    virtual bool isEnabledMaxTime() const = 0;

    /**
     * @brief How many samples between two stopping is enable.
     * @param p The new period value.
     * @throw OutOfBounds Raised if p < 1.
     */
    virtual void setPeriodSize(Size p) = 0;

    /**
     * @brief Returns the period size.
     * @return Returns the period size.
     */
    virtual Size periodSize() const = 0;

    /**
     * @brief Set the verbosity on (true) or off (false).
     * @param v If true, then verbosity is turned on.
     */
    virtual void setVerbosity(bool v) = 0;

    /**
     * @brief Returns true if verbosity is enabled.
     * @return Returns true if verbosity is enabled.
     */
    virtual bool verbosity() const = 0;

    /**
     * @brief Returns the approximation scheme state.
     * @return Returns the approximation scheme state.
     */
    virtual ApproximationSchemeSTATE stateApproximationScheme() const = 0;

    /**
     * @brief Returns the number of iterations.
     * @return Returns the number of iterations.
     * @throw OperationNotAllowed Raised if the scheme did not perform.
     */
    virtual Size nbrIterations() const = 0;

    /**
     * @brief Returns the scheme history.
     * @return Returns the scheme history.
     * @throw OperationNotAllowed Raised if the scheme did not performed or
     * if verbosity is set to false.
     */
    virtual const std::vector< double >& history() const = 0;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/approximations/IApproximationSchemeConfiguration_inl.h>
#endif

#endif   // GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
