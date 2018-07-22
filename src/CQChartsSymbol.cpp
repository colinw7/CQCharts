#include <CQChartsSymbol.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbol, toString, fromString)

//---

QString
CQChartsSymbol::
typeToName(CQChartsSymbol::Type type)
{
  switch (type) {
    case CQChartsSymbol::Type::DOT:       return "dot";
    case CQChartsSymbol::Type::CROSS:     return "cross";
    case CQChartsSymbol::Type::PLUS:      return "plus";
    case CQChartsSymbol::Type::Y:         return "y";
    case CQChartsSymbol::Type::TRIANGLE:  return "triangle";
    case CQChartsSymbol::Type::ITRIANGLE: return "itriangle";
    case CQChartsSymbol::Type::BOX:       return "box";
    case CQChartsSymbol::Type::DIAMOND:   return "diamond";
    case CQChartsSymbol::Type::STAR5:     return "star5";
    case CQChartsSymbol::Type::STAR6:     return "star6";
    case CQChartsSymbol::Type::CIRCLE:    return "circle";
    case CQChartsSymbol::Type::PENTAGON:  return "pentagon";
    case CQChartsSymbol::Type::IPENTAGON: return "ipentagon";
    case CQChartsSymbol::Type::HLINE:     return "hline";
    case CQChartsSymbol::Type::VLINE:     return "vline";
    default:                              return "none";
  }
}

CQChartsSymbol::Type
CQChartsSymbol::
nameToType(const QString &str)
{
  QString lstr = str.toLower();

  if (lstr == "dot"      ) return CQChartsSymbol::Type::DOT;
  if (lstr == "cross"    ) return CQChartsSymbol::Type::CROSS;
  if (lstr == "plus"     ) return CQChartsSymbol::Type::PLUS;
  if (lstr == "y"        ) return CQChartsSymbol::Type::Y;
  if (lstr == "triangle" ) return CQChartsSymbol::Type::TRIANGLE;
  if (lstr == "itriangle") return CQChartsSymbol::Type::ITRIANGLE;
  if (lstr == "box"      ) return CQChartsSymbol::Type::BOX;
  if (lstr == "diamond"  ) return CQChartsSymbol::Type::DIAMOND;
  if (lstr == "star"     ) return CQChartsSymbol::Type::STAR5;
  if (lstr == "star5"    ) return CQChartsSymbol::Type::STAR5;
  if (lstr == "star6"    ) return CQChartsSymbol::Type::STAR6;
  if (lstr == "circle"   ) return CQChartsSymbol::Type::CIRCLE;
  if (lstr == "pentagon" ) return CQChartsSymbol::Type::PENTAGON;
  if (lstr == "ipentagon") return CQChartsSymbol::Type::IPENTAGON;
  if (lstr == "hline"    ) return CQChartsSymbol::Type::HLINE;
  if (lstr == "vline"    ) return CQChartsSymbol::Type::VLINE;

  return CQChartsSymbol::Type::NONE;
}

//---

void
CQChartsSymbol::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsSymbol);
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

void
CQChartsSymbol::
fromString(const QString &s)
{
  Type type = nameToType(s);

  if (type != CQChartsSymbol::Type::NONE)
    type_ = nameToType(s);
}
