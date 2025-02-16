/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief gum::ApproximationSchemeListener header file.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef APPROXIMATIONSCHEMELISTENER_H
#define APPROXIMATIONSCHEMELISTENER_H

#include <agrum/agrum.h>

#include <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>

namespace gum {

  /**
   * @class ApproximationSchemeListener
   * @brief The ApproximationSchemeListener class.
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/base/core/approximations/approximationSchemeListener.h>
   * @endcode
   */
  class ApproximationSchemeListener: public Listener {
    public:
    /**
     * @brief Class constructor.
     * @param sch The scheme configuration.
     */
    explicit ApproximationSchemeListener(IApproximationSchemeConfiguration& sch);

    /**
     * @brief Class destructor.
     */
    virtual ~ApproximationSchemeListener();

    /**
     * @brief When progress was made.
     * @param src The signal source.
     * @param pourcent The advancement in percent.
     * @param error The number of errors.
     * @param time The time.
     */
    virtual void
        whenProgress(const void* src, const Size step, const double error, const double time)
        = 0;

    /**
     * @brief When it stops.
     * @param src The signal source.
     * @param message A message.
     */
    virtual void whenStop(const void* src, const std::string& message) = 0;

    protected:
    /**
     * Copy constructor.
     */
    ApproximationSchemeListener(const ApproximationSchemeListener& other);

    /**
     * Copy operator.
     */
    ApproximationSchemeListener& operator=(const ApproximationSchemeListener& other);

    private:
    /// The scheme configuration.
    IApproximationSchemeConfiguration& _sch_;
  };
}   // namespace gum

#endif   // APPROXIMATIONSCHEMELISTENER_H
