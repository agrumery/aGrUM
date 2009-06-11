#define TOTO MAKE_NAME(BasicSignaler)
namespace gum {

  namespace sig {
    template<LIST_DECL_CLASSES>

    class MAKE_NAME(BasicEmitter) {
      public:
        virtual ~MAKE_NAME(BasicEmitter)() { }

        virtual Listener* getdest() const = 0;
        virtual void notify( const void *,LIST_DECL_ARGS ) = 0;
        virtual MAKE_NAME(BasicEmitter)<LIST_CLASSES>* clone() = 0;
        virtual MAKE_NAME(BasicEmitter)<LIST_CLASSES>* duplicate( Listener* pnewdest ) = 0;
    };

    template<LIST_DECL_CLASSES>

    class MAKE_NAME(BasicSignaler) : public BasicSignaler {
      public:
        typedef List<MAKE_NAME(BasicEmitter)<LIST_CLASSES> *>  ConnectorList;
        typedef typename ConnectorList::iterator constConnectors_iterator; //Connections::const_iterator

        MAKE_NAME(BasicSignaler)() {
          GUM_CONSTRUCTOR( TOTO );
        }

        MAKE_NAME(BasicSignaler)( const MAKE_NAME(BasicSignaler)& s ) : BasicSignaler( s ) {
          GUM_CONS_CPY( TOTO );

          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it ) {
            ( *it )->getdest()->signal_link( this );
            _connectors.pushBack(( *it )->clone() );
          }
        }

        virtual ~MAKE_NAME(BasicSignaler)() {
          GUM_DESTRUCTOR( TOTO );
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

    template<class TargetClass,LIST_DECL_CLASSES>

    class MAKE_NAME(Connector) : public MAKE_NAME(BasicEmitter)<LIST_CLASSES> {
      public:
        MAKE_NAME(Connector)() {
          GUM_CONSTRUCTOR( MAKE_NAME(Connector) );
          __target = NULL;
          __method = NULL;
        }

        MAKE_NAME(Connector)( TargetClass* target, void ( TargetClass::*method )( const void *,LIST_CLASSES ) ) {
          GUM_CONSTRUCTOR( MAKE_NAME(Connector) );
          __target = target;
          __method = method;
        }

        MAKE_NAME(Connector)( const MAKE_NAME(Connector)<TargetClass,LIST_CLASSES>* src ) : MAKE_NAME(BasicEmitter)<LIST_CLASSES>( src ) {
          GUM_CONS_CPY( MAKE_NAME(Connector) );
        }


        virtual ~MAKE_NAME(Connector)() {
          GUM_DESTRUCTOR( MAKE_NAME(Connector) );
        }

        virtual MAKE_NAME(BasicEmitter)<LIST_CLASSES>* clone() {
          return new MAKE_NAME(Connector)<TargetClass,LIST_CLASSES>( *this );
        }

        virtual MAKE_NAME(BasicEmitter)<LIST_CLASSES>* duplicate( Listener* pnewdest ) {
          return new MAKE_NAME(Connector)<TargetClass,LIST_CLASSES>(( TargetClass * )pnewdest, __method );
        }

        virtual void notify( const void * src,LIST_DECL_ARGS ) {
          ( __target->*__method )( src ,LIST_ARGS );
        }

        virtual Listener* getdest() const {
          return __target;
        }

      private:
        TargetClass* __target;
        void ( TargetClass::* __method )( const void * ,LIST_CLASSES );
    };
  } // namespace sig

  template<LIST_DECL_CLASSES>

  class MAKE_NAME(Signaler) : public sig::MAKE_NAME(BasicSignaler)<LIST_CLASSES> {
      typedef typename sig::MAKE_NAME(BasicSignaler)<LIST_CLASSES>::constConnectors_iterator constConnectors_iterator;
    public:
      MAKE_NAME(Signaler)() {
        GUM_CONSTRUCTOR( MAKE_NAME(Signaler) );
      }

      MAKE_NAME(Signaler)( const MAKE_NAME(Signaler)& s ) : sig::MAKE_NAME(BasicSignaler)<LIST_CLASSES>( s ) {
        GUM_CONS_CPY( MAKE_NAME(Signaler) );
      }

      virtual ~MAKE_NAME(Signaler)() {
        GUM_DESTRUCTOR( MAKE_NAME(Signaler) );
      }

      template<class TargetClass>
      void link( TargetClass* target, void ( TargetClass::*method )( const void *,LIST_CLASSES ) ) {

        sig::MAKE_NAME(Connector)<TargetClass,LIST_CLASSES>* conn = new sig::MAKE_NAME(Connector)<TargetClass,LIST_CLASSES>( target, method );
        this->_connectors.pushBack( conn );
        target->signal_link( this );
      }

      void notify( const void *src,LIST_DECL_ARGS ) {
        for ( constConnectors_iterator it = this->_connectors.begin(); it != this->_connectors.end(); ++it )
          ( *it )->notify( src ,LIST_ARGS );
      }

      void operator()( const void *src ,LIST_DECL_ARGS ) {
        for ( constConnectors_iterator it = this->_connectors.begin(); it != this->_connectors.end(); ++it )
          ( *it )->notify( src,LIST_ARGS );
      }
  };

} // namespace gum

