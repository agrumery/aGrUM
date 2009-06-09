#ifndef SIGNALER2_H__
#define SIGNALER2_H__

#include "signaler.h"

namespace gum {

  namespace sig {
    template<class type1,class type2>

    class BasicEmitter2 {
      public:
        virtual Listener* getdest() const = 0;
        virtual void notify( const void *,type1,type2 ) = 0;
        virtual BasicEmitter2<type1,type2>* clone() = 0;
        virtual BasicEmitter2<type1,type2>* duplicate( Listener* pnewdest ) = 0;
    };

    template<class type1,class type2>

    class BasicSignaler2 : public BasicSignaler {
      public:
        typedef List<BasicEmitter2<type1,type2> *>  ConnectorList;
        typedef typename ConnectorList::iterator constConnectors_iterator; //Connections::const_iterator

        BasicSignaler2() {
          GUM_CONSTRUCTOR( BasicSignaler2 );
        }

        BasicSignaler2( const BasicSignaler2& s ) : BasicSignaler( s ) {
          GUM_CONS_CPY( BasicSignaler0 );

          constConnectors_iterator it = s.__connectors.begin();
          constConnectors_iterator itEnd = s.__connectors.end();

          while ( it != itEnd ) {
            ( *it )->getdest()->signal_link( this );
            __connectors.pushBack(( *it )->clone() );

            ++it;
          }
        }

        virtual ~BasicSignaler2() {
          GUM_DESTRUCTOR( BasicSignaler2 );
          unlink_all();
        }

        void unlink_all() {

          constConnectors_iterator it = __connectors.begin();
          constConnectors_iterator itEnd = __connectors.end();

          while ( it != itEnd ) {
            ( *it )->getdest()->signal_unlink( this );
            delete *it;

            ++it;
          }

          __connectors.clear();
        }

        void unlink( Listener* pclass ) {

          constConnectors_iterator it = __connectors.begin();
          constConnectors_iterator itEnd = __connectors.end();

          while ( it != itEnd ) {
            if (( *it )->getdest() == pclass ) {
              delete *it;
              __connectors.erase( it );
              pclass->signal_unlink( this );
              return;
            }

            ++it;
          }
        }

        void slot_unlink( Listener* pslot ) {
          constConnectors_iterator it = __connectors.begin();
          constConnectors_iterator itEnd = __connectors.end();

          while ( it != itEnd ) {
            constConnectors_iterator itNext = it;
            ++itNext;

            if (( *it )->getdest() == pslot ) {
              __connectors.erase( it );
              //			delete *it;
            }

            it = itNext;
          }
        }

        void slot_duplicate( const Listener* oldtarget, Listener* newtarget ) {
          constConnectors_iterator it = __connectors.begin();
          constConnectors_iterator itEnd = __connectors.end();

          while ( it != itEnd ) {
            if (( *it )->getdest() == oldtarget ) {
              __connectors.pushBack(( *it )->duplicate( newtarget ) );
            }

            ++it;
          }
        }

      protected:
        ConnectorList __connectors;
    };

    template<class dest_type,class type1,class type2>

    class Connector2 : public BasicEmitter2<type1,type2> {
      public:
        Connector2() {
          GUM_CONSTRUCTOR( Connector2 );
          _pobject = NULL;
          _pmemfun = NULL;
        }

        Connector2( dest_type* pobject, void ( dest_type::*pmemfun )( const void *,type1,type2 ) ) {
          GUM_CONSTRUCTOR( Connector2 );
          _pobject = pobject;
          _pmemfun = pmemfun;
        }

        virtual ~Connector2() {
          GUM_DESTRUCTOR( Connector2 );
        }

        virtual BasicEmitter2<type1,type2>* clone() {
          return new Connector2<dest_type,type1,type2>( *this );
        }

        virtual BasicEmitter2<type1,type2>* duplicate( Listener* pnewdest ) {
          return new Connector2<dest_type,type1,type2>(( dest_type * )pnewdest, _pmemfun );
        }

        virtual void notify( const void * src,type1 arg1,type2 arg2 ) {
          ( _pobject->*_pmemfun )( src ,arg1 ,arg2 );
        }

        virtual Listener* getdest() const {
          return _pobject;
        }

      private:
        dest_type* _pobject;
        void ( dest_type::* _pmemfun )( const void * ,type1,type2 );
    };
  } // namespace sig

  template<class type1,class type2>

  class Signaler2 : public sig::BasicSignaler2<type1,type2> {
      typedef typename sig::BasicSignaler2<type1,type2>::constConnectors_iterator constConnectors_iterator;
    public:
      Signaler2() {
        GUM_CONSTRUCTOR( Signaler2 );
      }

      Signaler2( const Signaler2& s ) : sig::BasicSignaler2<type1,type2>( s ) {
        GUM_CONS_CPY( Signaler2 );
      }

      virtual ~Signaler2() {
        GUM_DESTRUCTOR( Signaler2 );
      }

      template<class desttype>
      void link( desttype* pclass, void ( desttype::*pmemfun )( const void *,type1,type2 ) ) {

        sig::Connector2<desttype,type1,type2>* conn = new sig::Connector2<desttype,type1,type2>( pclass, pmemfun );
        this->__connectors.pushBack( conn );
        pclass->signal_link( this );
      }

      void notify( const void *src,type1 arg1 , type2 arg2 ) {

        constConnectors_iterator itNext, it = this->__connectors.begin();
        constConnectors_iterator itEnd = this->__connectors.end();

        while ( it != itEnd ) {
          itNext = it;
          ++itNext;

          ( *it )->notify( src ,arg1,arg2 );

          it = itNext;
        }
      }

      void operator()( const void *src ,type1 arg1,type2 arg2 ) {

        constConnectors_iterator itNext, it = this->__connectors.begin();
        constConnectors_iterator itEnd = this->__connectors.end();

        while ( it != itEnd ) {
          itNext = it;
          ++itNext;

          ( *it )->notify( src,arg1,arg2 );

          it = itNext;
        }
      }
  };

} // namespace gum

#define GUM_EMIT2(sender,signal,a,b) (sender)->signal((sender),(a),(b))

#endif // SIGNALER2_H__

