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
    case Type::CHAR:      return "char";
    case Type::PATH:      return "path";
    default:              return "none";
  }
}

CQChartsSymbol::Type
CQChartsSymbol::
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
  if (lstr == "char"     ) return Type::CHAR;
  if (lstr == "path"     ) return Type::PATH;

  return Type::NONE;
}

QStringList
CQChartsSymbol::
typeNames()
{
  static QStringList typeNames = QStringList() <<
    "dot"      << "cross"    << "plus"     << "y"        << "z"     <<
    "triangle" << "itriangle"<< "box"      << "diamond"  << "star5" <<
    "star6"    << "circle"   << "pentagon" << "ipentagon"<< "hline" <<
    "vline"    << "paw"      << "hash";

  return typeNames;
}

//---

// TODO: move into charts
namespace {

struct PathsStylesData {
  CQChartsSymbol::Paths  paths;
  CQChartsSymbol::Styles styles;
};

using NamePathData = std::map<QString, PathsStylesData>;

static NamePathData           s_namePathData;
static CQChartsSymbol::Paths  s_paths;
static CQChartsSymbol::Styles s_styles;

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
CQChartsSymbol(Type type) :
 type_(type)
{
}

CQChartsSymbol::
CQChartsSymbol(const CharData &charData) :
  type_(Type::CHAR), charData_(charData)
{
}

CQChartsSymbol::
CQChartsSymbol(const PathData &pathData) :
 type_(Type::PATH), pathName_(pathData.name)
{
  if (pathName_ == "")
    pathName_ = QString("path.%1").arg(s_namePathData.size() + 1);

  Paths paths = pathData.paths;

  // set bbox or use calculated
  BBox bbox = pathData.bbox;

  if (! bbox.isSet()) {
    for (auto &path : paths)
      bbox += path.bbox();
  }

  if (bbox.isValid()) {
    double sx = 2.0/bbox.getWidth ();
    double sy = 2.0/bbox.getHeight();
    double cx = bbox.getXMid();
    double cy = bbox.getYMid();

    for (auto &path : paths) {
      auto path1 = CQChartsPath::moveScalePath(path.path(), bbox, -cx, -cy, sx, sy);

      path.setPath(path1);
    }
  }

  PathsStylesData pathsStylesData;

  pathsStylesData.paths  = paths;
  pathsStylesData.styles = pathData.styles;

  s_namePathData[pathName_] = pathsStylesData;
}

CQChartsSymbol::
CQChartsSymbol(const QString &s)
{
  if (! fromString(s))
    type_ = Type::NONE;
}

const CQChartsSymbol::Paths &
CQChartsSymbol::
paths() const
{
  assert(type_ == Type::PATH);

  auto p = s_namePathData.find(pathName_);

  if (p == s_namePathData.end())
    return s_paths;

  return (*p).second.paths;
}

const CQChartsSymbol::Styles &
CQChartsSymbol::
styles() const
{
  assert(type_ == Type::PATH);

  auto p = s_namePathData.find(pathName_);

  if (p == s_namePathData.end())
    return s_styles;

  return (*p).second.styles;
}

QString
CQChartsSymbol::
toString() const
{
  if      (type_ == Type::CHAR) {
    if (charData_.name != "")
      return QString("char:%1:%2").arg(charName()).arg(charStr());
    else
      return QString("char:%1").arg(charData_.c);
  }
  else if (type_ == Type::PATH)
    return QString("path:%1").arg(pathName());
  else
    return typeToName(type_);
}

bool
CQChartsSymbol::
fromString(const QString &s)
{
  if      (s.left(5) == "char:") {
    type_ = Type::CHAR;

    auto c = s.mid(5);

    auto pos = c.indexOf(":");

    if (pos >= 0) {
      charData_.name = c.mid(pos + 1);

      c = c.mid(0, pos);
    }

    charData_.c = c;
  }
  else if (s.left(5) == "path:") {
    type_     = Type::PATH;
    pathName_ = s.mid(5);
  }
  else
    type_ = nameToType(s);

  return (type_ != Type::NONE);
}
