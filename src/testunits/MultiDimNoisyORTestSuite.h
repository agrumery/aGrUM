/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/rangeVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimNoisyOR.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/Gibbs.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>


class MultiDimNoisyORTestSuite: public CxxTest::TestSuite {
  public:
    void testCreationNoisyOr() {
      gum::LabelizedVariable a( "a","",2 ),b( "b","",2 ),c( "c","",2),d( "d","",2);
      gum::MultiDimNoisyOR<float> p(0.2);
			p.causalWeight(b,0.4);
			p.causalWeight(d,0.7);
      TS_GUM_ASSERT_THROWS_NOTHING( p<<a<<b<<c<<d );
			TS_ASSERT_EQUALS( p.toString(),"a<0,1>=noisyOR([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");

			gum::MultiDimNoisyOR<float> q(p);
			TS_ASSERT_EQUALS( q.toString(),"a<0,1>=noisyOR([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");
    }

		void testComputationInNoisyOR() {
			gum::LabelizedVariable cold("Cold","",2);
			gum::LabelizedVariable flu("Flu","",2);
			gum::LabelizedVariable malaria("Malaria","",2);
			gum::LabelizedVariable fever("Fever","",2);

			gum::MultiDimNoisyOR<float> p(0.0);
			p.causalWeight(cold,0.4);
			p.causalWeight(flu,0.8);
			p.causalWeight(malaria,0.9);
			p<<fever<<malaria<<flu<<cold;

			gum::Instantiation i(p);
			float witness[]={1,0,0.1,0.9,0.2,0.8,0.02,0.98,0.6,0.4,0.06,0.94,0.12,0.88,0.012,0.988};

			int j=0;
			for(i.setFirst();! i.end(); ++i,j++) {
				TS_ASSERT_DELTA(p[i],witness[j],1e-6);
			}
			
			gum::MultiDimNoisyOR<float> q(p);
			j=0;
			for(i.setFirst();! i.end(); ++i,j++) {
				TS_ASSERT_DELTA(q[i],witness[j],1e-6);
			}
		}
};
// kate: indent-mode cstyle; replace-tabs off; tab-width 2;
