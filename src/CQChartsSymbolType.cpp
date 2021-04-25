#include <CQChartsSymbolType.h>
#include <CQChartsSymbolSet.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbolType, toString, fromString)

int CQChartsSymbolType::metaTypeId;

//---

QString
CQChartsSymbolType::
typeToName(Type type)
{
  switch (type) {
    case Type::DOT:       return "dot";
    case Type::CROSS:     return "cross";
    case Type::PLUS:      return "plus";
    case Type::Y:         return "y";
    case Type::Z:         return "z";
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
    case Type::PAW:       return "paw";
    case Type::HASH:      return "hash";
    default:              return "none";
  }
}

CQChartsSymbolType::Type
CQChartsSymbolType::
nameToType(const QString &str)
{
  auto lstr = str.toLower();

  if (lstr == "dot"      ) return Type::DOT;
  if (lstr == "cross"    ) return Type::CROSS;
  if (lstr == "plus"     ) return Type::PLUS;
  if (lstr == "y"        ) return Type::Y;
  if (lstr == "z"        ) return Type::Z;
  if (lstr == "triangle" ) return Type::TRIANGLE;
  if (lstr == "itriangle") return Type::ITRIANGLE;
  if (lstr == "square"   ) return Type::BOX;
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
  if (lstr == "paw"      ) return Type::PAW;
  if (lstr == "hash"     ) return Type::HASH;

  return Type::NONE;
}

QStringList
CQChartsSymbolType::
typeNames()
{
  static auto typeNames = QStringList() <<
    "dot"      << "cross"    << "plus"     << "y"        << "z"     <<
    "triangle" << "itriangle"<< "box"      << "diamond"  << "star5" <<
    "star6"    << "circle"   << "pentagon" << "ipentagon"<< "hline" <<
    "vline"    << "paw"      << "hash";

  return typeNames;
}

//---

void
CQChartsSymbolType::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSymbolType);

  CQPropertyViewMgrInst->setUserName("CQChartsSymbolType", "symbolType");
}

CQChartsSymbolType::
CQChartsSymbolType(Type type) :
 type_(type)
{
  assert(isValidType(type_));
}

CQChartsSymbolType::
CQChartsSymbolType(const QString &s)
{
  if (! fromString(s))
    type_ = Type::NONE;
}

QString
CQChartsSymbolType::
toString() const
{
  return typeToName(type_);
}

bool
CQChartsSymbolType::
fromString(const QString &s)
{
  type_ = nameToType(s);

  return (type_ != Type::NONE);
}
