/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

namespace gum_tests {
class OMPCounterTestSuite : public CxxTest::TestSuite {
public:
	void testOMP() {
		gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv" ) );

		auto translators = gum::learning::make_translators( gum::learning::Create<gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>() );

		auto generators = gum::learning::make_generators( gum::learning::RowGeneratorIdentity() );

		auto filter = gum::learning::make_DB_row_filter( database, translators, generators );

		std::vector<gum::Size> modalities( 8, 2 );

		gum::learning::RecordCounter<> counter( filter, modalities );

		std::vector<gum::Idx> set1{ 0 };
		std::vector<gum::Idx> set2{ 1 };
		std::vector<gum::Idx> set3{ 1, 0 };

		counter.setMaxNbThreads( 1 );
		{
			gum::Idx id1 = counter.addNodeSet( set1 );
			gum::Idx id2 = counter.addNodeSet( set2 );
			gum::Idx id3 = counter.addNodeSet( set3 );

			counter.count();

			const std::vector<double>& vect1 = counter.getCounts( id1 );
			const std::vector<double>& vect2 = counter.getCounts( id2 );
			const std::vector<double>& vect3 = counter.getCounts( id3 );

			TS_ASSERT( vect1[0] == 5028 );
			TS_ASSERT( vect1[1] == 4972 );
			TS_ASSERT( vect2[0] == 538 );
			TS_ASSERT( vect2[1] == 9462 );

			TS_ASSERT( vect3[0] == 498 );
			TS_ASSERT( vect3[1] == 4530 );
			TS_ASSERT( vect3[2] == 40 );
			TS_ASSERT( vect3[3] == 4932 );
		}

		counter.clearNodeSets();

		counter.setMaxNbThreads( 2 );
		{
			gum::Idx id1 = counter.addNodeSet( set1 );
			gum::Idx id2 = counter.addNodeSet( set2 );
			gum::Idx id3 = counter.addNodeSet( set3 );

			counter.count();

			const std::vector<double>& vect1 = counter.getCounts( id1 );
			const std::vector<double>& vect2 = counter.getCounts( id2 );
			const std::vector<double>& vect3 = counter.getCounts( id3 );

			TS_ASSERT( vect1[0] == 5028 );
			TS_ASSERT( vect1[1] == 4972 );
			TS_ASSERT( vect2[0] == 538 );
			TS_ASSERT( vect2[1] == 9462 );

			TS_ASSERT( vect3[0] == 498 );
			TS_ASSERT( vect3[1] == 4530 );
			TS_ASSERT( vect3[2] == 40 );
			TS_ASSERT( vect3[3] == 4932 );
		}
	} // end testOMP()

}; // end class OMPCounterTestSuite

} /* namespace gum_tests */