#ifndef CQChartsTheme_H
#define CQChartsTheme_H

#include <QObject>
#include <QColor>
#include <QStringList>
#include <map>
#include <vector>

class CQChartsThemeObj;
class CQChartsGradientPalette;

#define CQChartsThemeMgrInst CQChartsThemeMgr::instance()

// manager class for named palettes and named themes
class CQChartsThemeMgr {
 public:
  static CQChartsThemeMgr *instance();

 ~CQChartsThemeMgr();

  void addNamedPalette(const QString &name, CQChartsGradientPalette *palette);

  CQChartsGradientPalette *getNamedPalette(const QString &name) const;

  void getPaletteNames(QStringList &names) const;

  void addTheme(const QString &name, CQChartsThemeObj *theme);

  CQChartsThemeObj *getTheme(const QString &name) const;

 private:
  CQChartsThemeMgr();

  void init();

 private:
  using ThemeMap      = std::map<QString,CQChartsThemeObj*>;
  using NamedPalettes = std::map<QString,CQChartsGradientPalette*>;

  NamedPalettes namedPalettes_; //! named palettes
  ThemeMap      themes_;        //! named themes
};

//------

// theme (ordered set of named palettes)
class CQChartsThemeObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor selectColor READ selectColor WRITE setSelectColor)
  Q_PROPERTY(QColor insideColor READ insideColor WRITE setInsideColor)

 public:
  CQChartsThemeObj();
 ~CQChartsThemeObj();

  // get/set theme name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  // number of palettes
  int numPalettes() const { return palettes_.size(); }

  // get/set nth palette
  CQChartsGradientPalette *palette(int i=0) const;
  void setPalette(int i, CQChartsGradientPalette *palette);

  // set nth palette to palette of specified name
  void setNamedPalette(int i, const QString &name);

  // shif palettes by n palces
  void shiftPalettes(int n);

  // get/set select color
  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c) { selectColor_ = c; }

  // get/set inside color
  const QColor &insideColor() const { return insideColor_; }
  void setInsideColor(const QColor &c) { insideColor_ = c; }

 protected:
  // initialize with all named palettes
  void addNamedPalettes();

  // add named palette
  void addNamedPalette(const QString &name);

 protected:
  using Palettes = std::vector<CQChartsGradientPalette*>;

  QString  name_;                       //! theme name
  Palettes palettes_;                   //! theme palette list
  QColor   selectColor_ { Qt::yellow }; //! selection color
  QColor   insideColor_ { Qt::cyan };   //! inside color
};

//---

// default theme
class CQChartsDefaultTheme : public CQChartsThemeObj {
 public:
  CQChartsDefaultTheme();
};

//---

// theme 1
class CQChartsTheme1 : public CQChartsThemeObj {
 public:
  CQChartsTheme1();
};

//---

// theme 2
class CQChartsTheme2 : public CQChartsThemeObj {
 public:
  CQChartsTheme2();
};

//---

class CQChartsTheme {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsTheme(const QString &name="") {
    setName(name);
  }

  CQChartsTheme(const CQChartsTheme &rhs) :
    name_(rhs.name_), obj_(rhs.obj_) {
  }

  CQChartsTheme &operator=(const CQChartsTheme &rhs) {
    name_ = rhs.name_;
    obj_  = rhs.obj_;

    return *this;
  }

  //--

  const QString &name() const { return name_; }

  bool setName(const QString &name) {
    CQChartsThemeObj *obj = CQChartsThemeMgrInst->getTheme(name);
    if (! obj) return false;

    name_ = name;
    obj_  = obj;

    return true;
  }

  //--

  CQChartsThemeObj *obj() const { return obj_; }

  //---

  QString toString() const { return name(); }
  bool fromString(const QString &s) { return setName(s); }

  //---

  friend bool operator==(const CQChartsTheme &lhs, const CQChartsTheme &rhs) {
    if (lhs.name_ != rhs.name_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsTheme &lhs, const CQChartsTheme &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << name().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsTheme &l) {
    l.print(os);

    return os;
  }

 private:
  QString           name_;
  CQChartsThemeObj* obj_ { nullptr };
};

//---

class CQChartsInterfaceTheme {
 public:
  CQChartsInterfaceTheme();

 ~CQChartsInterfaceTheme();

  CQChartsGradientPalette *palette() const { return palette_; }

  bool isDark() const { return isDark_; }
  void setDark(bool b);

  QColor interpColor(double r, bool scale) const;

 private:
  CQChartsGradientPalette* palette_      { nullptr };   //! palette
  bool                     isDark_       { false };     //! is dark
  QColor                   lightBgColor_ { "#ffffff" }; //! light bg color
  QColor                   lightFgColor_ { "#000000" }; //! light fg color
  QColor                   darkBgColor_  { "#222222" }; //! dark bg color
  QColor                   darkFgColor_  { "#dddddd" }; //! dark fg color
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsTheme)

#endif
