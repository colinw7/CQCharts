#ifndef CQPropertyViewUtil_H
#define CQPropertyViewUtil_H

#include <CQUtil.h>

namespace CQPropertyViewUtil {

inline QString variantToString(const QVariant &var) {
  QString str;

  if (var.type() == QVariant::UserType) {
    if (! CQUtil::userVariantToString(var, str))
      return "";
  }
  else {
    if (! CQUtil::variantToString(var, str))
      return "";
  }

  return str;
}

}

#endif
