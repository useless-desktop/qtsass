
#if defined(QTSASS_STATIC_LIBRARY)
#  define QTSASS_EXPORT
#else 
# if defined(QTSASS_SHARED_LIBRARY)
#  define QTSASS_EXPORT Q_DECL_EXPORT
# else
#  define QTSASS_EXPORT Q_DECL_IMPORT
# endif
#endif


#include <memory>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qregularexpression.h>
