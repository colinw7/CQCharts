#ifndef CQChartsEnv_H
#define CQChartsEnv_H

#include <QString>

namespace CQChartsEnv {

QString getString(const char *name, const QString &def="");
bool    getBool  (const char *name, bool def=false);
int     getInt   (const char *name, int def=0);

}

#endif
