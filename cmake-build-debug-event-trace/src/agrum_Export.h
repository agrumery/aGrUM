
#ifndef agrum_EXPORT_H
#define agrum_EXPORT_H

#ifdef agrum_BUILT_AS_STATIC
#  define agrum_EXPORT
#  define AGRUM_NO_EXPORT
#else
#  ifndef agrum_EXPORT
#    ifdef agrum_EXPORTS
        /* We are building this library */
#      define agrum_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define agrum_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef AGRUM_NO_EXPORT
#    define AGRUM_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef AGRUM_DEPRECATED
#  define AGRUM_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef AGRUM_DEPRECATED_EXPORT
#  define AGRUM_DEPRECATED_EXPORT agrum_EXPORT AGRUM_DEPRECATED
#endif

#ifndef AGRUM_DEPRECATED_NO_EXPORT
#  define AGRUM_DEPRECATED_NO_EXPORT AGRUM_NO_EXPORT AGRUM_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef AGRUM_NO_DEPRECATED
#    define AGRUM_NO_DEPRECATED
#  endif
#endif

#endif /* agrum_EXPORT_H */
