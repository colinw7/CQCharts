#include <CQChartsSymbol.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsSVGUtil.h>
#include <CQChartsFile.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbol, toString, fromString)

int CQChartsSymbol::metaTypeId;

//---

#if 0
static CQChartsSymbol
CQChartsSymbol::
interpOutline(double r)
{
  return CQChartsSymbol((CQChartsSymbol::Type)
    int(CMathUtil::map(r, 0, 1, minOutlineValue(), maxOutlineValue())));
}
#endif

// interp index (min -> inf) wrapping around if bigger than max
CQChartsSymbol
CQChartsSymbol::
interpOutlineWrap(int i)
{
  return interpOutlineWrap(i, SymbolType::minOutlineValue(), SymbolType::maxOutlineValue());
}

CQChartsSymbol
CQChartsSymbol::
interpOutlineWrap(int i, int imin, int imax)
{
#if 0
  int len = imax - imin + 1;

  int n = (i - imin)/len;

  int i1 = (i - n*len);
#else
  int i1 = CQChartsSymbolSet::interpIRange(i, imin, imax);
#endif
  assert(i1 >= SymbolType::minOutlineValue() && i1 <= SymbolType::maxOutlineValue());

  return CQChartsSymbol(SymbolType((SymbolType::Type) i1));
}

#if 0
CQChartsSymbol
CQChartsSymbol::
interpFill(double r)
{
  return CQChartsSymbol((CQChartsSymbol::Type)
    int(CMathUtil::map(r, 0, 1, minFillValue(), maxFillValue())));
}
#endif

//---

// TODO: move into charts
class CQChartsSymbolNamedPaths {
 public:
  struct PathsStylesData {
    CQChartsSymbol::Type   type { CQChartsSymbol::Type::NONE };
    CQChartsSymbol::Paths  paths;
    CQChartsSymbol::Styles styles;
    CQChartsGeom::BBox     bbox;
    QString                srcStr;
  };

 public:
  CQChartsSymbolNamedPaths() { }

  PathsStylesData &namedPathData(const QString &name) {
    auto p = namePathData_.find(name);

    if (p == namePathData_.end())
      return pathData_;

    return (*p).second;
  }

  void setNamedPathData(const QString &name, const PathsStylesData &pathsStylesData) {
    assert(pathsStylesData.paths.size() == pathsStylesData.styles.size());

    namePathData_[name] = pathsStylesData;
  }

  int numNamedPaths() const { return namePathData_.size(); }

  QStringList pathNames() const {
    QStringList names;

    for (const auto &pn : namePathData_) {
      if (pn.second.type == CQChartsSymbol::Type::PATH)
        names.push_back(pn.first);
    }

    return names;
  }

  QStringList svgNames() const {
    QStringList names;

    for (const auto &pn : namePathData_) {
      if (pn.second.type == CQChartsSymbol::Type::SVG)
        names.push_back(pn.first);
    }

    return names;
  }

 private:
  using NamePathData = std::map<QString, PathsStylesData>;

  NamePathData    namePathData_;
  PathsStylesData pathData_;
};

namespace {

CQChartsSymbolNamedPaths s_namedPaths;

}

QStringList
CQChartsSymbol::
pathNames()
{
  return s_namedPaths.pathNames();
}

QStringList
CQChartsSymbol::
svgNames()
{
  return s_namedPaths.svgNames();
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
CQChartsSymbol(SymbolType symbolType) :
 type_(Type::SYMBOL), symbolType_(symbolType)
{
}

CQChartsSymbol::
CQChartsSymbol(SymbolType::Type symbolType) :
 type_(Type::SYMBOL), symbolType_(symbolType)
{
}

CQChartsSymbol::
CQChartsSymbol(const CharData &charData) :
 type_(Type::CHAR), c_(charData.c), name_(charData.name)
{
}

CQChartsSymbol::
CQChartsSymbol(const PathData &pathData) :
 type_(Type::PATH), name_(pathData.name)
{
  bool exists = false;

  Paths paths;

  if (pathData.path.isValid())
    paths.push_back(pathData.path);

  auto srcStr = pathData.src;

  // get existing path of name if no path data
  if (name_ != "" && paths.empty()) {
    auto &pathsStylesData = s_namedPaths.namedPathData(name_);

    if (pathsStylesData.type == Type::PATH) {
      paths  = pathsStylesData.paths;
      srcStr = pathsStylesData.srcStr;

      exists = ! paths.empty();
    }
  }

  // if no existing path of name and no path data then use name or src to init path data
  if (name_ != "" && paths.empty()) {
    if (srcStr == "")
      srcStr = pathData.name;

    auto path = CQChartsPath(srcStr);

    if (path.isValid()) {
      if (pathData.src == "")
        name_ = QString("path.%1").arg(s_namedPaths.numNamedPaths() + 1);

      paths.push_back(std::move(path));
    }
  }

  // init name if not set
  if (name_ == "")
    name_ = QString("path.%1").arg(s_namedPaths.numNamedPaths() + 1);

  // init src string if not set
  if (srcStr == "" && ! paths.empty())
    srcStr = paths[0].toString();

  // add named path (if didn't already exist)
  if (! exists && ! paths.empty()) {
    CQChartsSymbolNamedPaths::PathsStylesData pathsStylesData;

    pathsStylesData.type   = type_;
    pathsStylesData.paths  = paths;
    pathsStylesData.srcStr = srcStr;

    for (std::size_t i = 0; i < paths.size(); ++i)
      pathsStylesData.styles.emplace_back();

    s_namedPaths.setNamedPathData(name_, pathsStylesData);
  }
}

CQChartsSymbol::
CQChartsSymbol(const SVGData &svgData) :
 type_(Type::SVG), name_(svgData.name)
{
  bool exists = false;

  auto paths  = svgData.paths;
  auto styles = svgData.styles;
  auto bbox   = svgData.bbox;
  auto srcStr = svgData.src;

  // get existing SVG of name if no SVG data
  if (name_ != "" && paths.empty() && styles.empty()) {
    auto &pathsStylesData = s_namedPaths.namedPathData(name_);

    if (pathsStylesData.type == Type::SVG) {
      paths  = pathsStylesData.paths;
      styles = pathsStylesData.styles;
      srcStr = pathsStylesData.srcStr;

      if (pathsStylesData.bbox.isValid())
        bbox = pathsStylesData.bbox;

      exists = ! paths.empty();
    }
  }

  // if no existing SVG of name and no SVG data then use name or src to set init SVG
  if (name_ != "" && paths.empty() && styles.empty()) {
    CQChartsSVGUtil::Paths  paths1;
    CQChartsSVGUtil::Styles styles1;
    CQChartsGeom::BBox      bbox1;

    if (srcStr == "")
      srcStr = svgData.name;

    if (CQChartsSVGUtil::svgFileToPaths(srcStr, paths1, styles1, bbox1)) {
      if (svgData.src == "")
        name_ = QString("svg.%1").arg(s_namedPaths.numNamedPaths() + 1);

      paths  = paths1;
      styles = styles1;
      bbox   = bbox1;
    }
  }

  // init name if not set
  if (name_ == "")
    name_ = QString("svg.%1").arg(s_namedPaths.numNamedPaths() + 1);

  // set bbox or use calculated
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

  // add named svg (if didn't already exist)
  if (! exists && ! paths.empty()) {
    CQChartsSymbolNamedPaths::PathsStylesData pathsStylesData;

    pathsStylesData.type   = type_;
    pathsStylesData.paths  = paths;
    pathsStylesData.styles = styles;
    pathsStylesData.bbox   = bbox;
    pathsStylesData.srcStr = srcStr;

    s_namedPaths.setNamedPathData(name_, pathsStylesData);
  }
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
  assert(type_ == Type::PATH || type_ == Type::SVG);

  return s_namedPaths.namedPathData(name_).paths;
}

const CQChartsSymbol::Styles &
CQChartsSymbol::
styles() const
{
  assert(type_ == Type::PATH || type_ == Type::SVG);

  return s_namedPaths.namedPathData(name_).styles;
}

QString
CQChartsSymbol::
srcStr() const
{
  assert(type_ == Type::PATH || type_ == Type::SVG);

  return s_namedPaths.namedPathData(name_).srcStr;
}

QString
CQChartsSymbol::
toString() const
{
  QString s;

  if      (type_ == Type::SYMBOL) {
    if (filled_ && stroked_)
      s = symbolType_.toString();
    else {
      s = QString("symbol:%1").arg(symbolType_.toString());

      if      (! filled_)
        s += ":stroked";
      else if (! stroked_)
        s += ":filled";
    }
  }
  else if (type_ == Type::SYMBOL_SET) {
    if (name_.length() && ind_ >= 0)
      s = QString("symbol_set:%1:%2").arg(name_).arg(ind_);
  }
  else if (type_ == Type::CHAR) {
    if (charName() != "")
      s = QString("char:%1:%2").arg(charName()).arg(charStr());
    else
      s = QString("char:%1").arg(charStr());
  }
  else if (type_ == Type::PATH) {
    s = QString("path:%1").arg(pathName());

    if      (! filled_)
      s += ":stroked";
    else if (! stroked_)
      s += ":filled";
  }
  else if (type_ == Type::SVG) {
    s = QString("svg:%1").arg(svgName());

    if      (! filled_)
      s += ":stroked";
    else if (! stroked_)
      s += ":filled";
  }

  return s;
}

bool
CQChartsSymbol::
fromString(const QString &s)
{
  reset();

  if      (s.left(7) == "symbol:") {
    type_ = Type::SYMBOL;

    auto s1 = s.mid(7);

    auto pos = s1.indexOf(":");

    QString attr;

    if (pos >= 0) {
      attr = s1.mid(pos + 1);
      s1   = s1.mid(0, pos);
    }

    symbolType_ = SymbolType(s1);

    if (! symbolType_.isValid())
      type_ = Type::NONE;

    filled_  = true;
    stroked_ = true;

    if      (attr == "filled")
      stroked_ = false;
    else if (attr == "stroked")
      filled_ = false;
  }
  else if (s.left(11) == "symbol_set:") {
    type_ = Type::SYMBOL_SET;

    auto s1 = s.mid(11);

    auto pos = s1.indexOf(":");
    if (pos <= 0) return false;

    name_ = s1.mid(0, pos);

    auto indStr = s1.mid(pos + 1);

    bool ok;
    ind_ = indStr .toInt(&ok);
    if (! ok) return false;
  }
  else if (s.left(5) == "char:") {
    type_ = Type::CHAR;

    auto c = s.mid(5);

    auto pos = c.indexOf(":");

    if (pos >= 0) {
      name_ = c.mid(pos + 1);

      c = c.mid(0, pos);
    }

    c_ = c;

    filled_  = true;
    stroked_ = true;
  }
  else if (s.left(5) == "path:") {
    type_ = Type::PATH;
    name_ = s.mid(5);

    auto pos = name_.indexOf(":");

    QString attr;

    if (pos >= 0) {
      attr  = name_.mid(pos + 1);
      name_ = name_.mid(0, pos);
    }

    //---

    QString srcStr;

    auto &pathData = s_namedPaths.namedPathData(name_);

    if (pathData.paths.empty()) {
      auto path = CQChartsPath(name_);

      if (path.isValid()) {
        srcStr = name_;
        name_  = QString("path.%1").arg(s_namedPaths.numNamedPaths() + 1);

        CQChartsSymbolNamedPaths::PathsStylesData pathsStylesData;

        pathsStylesData.type   = type_;
        pathsStylesData.srcStr = srcStr;

        pathsStylesData.paths.push_back(std::move(path));

        pathsStylesData.styles.emplace_back();

        s_namedPaths.setNamedPathData(name_, pathsStylesData);
      }
    }

    //---

    filled_  = true;
    stroked_ = true;

    if      (attr == "filled")
      stroked_ = false;
    else if (attr == "stroked")
      filled_ = false;
  }
  else if (s.left(4) == "svg:") {
    type_ = Type::SVG;
    name_ = s.mid(4);

    auto pos = name_.indexOf(":");

    QString attr;

    if (pos >= 0) {
      attr  = name_.mid(pos + 1);
      name_ = name_.mid(0, pos);
    }

    //---

    QString srcStr;

    auto &pathData = s_namedPaths.namedPathData(name_);

    if (pathData.paths.empty()) {
      CQChartsSVGUtil::Paths  paths1;
      CQChartsSVGUtil::Styles styles1;
      CQChartsGeom::BBox      bbox1;

      if (CQChartsSVGUtil::svgFileToPaths(name_, paths1, styles1, bbox1)) {
        srcStr = name_;
        name_  = QString("svg.%1").arg(s_namedPaths.numNamedPaths() + 1);

        CQChartsSymbolNamedPaths::PathsStylesData pathsStylesData;

        pathsStylesData.type   = type_;
        pathsStylesData.paths  = paths1;
        pathsStylesData.styles = styles1;
        pathsStylesData.bbox   = bbox1;
        pathsStylesData.srcStr = srcStr;

        s_namedPaths.setNamedPathData(name_, pathsStylesData);
      }
    }

    //---

    filled_  = true;
    stroked_ = true;

    if      (attr == "filled")
      stroked_ = false;
    else if (attr == "stroked")
      filled_ = false;
  }
  else {
    type_       = Type::SYMBOL;
    symbolType_ = SymbolType(s);

    if (! symbolType_.isValid()) {
      type_ = Type::CHAR;

      QString res;

      if (CQChartsUtil::encodeUtf(s, res))
        c_ = res;
      else
        c_ = s;
    }
  }

  return (type_ != Type::NONE);
}

//---

CQChartsSymbol
CQChartsSymbol::
fromSVGFile(const CQChartsFile &file, const QString &name, bool styled)
{
  CQChartsSVGUtil::Paths  paths;
  CQChartsSVGUtil::Styles styles;
  CQChartsGeom::BBox      bbox;

  if (! CQChartsSVGUtil::svgFileToPaths(file.resolve(), paths, styles, bbox))
    return CQChartsSymbol();

  CQChartsSymbol::SVGData svgData;

  svgData.name = name;

  if (svgData.name == "")
    svgData.name = file.resolve();

  if (! bbox.isSet()) {
    for (auto &path : paths)
      bbox += path.bbox();
  }

  svgData.bbox = bbox;

  int np = paths.size();

  assert(np == int(styles.size()));

  for (int ip = 0; ip < np; ++ip) {
    auto &path = paths[ip];

    auto path1 = CQChartsPath::moveScalePath(path.path(), bbox, 0.0, 0.0, 1.0, -1.0);

    path.setPath(path1);

    svgData.paths.push_back(path);

    CQChartsStyle style;

    if (styled)
      style = styles[ip];

    svgData.styles.push_back(std::move(style));
  }

  svgData.src = file.resolve();

  return CQChartsSymbol(svgData);
}
