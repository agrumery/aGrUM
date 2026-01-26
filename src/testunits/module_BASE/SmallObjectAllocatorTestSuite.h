/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <cstdlib>
#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>

#include <agrum/base/core/smallobjectallocator/fixedAllocator.h>
#include <agrum/base/core/smallobjectallocator/smallObjectAllocator.h>
#include <agrum/base/core/timer.h>

#include "utils.h"

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SmallObjectAllocator
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct SmallObjectAllocatorTestSuite {
    private:

    public:
    // ==============================================================================
    // Test CREATION et DESTRUCTION dun Fixed Allocator
    // ==============================================================================
    static void test_Fixed_Allocator_CONST_AND_DEST() {
      gum::FixedAllocator* fa = nullptr;
      // Test constructor
      CHECK_NOTHROW(fa = new gum::FixedAllocator(5 * sizeof(gum::Idx), 50));
      // Test destructor
      CHECK_NOTHROW(if (fa != nullptr) delete fa);
    }   // namespace gum_tests

    // ==============================================================================
    // Test Allocation et Deallocation d'un élément
    // ==============================================================================
    static void test_Fixed_Allocator_ALLOC_DEALLOC_1_ELEM() {
      gum::FixedAllocator* fa    = new gum::FixedAllocator(5 * sizeof(gum::Idx), 50);
      void*                pVoid = nullptr;
      CHECK_NOTHROW(pVoid = fa->allocate());
      CHECK_NOTHROW(fa->deallocate(pVoid));
      CHECK_NOTHROW(if (fa != nullptr) delete fa);
    }

    // ==============================================================================
    // Test Allocation de 50 et 1 élément => création d'un nouveau chunk
    // ==============================================================================
    static void test_Fixed_Allocator_ALLOC_DEALLOC_51_ELEM() {
      gum::FixedAllocator* fa = new gum::FixedAllocator(5 * sizeof(gum::Idx), 50);
      std::vector< void* > vVoid;
      // Alocation des 50 premiers éléments
      for (int i = 0; i < 50; ++i)
        CHECK_NOTHROW(vVoid.push_back(fa->allocate()));
      // Demande du 51ème élément
      CHECK_NOTHROW(vVoid.push_back(fa->allocate()));
      // Désallocation de tous les éléments.
      for (std::vector< void* >::iterator vite = vVoid.begin(); vite != vVoid.end(); ++vite)
        CHECK_NOTHROW(fa->deallocate(*vite));
      CHECK_NOTHROW(delete fa);
    }

    // ==============================================================================
    // Test d'allocation et déallocation cascade coirsée
    // Au terme de ce test, seul 7 Chunk supplémentaire doivent avoir été créé
    // ==============================================================================
    static void test_Fixed_Allocator_ALLOC_DEALLOC_CROISEE() {
      gum::FixedAllocator*                 fa = new gum::FixedAllocator(5 * sizeof(gum::Idx), 50);
      std::vector< std::vector< void* >* > vvVoid;
      for (int i = 0; i < 7; ++i)
        vvVoid.push_back(new std::vector< void* >());

      // Alocation de 250 premiers éléments
      for (int i = 0; i < 250; ++i)
        CHECK_NOTHROW(vvVoid[i / 50]->push_back(fa->allocate()));

      // Désallocation d'éléments.
      for (int i = 0; i < 50; ++i) {
        //              std::cout << "Destruction du " << i << "ème élément de
        //              la table " << i%5 << std::endl;
        CHECK_NOTHROW(fa->deallocate(vvVoid[i % 5]->at(i)));
      }

      // Réallocation et forçage diagonal
      std::vector< void* > v6;
      for (int i = 0; i < 125; ++i)
        CHECK_NOTHROW(v6.push_back(fa->allocate()));

      // Nettoyage
      for (int i = 0; i < 7; ++i) {
        for (std::vector< void* >::iterator vite = vvVoid[i]->begin(); vite != vvVoid[i]->end();
             ++vite)
          CHECK_NOTHROW(fa->deallocate(*vite));
        delete vvVoid[i];
      }
      for (std::vector< void* >::iterator vite = v6.begin(); vite != v6.end(); ++vite)
        CHECK_NOTHROW(fa->deallocate(*vite));
      CHECK_NOTHROW(delete fa);
    }

    // ==============================================================================
    // Test CREATION et DESTRUCTION dun SmallObjectAllocator
    // ==============================================================================
    static void test_Small_Object_Allocator_CONST_AND_DEST() {
      // Test constructor
      CHECK_NOTHROW(gum::SmallObjectAllocator::instance());

      // Test destructor
      // CHECK_NOTHROW( delete soa );
    }

    // ==============================================================================
    // Test Allocation et Deallocation d'un élément
    // ==============================================================================
    static void test_Small_Object_Allocator_ALLOC_DEALLOC_1_ELEM() {
      void* pVoid = nullptr;
      CHECK_NOTHROW(pVoid = gum::SmallObjectAllocator::instance().allocate(5 * sizeof(gum::Idx)));
      CHECK_NOTHROW(gum::SmallObjectAllocator::instance().deallocate(pVoid, 5 * sizeof(gum::Idx)));
    }

    // ==============================================================================
    // Test Allocation et Deallocation d'un élément
    // ==============================================================================
    static void test_Small_Object_Allocator_ALLOC_DEALLOC_ELEMS() {
      std::vector< void* > vVoid;
      for (std::size_t i = 1; i < 13; ++i)
        CHECK_NOTHROW(vVoid.push_back(
            gum::SmallObjectAllocator::instance().allocate(gum::Size(2 * i * sizeof(gum::Idx)))));

      std::vector< std::size_t > dv = {12, 6, 3, 9, 4, 11, 2, 7, 1, 5, 8, 10};
      for (int i = 0; i < 12; ++i)
        CHECK_NOTHROW(gum::SmallObjectAllocator::instance().deallocate(
            vVoid[dv[i] - 1],
            gum::Size(2 * dv[i] * sizeof(gum::Idx))));
    }

    // ==============================================================================
    // Test Performance
    // ==============================================================================
    //    void est_Small_Object_Allocator_Heavy_Death_Comparison() {

    //      gum::Idx NbTotalSOA = 10000000;
    //      gum::Idx intervalle = 10 - 2;

    //      gum::SmallObjectAllocator* soa =
    //          new gum::SmallObjectAllocator( 16384, 24 * sizeof( gum::Idx ) );

    //      std::vector<gum::Idx> vectorSize;
    //      std::vector<gum::Idx> adsv;
    //      for ( gum::Idx i = 0; i < NbTotalSOA; ++i ) {
    //        vectorSize.push_back( rand() % ( intervalle ) + intervalle / 2 );
    //      }

    //      gum::Timer timy;
    //      timy.reset();
    //      std::vector<void*> vVoid;
    //      for ( gum::Idx i = 0; i < NbTotalSOA; ++i )
    //        CHECK_NOTHROW( vVoid.push_back(
    //            soa->allocate( vectorSize[i] * sizeof( gum::Idx ) ) ) );

    //      for ( gum::Idx i = 0; i < NbTotalSOA; ++i )
    //        CHECK_NOTHROW(
    //            soa->deallocate( vVoid[i], vectorSize[i] * sizeof( gum::Idx )
    //            ) );
    //      double soaT = timy.pause();
    //      std::cout << "Temps Alloc/Dealloc SOA: " << soaT << std::endl;
    //      CHECK_NOTHROW( delete soa );

    //      std::vector<std::vector<gum::Idx>*> vVector;
    //      timy.resume();
    //      for ( gum::Idx i = 0; i < NbTotalSOA; ++i )
    //        CHECK_NOTHROW( vVector.push_back(
    //            new std::vector<gum::Idx>( vectorSize[i], 0 ) ) );
    //      for ( gum::Idx i = 0; i < NbTotalSOA; ++i )
    //        CHECK_NOTHROW( delete vVector[i] );
    //      double vT = timy.pause();
    //      std::cout << "Temps Alloc/Dealloc SOA: " << vT - soaT << std::endl;
    //    }
  };

  GUM_TEST_ACTIF(_Fixed_Allocator_CONST_AND_DEST)
  GUM_TEST_ACTIF(_Fixed_Allocator_ALLOC_DEALLOC_1_ELEM)
  GUM_TEST_ACTIF(_Fixed_Allocator_ALLOC_DEALLOC_51_ELEM)
  GUM_TEST_ACTIF(_Fixed_Allocator_ALLOC_DEALLOC_CROISEE)
  GUM_TEST_ACTIF(_Small_Object_Allocator_CONST_AND_DEST)
  GUM_TEST_ACTIF(_Small_Object_Allocator_ALLOC_DEALLOC_1_ELEM)
  GUM_TEST_ACTIF(_Small_Object_Allocator_ALLOC_DEALLOC_ELEMS)
}   // namespace gum_tests
