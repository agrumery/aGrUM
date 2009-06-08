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
/**
 * @file
 * @brief Outlined implementation of SkoobDriver.
 *
 * @author Lionel Torti
 */
// ============================================================================
#include <agrum/BN/io/BIFparser/BIFDriver.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/BN/io/BIFparser/BIFDriver.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum_bif {

// Default constructor.
// @param bayesNet A pointer over the BayesNet used by the factory. It can
//                 be a non-empty BayesNet.
BIFDriver::BIFDriver(gum::BayesNet<double>* bayesNet):
  __factory(new gum::BayesNetFactory<double>(bayesNet)),__traceScanning(false),__traceParsing(false)
{
  GUM_CONSTRUCTOR( BIFDriver );
}

// Destructor.
BIFDriver::~BIFDriver()
{
  GUM_DESTRUCTOR( BIFDriver );
  delete __factory;
}

} /* namespace gum_bif */
// ============================================================================
