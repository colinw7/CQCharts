#ifndef CQChartsSymbol_H
#define CQChartsSymbol_H

#include <CQChartsSymbolType.h>
#include <CQChartsTmpl.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CMathUtil.h>
#include <QString>
#include <QStringList>
#include <cassert>

class CQChartsFile;

/*!
 * \brief Symbol Data
 * \ingroup Charts
 *
 * A symbol is a vector (dynamically resizable) shape supporting:
 *  + Builtin symbol types
 +  + UTF-8 Characters
 *  + SVG
 */
class CQChartsSymbol :
  public CQChartsComparatorBase<CQChartsSymbol>,
  public CQChartsToStringBase<CQChartsSymbol> {
 public:
  using SymbolType = CQChartsSymbolType;

  using BBox = CQChartsGeom::BBox;

  struct CharData {
    CharData() = default;

    explicit CharData(const QString &c, const QString &name="") :
     c(c), name(name) {
    }

    void reset() { c = ""; name = ""; }

    QString c;    //!< unicode character
    QString name; //!< display name
  };

  using Path = CQChartsPath;

  struct PathData {
    PathData() = default;

    explicit PathData(const Path &path, const QString &name="") :
     path(path), name(name) {
    }

    void reset() { path = CQChartsPath(); name = ""; }

    Path    path; //!< path
    QString name; //!< path name (should be non-empty and unique)
    QString src;  //!< path src
  };

  using Paths  = std::vector<Path>;
  using Styles = std::vector<CQChartsStyle>;

  struct SVGData {
    SVGData() = default;

    explicit SVGData(const Paths &paths, const Styles &styles, const QString &name="") :
     paths(paths), styles(styles), name(name) {
    }

    void reset() { paths.clear(); styles.clear(); name = ""; bbox = BBox(); }

    Paths   paths;  //!< paths
    Styles  styles; //!< styles
    QString name;   //!< svg name (should be non-empty an unique)
    QString src;    //!< svg src
    BBox    bbox;   //!< optional pre-calculated bbox
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  static QStringList pathNames();
  static QStringList svgNames();

  // interp index (min -> inf) wrapping around if bigger than max
  static CQChartsSymbol interpOutlineWrap(int i);
  static CQChartsSymbol interpOutlineWrap(int i, int imin, int imax);

  static CQChartsSymbol fromSVGFile(const CQChartsFile &file, const QString &name, bool styled);

  static CQChartsSymbol circle() { return CQChartsSymbol(SymbolType::Type::CIRCLE); }
  static CQChartsSymbol triangle() { return CQChartsSymbol(SymbolType::Type::TRIANGLE); }
  static CQChartsSymbol diamond() { return CQChartsSymbol(SymbolType::Type::DIAMOND); }

  static CQChartsSymbol vline() { return CQChartsSymbol(SymbolType::Type::VLINE); }
  static CQChartsSymbol hline() { return CQChartsSymbol(SymbolType::Type::HLINE); }

  static CQChartsSymbol set(const QString &name, int ind) {
    CQChartsSymbol s; s.name_ = name; s.type_ = Type::SYMBOL_SET; s.ind_ = ind; return s;
  }

 public:
  enum Type {
    NONE,
    SYMBOL,
    SYMBOL_SET,
    CHAR,
    PATH,
    SVG
  };

 public:
  CQChartsSymbol() = default;

  explicit CQChartsSymbol(SymbolType type);
  explicit CQChartsSymbol(SymbolType::Type type);

  explicit CQChartsSymbol(const CharData &charData);
  explicit CQChartsSymbol(const PathData &pathData);
  explicit CQChartsSymbol(const SVGData  &svgData );

  explicit CQChartsSymbol(const QString &s);

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  //bool isDot() const {
  //  return (type() == Type::SYMBOL && symbolType().type() == SymbolType::Type::DOT);
  //}

  //---

  // symbol data
  const SymbolType &symbolType() const { assert(type_ == Type::SYMBOL); return symbolType_; }
  void setSymbolType(const SymbolType &t) { reset(); type_ = Type::SYMBOL; symbolType_ = t; }

  //---

  // symbol set data
  const QString &setName() const { assert(type_ == Type::SYMBOL_SET); return name_; }
  void setSetName(const QString &s) { reset(); type_ = Type::SYMBOL_SET; name_ = s; }

  int setInd() const { assert(type_ == Type::SYMBOL_SET); return ind_; }
  void setSetInd(int ind) { reset(); type_ = Type::SYMBOL_SET; ind_ = ind; }

  //---

  // char data
  const QString &charStr () const { assert(type_ == Type::CHAR); return c_; }
  const QString &charName() const { assert(type_ == Type::CHAR); return name_; }

  //---

  // path data
  const Path    &path    () const;
  const QString &pathName() const { assert(type_ == Type::PATH); return name_; }

  //---

  // svg data
  const QString &svgName() const { assert(type_ == Type::SVG); return name_; }

  //---

  // svg/data data
  const Paths  &paths () const;
  const Styles &styles() const;

  QString srcStr() const;

  //---

  bool isFilled() const { return filled_; }
  void setFilled(bool b) { filled_ = b; }

  bool isStroked() const { return stroked_; }
  void setStroked(bool b) { stroked_ = b; }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsSymbol &lhs, const CQChartsSymbol &rhs) {
    if (lhs.type_       > rhs.type_      ) return  1;
    if (lhs.type_       < rhs.type_      ) return -1;
    if (lhs.symbolType_ > rhs.symbolType_) return  1;
    if (lhs.symbolType_ < rhs.symbolType_) return -1;
    if (lhs.c_          > rhs.c_         ) return  1;
    if (lhs.c_          < rhs.c_         ) return -1;
    if (lhs.name_       > rhs.name_      ) return  1;
    if (lhs.name_       < rhs.name_      ) return -1;
    if (lhs.filled_     > rhs.filled_    ) return  1;
    if (lhs.filled_     < rhs.filled_    ) return -1;
    if (lhs.stroked_    > rhs.stroked_   ) return  1;
    if (lhs.stroked_    < rhs.stroked_   ) return -1;
    return 0;
  }

 private:
  void reset() {
    type_       = Type::NONE;
    symbolType_ = SymbolType();
    c_          = "";
    name_       = "";
    filled_     = true;
    stroked_    = true;
  }

 private:
  Type       type_    { Type::NONE };
  SymbolType symbolType_;
  int        ind_     { -1 };
  QString    c_;
  QString    name_;
  bool       filled_  { true };
  bool       stroked_ { true };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsSymbol)

#endif
