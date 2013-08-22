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


#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"

#include <agrum/core/smallobjectallocator/fixedAllocator.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
#include <agrum/core/timer.h>


namespace gum_tests {

  class SmallObjectAllocatorTestSuite: public CxxTest::TestSuite {

    private :

    public:

      // ==============================================================================
      // Test CREATION et DESTRUCTION dun Fixed Allocator
      // ==============================================================================
      void est_Fixed_Allocator_CONST_AND_DEST() {

        gum::FixedAllocator* fa;
        // Test constructor
        TS_ASSERT_THROWS_NOTHING( fa = new gum::FixedAllocator(5*sizeof(gum::Idx), 50));
        // Test destructor
        TS_ASSERT_THROWS_NOTHING( delete fa);
      }

      // ==============================================================================
      // Test Allocation et Deallocation d'un élément
      // ==============================================================================
      void est_Fixed_Allocator_ALLOC_DEALLOC_1_ELEM() {

        gum::FixedAllocator* fa = new gum::FixedAllocator(5*sizeof(gum::Idx), 50);
        void* pVoid;
        TS_ASSERT_THROWS_NOTHING( pVoid = fa->allocate() );
        TS_ASSERT_THROWS_NOTHING( fa->deallocate(pVoid) );
        TS_ASSERT_THROWS_NOTHING( delete fa);
      }

      // ==============================================================================
      // Test Allocation de 50 et 1 élément => création d'un nouveau chunk
      // ==============================================================================
      void est_Fixed_Allocator_ALLOC_DEALLOC_51_ELEM() {
        gum::FixedAllocator* fa = new gum::FixedAllocator(5*sizeof(gum::Idx), 50);
        std::vector<void*> vVoid;
        // Alocation des 50 premiers éléments
        for( int i = 0; i < 50; ++i)
          TS_ASSERT_THROWS_NOTHING( vVoid.push_back(fa->allocate()) );
        // Demande du 51ème élément
        TS_ASSERT_THROWS_NOTHING( vVoid.push_back(fa->allocate()) );
        // Désallocation de tous les éléments.
        for(std::vector<void*>::iterator vite = vVoid.begin(); vite != vVoid.end(); ++vite)
          TS_ASSERT_THROWS_NOTHING( fa->deallocate(*vite) );
        TS_ASSERT_THROWS_NOTHING( delete fa);
      }

      // ==============================================================================
      // Test d'allocation et déallocation cascade coirsée
      // Au terme de ce test, seul 7 Chunk supplémentaire doivent avoir été créé
      // ==============================================================================
      void est_Fixed_Allocator_ALLOC_DEALLOC_CROISEE() {
        gum::FixedAllocator* fa = new gum::FixedAllocator(5*sizeof(gum::Idx), 50);
        std::vector<std::vector<void*>*> vvVoid;
        for(int i = 0; i < 7; ++i)
          vvVoid.push_back(new std::vector<void*>());

        // Alocation de 250 premiers éléments
        for( int i = 0; i < 250; ++i)
          TS_ASSERT_THROWS_NOTHING( vvVoid[i/50]->push_back(fa->allocate()) );

        // Désallocation d'éléments.
        for(int i = 0; i < 50; ++i){
//              std::cout << "Destruction du " << i << "ème élément de la table " << i%5 << std::endl;
          TS_ASSERT_THROWS_NOTHING( fa->deallocate(vvVoid[i%5]->at(i)) );
        }

        // Réallocation et forçage diagonal
        std::vector<void*> v6;
        for( int i = 0; i < 125; ++i)
          TS_ASSERT_THROWS_NOTHING( v6.push_back(fa->allocate()) );

        // Nettoyage
        for(int i = 0; i < 7; ++i){
          for(std::vector<void*>::iterator vite = vvVoid[i]->begin(); vite != vvVoid[i]->end(); ++vite)
            TS_ASSERT_THROWS_NOTHING( fa->deallocate(*vite) );
          delete vvVoid[i];
        }
        for(std::vector<void*>::iterator vite = v6.begin(); vite != v6.end(); ++vite)
          TS_ASSERT_THROWS_NOTHING( fa->deallocate(*vite) );
        TS_ASSERT_THROWS_NOTHING( delete fa);
      }

      // ==============================================================================
      // Test CREATION et DESTRUCTION dun SmallObjectAllocator
      // ==============================================================================
      void est_Small_Object_Allocator_CONST_AND_DEST() {

        gum::SmallObjectAllocator* soa;
        // Test constructor
        TS_ASSERT_THROWS_NOTHING( soa = new gum::SmallObjectAllocator(8192, 24*sizeof(gum::Idx)));
        // Test destructor
        TS_ASSERT_THROWS_NOTHING( delete soa);
      }

      // ==============================================================================
      // Test Allocation et Deallocation d'un élément
      // ==============================================================================
      void est_Small_Object_Allocator_ALLOC_DEALLOC_1_ELEM() {
        gum::SmallObjectAllocator* soa = new gum::SmallObjectAllocator(8192, 24*sizeof(gum::Idx));
        void* pVoid;
        TS_ASSERT_THROWS_NOTHING( pVoid = soa->allocate(5*sizeof(gum::Idx)) );
        TS_ASSERT_THROWS_NOTHING( soa->deallocate(pVoid,5*sizeof(gum::Idx)) );
        TS_ASSERT_THROWS_NOTHING( delete soa);
      }

      // ==============================================================================
      // Test Allocation et Deallocation d'un élément
      // ==============================================================================
      void est_Small_Object_Allocator_ALLOC_DEALLOC_ELEMS() {

        gum::SmallObjectAllocator* soa = new gum::SmallObjectAllocator(8192, 24*sizeof(gum::Idx));
        std::vector<void*> vVoid;
        for(std::size_t i = 1; i < 13; ++i)
          TS_ASSERT_THROWS_NOTHING( vVoid.push_back(soa->allocate(2*i*sizeof(gum::Idx))));

        std::vector<std::size_t> dv = {12,6,3,9,4,11,2,7,1,5,8,10};
        for(int i = 0; i < 12; ++i)
          TS_ASSERT_THROWS_NOTHING( soa->deallocate(vVoid[dv[i]-1],2*dv[i]*sizeof(gum::Idx)) );
        TS_ASSERT_THROWS_NOTHING( delete soa);
      }

      // ==============================================================================
      // Test Allocation et Deallocation d'un élément
      // ==============================================================================
      void test_Small_Object_Allocator_Heavy_Death_Comparison() {

        gum::Idx NbTotalSOA = 100000000;
        gum::Idx intervalle = 10 - 2;

        gum::SmallObjectAllocator* soa = new gum::SmallObjectAllocator(16384, 24*sizeof(gum::Idx));

        srand( time( NULL ));
        std::vector<gum::Idx> vectorSize;
        std::vector<gum::Idx> adsv;
        for(gum::Idx i = 0; i < NbTotalSOA; ++i){
            vectorSize.push_back(rand()%(intervalle) + intervalle/2);
        }

        gum::Timer timy;
        timy.reset();
        std::vector<void*> vVoid;
        for(gum::Idx i = 0; i < NbTotalSOA; ++i)
            TS_ASSERT_THROWS_NOTHING( vVoid.push_back(soa->allocate(vectorSize[i]*sizeof(gum::Idx))));

        for(gum::Idx i = 0; i < NbTotalSOA; ++i)
            TS_ASSERT_THROWS_NOTHING( soa->deallocate(vVoid[i],vectorSize[i]*sizeof(gum::Idx)) );
        double soaT = timy.pause();
        std::cout << "Temps Alloc/Dealloc SOA: " << soaT << std::endl;
        TS_ASSERT_THROWS_NOTHING( delete soa);

        std::vector<std::vector<gum::Idx>*> vVector;
        timy.resume();
        for(gum::Idx i = 0; i < NbTotalSOA; ++i)
            TS_ASSERT_THROWS_NOTHING( vVector.push_back(new std::vector<gum::Idx>(vectorSize[i],0)));
        for(gum::Idx i = 0; i < NbTotalSOA; ++i)
            TS_ASSERT_THROWS_NOTHING( delete vVector[i] );
        double vT = timy.pause();
        std::cout << "Temps Alloc/Dealloc SOA: " << vT - soaT << std::endl;

      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

