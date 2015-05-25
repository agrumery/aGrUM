/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief The base class for all database handlers
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_HANDLER_H
#define GUM_LEARNING_DB_HANDLER_H

#include <utility>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>

#define CACHE_SIZE 128

namespace gum {

  namespace learning {

    /** @class DBHandler
     * @brief The base class for all database handlers
     * @ingroup learning_group
     */
    class DBHandler {
      public:
      /// returns the number of rows managed by the handler
      /** A handler needs not necessarily handle all the rows of the database.
       * For instance, RecordCounters cut the database into several pieces and
       * assign each piece to a handler. Then each handler is used to perform
       * countings only on a subset of the database */
      virtual unsigned long size() const noexcept = 0;

      /// the number of rows in the whole database
      virtual unsigned long DBSize() const noexcept = 0;

      /// returns the current row of the database
      /** @throws OutOfBounds if the handler points to the end of its area */
      virtual const DBRow &rowSafe() const = 0;

      /// returns the current row of the database
      /** @throws OutOfBounds if the handler points to the end of its area */
      virtual DBRow &rowSafe() = 0;

      /// returns the current row pointed to by the handler (unsafe version)
      /** @warning The method does not check whether the handler already points
       * to the end of its area. It is thus faster than method rowSafe () but,
       * when you call it, you must be sure that the row actually exists, i.e.,
       * that the handler has not reached its end. */
      virtual const DBRow &row() const = 0;

      /// returns the current row pointed to by the handler (unsafe version)
      /** @warning The method does not check whether the handler already points
       * to the end of its area. It is thus faster than method rowSafe () but,
       * when you call it, you must be sure that the row actually exists, i.e.,
       * that the handler has not reached its end. */
      virtual DBRow &row() = 0;

      /// go to the next row in the database
      virtual void nextRow() noexcept = 0;

      /// number of row
      virtual Idx numRow() const noexcept = 0;

      /// indicates wether there are still rows to parse in the database
      virtual bool hasRows() const noexcept = 0;

      /// puts the handler to the beginning of the database area it handles
      virtual void reset() = 0;

      /// sets the range of rows in the database that the handler will parse
      virtual void setRange(unsigned long begin, unsigned long end) noexcept = 0;

      /// returns the current range of the handler
      virtual std::pair<unsigned long, unsigned long> range() const noexcept = 0;

      /// returns the names of the variables
      virtual const std::vector<std::string> &variableNames() const noexcept = 0;

      /// returns the number of variables (columns) of the database
      virtual unsigned int nbVariables() const noexcept = 0;

      protected:
      /// a buffer to avoid cacheline problems due to parallelism
      char _align[CACHE_SIZE];
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_HANDLER_H */
