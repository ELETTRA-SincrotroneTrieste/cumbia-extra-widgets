#ifndef CUMBIAEXTRAWIDGETS_GLOBAL_H
#define CUMBIAEXTRAWIDGETS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CUMBIAEXTRAWIDGETS_LIBRARY)
#  define CUMBIAEXTRAWIDGETSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CUMBIAEXTRAWIDGETSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CUMBIAEXTRAWIDGETS_GLOBAL_H
