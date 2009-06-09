#ifndef SIGNALER0_H__
#define SIGNALER0_H__

#include "signaler.h"

namespace gum {

  namespace sig {

    class BasicEmitter0 {
      public:
        virtual ~BasicEmitter0() {
//GUM_TRACE( "DESTRUCTION BASIC_EMITTER0" );
        };

        virtual Listener* getdest() const = 0;
        virtual void notify( const void * ) = 0;
        virtual BasicEmitter0* clone() = 0;
        virtual BasicEmitter0* duplicate( Listener* pnewdest ) = 0;
    };

    class BasicSignaler0 : public BasicSignaler {
      public:
        typedef List<BasicEmitter0 *>  ConnectorList;
        typedef ConnectorList::iterator constConnectors_iterator; //Connections::const_iterator

        BasicSignaler0() {
          GUM_CONSTRUCTOR( BasicSignaler0 );
        }

        BasicSignaler0( const BasicSignaler0& s ) : BasicSignaler( s ) {
          GUM_CONS_CPY( BasicSignaler0 );
          constConnectors_iterator it = s.__connectors.begin();
          constConnectors_iterator itEnd = s.__connectors.end();

          while ( it != itEnd ) {
            ( *it )->getdest()->signal_link( this );
            __connectors.pushBack(( *it )->clone() );

            ++it;
          }
        }

        virtual ~BasicSignaler0() {
          //GUM_TRACE( "yi !" );
          GUM_DESTRUCTOR( BasicSignaler0 );
          unlink_all();
        }

        void unlink_all() {
          //GUM_TRACE( "unlink !"<<__connectors.size() );
          constConnectors_iterator it = __connectors.begin();
          constConnectors_iterator itEnd = __connectors.end();

          while ( it != itEnd ) {
            ( *it )->getdest()->signal_unlink( this );
            //GUM_TRACE( "Destruction" );
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

    template<class dest_type>

    class Connector0 : public BasicEmitter0 {
      public:
        Connector0() {
          GUM_CONSTRUCTOR( Connector0 );
          _pobject = NULL;
          _pmemfun = NULL;
        }

        Connector0( dest_type* pobject, void ( dest_type::*pmemfun )( const void * ) ) {
          GUM_CONSTRUCTOR( Connector0 );
          _pobject = pobject;
          _pmemfun = pmemfun;
        }

        Connector0( const Connector0<dest_type>* src ) : BasicEmitter0( src ) {
          GUM_CONS_CPY( Connector0 );
        }

        virtual ~Connector0() {
          GUM_DESTRUCTOR( Connector0 );
        }

        virtual BasicEmitter0* clone() {
          return new Connector0<dest_type>( *this );
        }

        virtual BasicEmitter0* duplicate( Listener* pnewdest ) {
          return new Connector0<dest_type>(( dest_type * )pnewdest, _pmemfun );
        }

        virtual void notify( const void * src ) {
          ( _pobject->*_pmemfun )( src );
        }

        virtual Listener* getdest() const {
          return _pobject;
        }

      private:
        dest_type* _pobject;
        void ( dest_type::* _pmemfun )( const void * );
    };
  } // namespace sig

  class Signaler0 : public sig::BasicSignaler0 {
    public:
      Signaler0() {
        GUM_CONSTRUCTOR( Signaler0 );
      }

      Signaler0( const Signaler0& s ) : sig::BasicSignaler0( s ) {
        GUM_CONS_CPY( Signaler0 );
      }

      virtual ~Signaler0() {
        GUM_DESTRUCTOR( Signaler0 );
      }

      template<class desttype>
      void link( desttype* pclass, void ( desttype::*pmemfun )( const void * ) ) {

        sig::Connector0<desttype>* conn = new sig::Connector0<desttype>( pclass, pmemfun );
        __connectors.pushBack( conn );
        pclass->signal_link( this );
      }

      void notify( const void *src ) {

        constConnectors_iterator itNext, it = __connectors.begin();
        constConnectors_iterator itEnd = __connectors.end();

        while ( it != itEnd ) {
          itNext = it;
          ++itNext;

          ( *it )->notify( src );

          it = itNext;
        }
      }

      void operator()( const void *src ) {

        constConnectors_iterator itNext, it = __connectors.begin();
        constConnectors_iterator itEnd = __connectors.end();

        while ( it != itEnd ) {
          itNext = it;
          ++itNext;

          ( *it )->notify( src );

          it = itNext;
        }
      }
  };

} // namespace gum

#define GUM_EMIT0(sender,signal) (sender)->signal((sender))

#endif // SIGNALER0_H__

