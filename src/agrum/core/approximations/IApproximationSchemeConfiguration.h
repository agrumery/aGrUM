/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief This file contains getters and setters defintion for
 * ApproximationSchem settings
 *
 * @see ApproximationScheme
 *
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
#define GUM_APPROXIMATION_SCHEME_CONFIGURATION_H

#include <cmath>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/timer.h>

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
   * #include <agrum/core/approximations/IApproximationSchemeConfiguration.h>
   * @endcode
   *
   * An interface for configuration of approximation scheme.
   */
  class IApproximationSchemeConfiguration {
    public:
    /// Progression, error and time.
    Signaler3<Size, double, double> onProgress;

    /// Criteria messageApproximationScheme
    Signaler1<std::string> onStop;

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
    ~IApproximationSchemeConfiguration();
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
     * @throw OutOfLowerBound Raised if eps < 0.
     */
    virtual void setEpsilon( double eps ) = 0;

    /**
     * @brief Returns the value of epsilon.
     * @return Returns the value of epsilon.
     */
    virtual double epsilon( void ) const = 0;

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
     * @throw OutOfLowerBound if rate<0
     */
    virtual void setMinEpsilonRate( double rate ) = 0;

    /**
     * @brief Returns the value of the minimal epsilon rate.
     * @return Returns the value of the minimal epsilon rate.
     */
    virtual double minEpsilonRate( void ) const = 0;

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
     * @throw OutOfLowerBound Raised if max <= 1.
     */
    virtual void setMaxIter( Size max ) = 0;

    /**
     * @brief Returns the criterion on number of iterations.
     * @return Returns the criterion on number of iterations.
     */
    virtual Size maxIter( void ) const = 0;

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
     * @throw OutOfLowerBound Raised if timeout <= 0.0.
     */
    virtual void setMaxTime( double timeout ) = 0;

    /**
     * @brief Returns the timeout (in seconds).
     * @return Returns the timeout (in seconds).
     */
    virtual double maxTime( void ) const = 0;

    /**
     * @brief Returns the current running time in second.
     * @return Returns the current running time in second.
     */
    virtual double currentTime( void ) const = 0;

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
     * @throw OutOfLowerBound Raised if p < 1.
     */
    virtual void setPeriodSize( Size p ) = 0;

    /**
     * @brief Returns the period size.
     * @return Returns the period size.
     */
    virtual Size periodSize( void ) const = 0;

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfLowerBound Raised if b < 1.
     */
    virtual void setBurnIn( Size b ) = 0;

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    virtual Size burnIn( void ) const = 0;

    /**
     * @brief Set the verbosity on (true) or off (false).
     * @param v If true, then verbosity is turned on.
     */
    virtual void setVerbosity( bool v ) = 0;

    /**
     * @brief Returns true if verbosity is enabled.
     * @return Returns true if verbosity is enabled.
     */
    virtual bool verbosity( void ) const = 0;

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
    virtual const std::vector<double>& history() const = 0;

    /**
     * @brief Configuration transmission.
     * @param cfg The configuration to copy.
     */
    void copyConfiguration( const IApproximationSchemeConfiguration& cfg );
  };
}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/core/approximations/IApproximationSchemeConfiguration_inl.h>
#endif

#endif  // GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
