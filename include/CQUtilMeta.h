#ifndef CQUtilMeta_H
#define CQUtilMeta_H

#include <QMetaType>
#include <QDataStream>
#include <QDebug>

#define CQUTIL_DCL_META_TYPE(TYPE) \
Q_DECLARE_METATYPE(TYPE) \
\
QDataStream &operator<<(QDataStream &out, const TYPE &t); \
\
QDataStream &operator>>(QDataStream &in, TYPE &t); \
\
QDebug operator<<(QDebug dbg, const TYPE &t);

//---

#define CQUTIL_DEF_META_TYPE_DATA_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER(); \
  out << str; \
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) { \
  QString str; \
  in >> str; \
  t.fromString(str); \
  return in; \
}

#define CQUTIL_DEF_META_TYPE_STD_DATA_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER().c_str(); \
  out << str; \
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) { \
  QString str; \
  in >> str; \
  t.fromString(str.toStdString()); \
  return in; \
} \

#define CQUTIL_DEF_META_TYPE_DEBUG(TYPE, GETTER) \
QDebug operator<<(QDebug dbg, const TYPE &t) \
{ \
  dbg.nospace() << t.GETTER(); \
  return dbg.maybeSpace(); \
} \

#define CQUTIL_DEF_META_TYPE_STD_DEBUG(TYPE, GETTER) \
QDebug operator<<(QDebug dbg, const TYPE &t) \
{ \
  dbg.nospace() << t.GETTER().c_str(); \
  return dbg.maybeSpace(); \
}

#define CQUTIL_DEF_META_TYPE_REGISTER(TYPE) \
namespace CQUtilMeta_##TYPE {\
int registerMetaType() { \
  static bool registered; \
  static int type = -1; \
  if (! registered) { \
    type = qRegisterMetaType<TYPE>(#TYPE); \
    qRegisterMetaTypeStreamOperators<TYPE>(#TYPE); \
    registered = true; \
  } \
  return type; \
} \
}

#define CQUTIL_DEF_META_TYPE(TYPE, GETTER, SETTER) \
CQUTIL_DEF_META_TYPE_DATA_STREAM(TYPE, GETTER, SETTER) \
\
CQUTIL_DEF_META_TYPE_DEBUG(TYPE, GETTER) \
\
CQUTIL_DEF_META_TYPE_REGISTER(TYPE)

//---

#define CQUTIL_DEF_META_TYPE_STD(TYPE, GETTER, SETTER) \
CQUTIL_DEF_META_TYPE_STD_DATA_STREAM(TYPE, GETTER, SETTER) \
\
CQUTIL_DEF_META_TYPE_STD_DEBUG(TYPE, GETTER) \
\
CQUTIL_DEF_META_TYPE_REGISTER(TYPE)

//---

#define CQUTIL_REGISTER_META(TYPE) CQUtilMeta_##TYPE :: registerMetaType()

#endif
