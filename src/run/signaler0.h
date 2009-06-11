#ifndef SIGNALER0_H__
#define SIGNALER0_H__

#include "signaler.h"

namespace gum {

  namespace sig {

    class BasicEmitter0 {
      public:
        virtual ~BasicEmitter0() { };

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

          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it ) {
            ( *it )->getdest()->signal_link( this );
            _connectors.pushBack(( *it )->clone() );
          }
        }

        virtual ~BasicSignaler0() {
          GUM_DESTRUCTOR( BasicSignaler0 );
          unlink_all();
        }

        void unlink_all() {
          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it ) {
            ( *it )->getdest()->signal_unlink( this );
            delete *it;
          }

          _connectors.clear();
        }

        void unlink( Listener* l ) {
          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it )
            if (( *it )->getdest() == l ) {
              delete *it;
              _connectors.erase( it );
              l->signal_unlink( this );
              return;
            }
        }

        void slot_unlink( Listener* l ) {
          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ) {
            constConnectors_iterator itNext = it;
            ++itNext;

            if (( *it )->getdest() == l ) {
              delete *it;
              _connectors.erase( it );
            }

            it = itNext;
          }
        }

        void slot_duplicate( const Listener* oldtarget, Listener* newtarget ) {
          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it )
            if (( *it )->getdest() == oldtarget ) {
              _connectors.pushBack(( *it )->duplicate( newtarget ) );
            }
        }

      protected:
        ConnectorList _connectors;
    };

    template<class TargetClass>

    class Connector0 : public BasicEmitter0 {
      public:
        Connector0() {
          GUM_CONSTRUCTOR( Connector0 );
          __target = NULL;
          __method = NULL;
        }

        Connector0( TargetClass* target, void ( TargetClass::*method )( const void * ) ) {
          GUM_CONSTRUCTOR( Connector0 );
          __target = target;
          __method = method;
        }

        Connector0( const Connector0<TargetClass>* src ) : BasicEmitter0( src ) {
          GUM_CONS_CPY( Connector0 );
        }

        virtual ~Connector0() {
          GUM_DESTRUCTOR( Connector0 );
        }

        virtual BasicEmitter0* clone() {
          return new Connector0<TargetClass>( *this );
        }

        virtual BasicEmitter0* duplicate( Listener* pnewdest ) {
          return new Connector0<TargetClass>(( TargetClass * )pnewdest, __method );
        }

        virtual void notify( const void * src ) {
          ( __target->*__method )( src );
        }

        virtual Listener* getdest() const {
          return __target;
        }

      private:
        TargetClass* __target;
        void ( TargetClass::* __method )( const void * );
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

      template<class TargetClass>
      void link( TargetClass* target, void ( TargetClass::*method )( const void * ) ) {

        sig::Connector0<TargetClass>* conn = new sig::Connector0<TargetClass>( target, method );
        this->_connectors.pushBack( conn );
        target->signal_link( this );
      }

      void notify( const void *src ) {
        for ( constConnectors_iterator it = this->_connectors.begin(); it != this->_connectors.end(); ++it )
          ( *it )->notify( src );
      }

      void operator()( const void *src ) {
        for ( constConnectors_iterator it = this->_connectors.begin(); it != this->_connectors.end(); ++it )
          ( *it )->notify( src );
      }
  };

} // namespace gum

#define GUM_EMIT0(signal) this->signal(this)

#endif // SIGNALER0_H__

