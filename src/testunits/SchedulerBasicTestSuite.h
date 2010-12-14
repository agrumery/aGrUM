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

#include <sstream>
#include <cxxtest/AgrumTestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimCombinationDefault.h>
#include <agrum/graphicalModels/inference/scheduleCombinationBasic.h>
#include <agrum/graphicalModels/inference/schedulerBasic.h>

namespace gum {
  
  namespace tests {
    
    static gum::MultiDimImplementation<float>*
    schedule_sched_myadd ( const gum::MultiDimImplementation<float>& f1,
                           const gum::MultiDimImplementation<float>& f2 ) {
      return f1 + f2;
    }
    
    class SchedulerBasicTestSuite: public CxxTest::TestSuite {
    public:
      void test_construct1 () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 2);
        }
        
        gum::Potential<float> pot1;
        pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
        randomInit ( pot1 );
        gum::ScheduleMultiDim<float> f1 ( pot1 );
        
        gum::Potential<float> pot2;
        pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
        randomInit ( pot2 );
        gum::ScheduleMultiDim<float> f2 ( pot2 );
        gum::ScheduleMultiDim<float> f2bis ( pot2 );
        
        gum::Potential<float> pot3;
        pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
        randomInit ( pot3 );
        gum::ScheduleMultiDim<float> f3 ( pot3 );

        gum::Potential<float> pot4;
        pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
        randomInit ( pot4 );
        gum::ScheduleMultiDim<float> f4 ( pot4 );
 
        Schedule<float> schedule;
 
        gum::ScheduleCombinationBasic<float> comb1 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set1;
        set1 << &f1 << &f2;
        const ScheduleMultiDim<float> result1 = comb1.combine ( set1, schedule );
        
        gum::ScheduleCombinationBasic<float> comb2 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set2;
        set2 << &f2bis << &f3;
        const ScheduleMultiDim<float> result2 = comb2.combine ( set2, schedule );
        
        gum::ScheduleCombinationBasic<float> comb3 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set3;
        set3 << &result2 << &f4;
        const ScheduleMultiDim<float> result3 = comb3.combine ( set3, schedule );
        
        gum::ScheduleCombinationBasic<float> comb4 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set4;
        set4 << &result1 << &result3;
        const ScheduleMultiDim<float> result4 = comb4.combine ( set4, schedule );
        
        gum::ScheduleDeleteMultiDim<float> del1  ( result1 );
        const NodeSet& ops1 = schedule.operationsInvolving ( result1 );
        NodeId del1_id = schedule.insert ( del1 );
        schedule.forceAfter ( del1_id, ops1 );

        gum::ScheduleDeleteMultiDim<float> del2  ( result2 );
        const NodeSet& ops2 = schedule.operationsInvolving ( result2 );
        NodeId del2_id = schedule.insert ( del2 );
        schedule.forceAfter ( del2_id, ops2 );

        gum::ScheduleDeleteMultiDim<float> del3  ( result3 );
        const NodeSet& ops3 = schedule.operationsInvolving ( result3 );
        NodeId del3_id = schedule.insert ( del3 );
        schedule.forceAfter ( del3_id, ops3 );
 
        SchedulerBasic<float> scheduler;
        bool ok = scheduler.execute ( schedule );

        TS_ASSERT ( ok == true );
  
        gum::ScheduleCombine<float> comb11 ( f1, f2, schedule_sched_myadd );
        comb11.execute ();
        const ScheduleMultiDim<float>& result11 = comb11.result ();
        gum::ScheduleCombine<float> comb12 ( f2, f3, schedule_sched_myadd );
        comb12.execute ();
        const ScheduleMultiDim<float>& result12 = comb12.result ();
        gum::ScheduleCombine<float> comb13 ( result12, f4, schedule_sched_myadd );
        comb13.execute ();
        const ScheduleMultiDim<float>& result13 = comb13.result ();
        gum::ScheduleCombine<float> comb14 ( result11, result13, schedule_sched_myadd );
        comb14.execute ();
        const ScheduleMultiDim<float>& result14 = comb14.result ();

        TS_ASSERT ( result14.multiDim () == result4.multiDim () );

        gum::ScheduleDeleteMultiDim<float> del4  ( result4 );
        gum::ScheduleDeleteMultiDim<float> del11 ( result11 );
        gum::ScheduleDeleteMultiDim<float> del12 ( result12 );
        gum::ScheduleDeleteMultiDim<float> del13 ( result13 );
        gum::ScheduleDeleteMultiDim<float> del14 ( result14 );
        del4.execute  ();
        del11.execute ();
        del12.execute ();
        del13.execute ();
        del14.execute ();
        
        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }



      void test_construct2 () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 2);
        }
        
        gum::Potential<float> pot1;
        pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
        randomInit ( pot1 );
        gum::ScheduleMultiDim<float> f1 ( pot1 );
        
        gum::Potential<float> pot2;
        pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
        randomInit ( pot2 );
        gum::ScheduleMultiDim<float> f2 ( pot2 );
        gum::ScheduleMultiDim<float> f2bis ( pot2 );
        
        gum::Potential<float> pot3;
        pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
        randomInit ( pot3 );
        gum::ScheduleMultiDim<float> f3 ( pot3 );

        gum::Potential<float> pot4;
        pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
        randomInit ( pot4 );
        gum::ScheduleMultiDim<float> f4 ( pot4 );
 
        Schedule<float> schedule;
 
        gum::ScheduleCombinationBasic<float> comb1 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set1;
        set1 << &f1 << &f2;
        const ScheduleMultiDim<float> result1 = comb1.combine ( set1, schedule );
        
        gum::ScheduleCombinationBasic<float> comb2 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set2;
        set2 << &f2bis << &f3;
        const ScheduleMultiDim<float> result2 = comb2.combine ( set2, schedule );
        
        gum::ScheduleCombinationBasic<float> comb3 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set3;
        set3 << &result2 << &f4;
        const ScheduleMultiDim<float> result3 = comb3.combine ( set3, schedule );
        
        gum::ScheduleCombinationBasic<float> comb4 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set4;
        set4 << &result1 << &result3;
        const ScheduleMultiDim<float> result4 = comb4.combine ( set4, schedule );
        
        gum::ScheduleDeleteMultiDim<float> del1  ( result1 );
        const NodeSet& ops1 = schedule.operationsInvolving ( result1 );
        NodeId del1_id = schedule.insert ( del1 );
        schedule.forceAfter ( del1_id, ops1 );

        gum::ScheduleDeleteMultiDim<float> del2  ( result2 );
        const NodeSet& ops2 = schedule.operationsInvolving ( result2 );
        NodeId del2_id = schedule.insert ( del2 );
        schedule.forceAfter ( del2_id, ops2 );

        gum::ScheduleDeleteMultiDim<float> del3  ( result3 );
        const NodeSet& ops3 = schedule.operationsInvolving ( result3 );
        NodeId del3_id = schedule.insert ( del3 );
        schedule.forceAfter ( del3_id, ops3 );
 
        SchedulerBasic<float> scheduler;
        Scheduler<float>* scheduler2 = scheduler.newFactory ();
        
        bool ok = scheduler2->execute ( schedule );
        delete scheduler2;

        TS_ASSERT ( ok == true );
  
        gum::ScheduleCombine<float> comb11 ( f1, f2, schedule_sched_myadd );
        comb11.execute ();
        const ScheduleMultiDim<float>& result11 = comb11.result ();
        gum::ScheduleCombine<float> comb12 ( f2, f3, schedule_sched_myadd );
        comb12.execute ();
        const ScheduleMultiDim<float>& result12 = comb12.result ();
        gum::ScheduleCombine<float> comb13 ( result12, f4, schedule_sched_myadd );
        comb13.execute ();
        const ScheduleMultiDim<float>& result13 = comb13.result ();
        gum::ScheduleCombine<float> comb14 ( result11, result13, schedule_sched_myadd );
        comb14.execute ();
        const ScheduleMultiDim<float>& result14 = comb14.result ();

        TS_ASSERT ( result14.multiDim () == result4.multiDim () );

        gum::ScheduleDeleteMultiDim<float> del4  ( result4 );
        gum::ScheduleDeleteMultiDim<float> del11 ( result11 );
        gum::ScheduleDeleteMultiDim<float> del12 ( result12 );
        gum::ScheduleDeleteMultiDim<float> del13 ( result13 );
        gum::ScheduleDeleteMultiDim<float> del14 ( result14 );
        del4.execute  ();
        del11.execute ();
        del12.execute ();
        del13.execute ();
        del14.execute ();
        
        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }

      
      void test_construct3 () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 2);
        }
        
        gum::Potential<float> pot1;
        pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
        randomInit ( pot1 );
        gum::ScheduleMultiDim<float> f1 ( pot1 );
        
        gum::Potential<float> pot2;
        pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
        randomInit ( pot2 );
        gum::ScheduleMultiDim<float> f2 ( pot2 );
        gum::ScheduleMultiDim<float> f2bis ( pot2 );
        
        gum::Potential<float> pot3;
        pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
        randomInit ( pot3 );
        gum::ScheduleMultiDim<float> f3 ( pot3 );

        gum::Potential<float> pot4;
        pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
        randomInit ( pot4 );
        gum::ScheduleMultiDim<float> f4 ( pot4 );
 
        Schedule<float> schedule;
 
        gum::ScheduleCombinationBasic<float> comb1 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set1;
        set1 << &f1 << &f2;
        const ScheduleMultiDim<float> result1 = comb1.combine ( set1, schedule );
        
        gum::ScheduleCombinationBasic<float> comb2 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set2;
        set2 << &f2bis << &f3;
        const ScheduleMultiDim<float> result2 = comb2.combine ( set2, schedule );
        
        gum::ScheduleCombinationBasic<float> comb3 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set3;
        set3 << &result2 << &f4;
        const ScheduleMultiDim<float> result3 = comb3.combine ( set3, schedule );
        
        gum::ScheduleCombinationBasic<float> comb4 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set4;
        set4 << &result1 << &result3;
        const ScheduleMultiDim<float> result4 = comb4.combine ( set4, schedule );
        
        gum::ScheduleDeleteMultiDim<float> del1  ( result1 );
        const NodeSet& ops1 = schedule.operationsInvolving ( result1 );
        NodeId del1_id = schedule.insert ( del1 );
        schedule.forceAfter ( del1_id, ops1 );

        gum::ScheduleDeleteMultiDim<float> del2  ( result2 );
        const NodeSet& ops2 = schedule.operationsInvolving ( result2 );
        NodeId del2_id = schedule.insert ( del2 );
        schedule.forceAfter ( del2_id, ops2 );

        gum::ScheduleDeleteMultiDim<float> del3  ( result3 );
        const NodeSet& ops3 = schedule.operationsInvolving ( result3 );
        NodeId del3_id = schedule.insert ( del3 );
        schedule.forceAfter ( del3_id, ops3 );
 
        SchedulerBasic<float> scheduler;
        SchedulerBasic<float> scheduler3 = scheduler;
        
        bool ok = scheduler3.execute ( schedule );
 
        TS_ASSERT ( ok == true );
  
        gum::ScheduleCombine<float> comb11 ( f1, f2, schedule_sched_myadd );
        comb11.execute ();
        const ScheduleMultiDim<float>& result11 = comb11.result ();
        gum::ScheduleCombine<float> comb12 ( f2, f3, schedule_sched_myadd );
        comb12.execute ();
        const ScheduleMultiDim<float>& result12 = comb12.result ();
        gum::ScheduleCombine<float> comb13 ( result12, f4, schedule_sched_myadd );
        comb13.execute ();
        const ScheduleMultiDim<float>& result13 = comb13.result ();
        gum::ScheduleCombine<float> comb14 ( result11, result13, schedule_sched_myadd );
        comb14.execute ();
        const ScheduleMultiDim<float>& result14 = comb14.result ();

        TS_ASSERT ( result14.multiDim () == result4.multiDim () );

        gum::ScheduleDeleteMultiDim<float> del4  ( result4 );
        gum::ScheduleDeleteMultiDim<float> del11 ( result11 );
        gum::ScheduleDeleteMultiDim<float> del12 ( result12 );
        gum::ScheduleDeleteMultiDim<float> del13 ( result13 );
        gum::ScheduleDeleteMultiDim<float> del14 ( result14 );
        del4.execute  ();
        del11.execute ();
        del12.execute ();
        del13.execute ();
        del14.execute ();
        
        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }


      void test_construct4 () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 2);
        }
        
        gum::Potential<float> pot1;
        pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
        randomInit ( pot1 );
        gum::ScheduleMultiDim<float> f1 ( pot1 );
        
        gum::Potential<float> pot2;
        pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
        randomInit ( pot2 );
        gum::ScheduleMultiDim<float> f2 ( pot2 );
        gum::ScheduleMultiDim<float> f2bis ( pot2 );
        
        gum::Potential<float> pot3;
        pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
        randomInit ( pot3 );
        gum::ScheduleMultiDim<float> f3 ( pot3 );

        gum::Potential<float> pot4;
        pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
        randomInit ( pot4 );
        gum::ScheduleMultiDim<float> f4 ( pot4 );
 
        Schedule<float> schedule;
 
        gum::ScheduleCombinationBasic<float> comb1 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set1;
        set1 << &f1 << &f2;
        ScheduleMultiDim<float> result1 = comb1.combine ( set1, schedule );
        
        gum::ScheduleCombinationBasic<float> comb2 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set2;
        set2 << &result1 << &f3;
        ScheduleMultiDim<float> result2 = comb2.combine ( set2, schedule );
        
        gum::ScheduleCombinationBasic<float> comb3 ( schedule_sched_myadd );
        Set<const ScheduleMultiDim<float>*> set3;
        set3 << &result2 << &f4;
        ScheduleMultiDim<float> result3 = comb3.combine ( set3, schedule );
        
        gum::ScheduleDeleteMultiDim<float> del2  ( result2 );
        const NodeSet& ops2 = schedule.operationsInvolving ( result2 );
        NodeId del2_id = schedule.insert ( del2 );
        schedule.forceAfter ( del2_id, ops2 );

        gum::ScheduleDeleteMultiDim<float> del3  ( result3 );
        const NodeSet& ops3 = schedule.operationsInvolving ( result3 );
        NodeId del3_id = schedule.insert ( del3 );
        schedule.forceAfter ( del3_id, ops3 );
        
        SchedulerBasic<float> scheduler;
         
        bool ok = scheduler.execute ( schedule, 1 );
        TS_ASSERT ( ok == true ); 
        gum::ScheduleCombine<float> comb11 ( f1, f2, schedule_sched_myadd );
        comb11.execute ();
        const ScheduleMultiDim<float>& result11 = comb11.result ();
        TS_ASSERT ( result1.multiDim () == result11.multiDim () );
        TS_ASSERT ( result2.isAbstract () );
        TS_ASSERT ( result3.isAbstract () );

        ok = scheduler.execute ( schedule, 1 );
        TS_ASSERT ( ok == true ); 
        gum::ScheduleCombine<float> comb12 ( result11, f3, schedule_sched_myadd );
        comb12.execute ();
        const ScheduleMultiDim<float>& result12 = comb12.result ();
        TS_ASSERT ( result2.multiDim () == result12.multiDim () );
        TS_ASSERT ( result3.isAbstract () );

        ok = scheduler.execute ( schedule, 1 );
        TS_ASSERT ( ok == true ); 
        gum::ScheduleCombine<float> comb13 ( result12, f4, schedule_sched_myadd );
        comb13.execute ();
        const ScheduleMultiDim<float>& result13 = comb13.result ();
        TS_ASSERT ( result3.multiDim () == result13.multiDim () );

        gum::ScheduleDeleteMultiDim<float> del1  ( result1 );
        del1.execute ();
        
        scheduler.execute ( schedule );
        gum::ScheduleDeleteMultiDim<float> del11 ( result11 );
        gum::ScheduleDeleteMultiDim<float> del12 ( result12 );
        gum::ScheduleDeleteMultiDim<float> del13 ( result13 );
        del11.execute ();
        del12.execute ();
        del13.execute ();
        
        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }


      
    private:
      // ==========================================================================
      /// initialize randomly a table
      // ==========================================================================
      void randomInit ( Potential<float>& t ) {
        Instantiation i (t);
        srand ( time ( NULL) );
        for ( i.setFirst(); ! i.end(); ++i )
          t.set (i, (int) ( ( (float) rand() / RAND_MAX ) * 100000 ) );
      }

    };

  } /* namespace tests */

} /* namespace gum */

