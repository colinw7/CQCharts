#include <CQChartsSymbol.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbol, toString, fromString)

int CQChartsSymbol::metaTypeId;

//---

QString
CQChartsSymbol::
typeToName(Type type)
{
  switch (type) {
    case Type::DOT:       return "dot";
    case Type::CROSS:     return "cross";
    case Type::PLUS:      return "plus";
    case Type::Y:         return "y";
    case Type::TRIANGLE:  return "triangle";
    case Type::ITRIANGLE: return "itriangle";
    case Type::BOX:       return "box";
    case Type::DIAMOND:   return "diamond";
    case Type::STAR5:     return "star5";
    case Type::STAR6:     return "star6";
    case Type::CIRCLE:    return "circle";
    case Type::PENTAGON:  return "pentagon";
    case Type::IPENTAGON: return "ipentagon";
    case Type::HLINE:     return "hline";
    case Type::VLINE:     return "vline";
    default:              return "none";
  }
}

CQChartsSymbol::Type
CQChartsSymbol::
nameToType(const QString &str)
{
  QString lstr = str.toLower();

  if (lstr == "dot"      ) return Type::DOT;
  if (lstr == "cross"    ) return Type::CROSS;
  if (lstr == "plus"     ) return Type::PLUS;
  if (lstr == "y"        ) return Type::Y;
  if (lstr == "triangle" ) return Type::TRIANGLE;
  if (lstr == "itriangle") return Type::ITRIANGLE;
  if (lstr == "box"      ) return Type::BOX;
  if (lstr == "diamond"  ) return Type::DIAMOND;
  if (lstr == "star"     ) return Type::STAR5;
  if (lstr == "star5"    ) return Type::STAR5;
  if (lstr == "star6"    ) return Type::STAR6;
  if (lstr == "circle"   ) return Type::CIRCLE;
  if (lstr == "pentagon" ) return Type::PENTAGON;
  if (lstr == "ipentagon") return Type::IPENTAGON;
  if (lstr == "hline"    ) return Type::HLINE;
  if (lstr == "vline"    ) return Type::VLINE;

  return Type::NONE;
}

QStringList
CQChartsSymbol::
typeNames()
{
  static QStringList typeNames = QStringList() <<
    "dot"      << "cross"    << "plus"     << "y"        <<
    "triangle" << "itriangle"<< "box"      << "diamond"  <<
    "star5"    << "star6"    << "circle"   << "pentagon" <<
    "ipentagon"<< "hline"    << "vline";

  return typeNames;
}

//---

void
CQChartsSymbol::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSymbol);

  CQPropertyViewMgrInst->setUserName("CQChartsSymbol", "symbol");
}

CQChartsSymbol::
CQChartsSymbol(const QString &s)
{
  type_ = nameToType(s);
}

QString
CQChartsSymbol::
toString() const
{
  return typeToName(type_);
}

bool
CQChartsSymbol::
fromString(const QString &s)
{
  Type type = nameToType(s);

  if (type != Type::NONE)
    type_ = nameToType(s);

  return (type_ != Type::NONE);
}
