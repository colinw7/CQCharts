#ifndef CQChartsTheme_H
#define CQChartsTheme_H

#include <QObject>
#include <QColor>
#include <QStringList>
#include <map>
#include <vector>

class CQChartsTheme;
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

  void addTheme(const QString &name, CQChartsTheme *theme);

  CQChartsTheme *getTheme(const QString &name) const;

 private:
  CQChartsThemeMgr();

  void init();

 private:
  using ThemeMap      = std::map<QString,CQChartsTheme*>;
  using NamedPalettes = std::map<QString,CQChartsGradientPalette*>;

  NamedPalettes namedPalettes_; //! named palettes
  ThemeMap      themes_;        //! named themes
};

//------

// theme (ordered set of named palettes)
class CQChartsTheme : public QObject {
  Q_OBJECT

 public:
  CQChartsTheme();
 ~CQChartsTheme();

  // get/set theme name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  // number of palettes
  int numPalettes() const { return palettes_.size(); }

  // get/set nth palette
  CQChartsGradientPalette *palette(int i=0);
  void setPalette(int i, CQChartsGradientPalette *palette);

  // set nth palette to palette of specified name
  void setNamedPalette(int i, const QString &name);

  // shif palettes by n palces
  void shiftPalettes(int n);

  // get/set select color
  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c) { selectColor_ = c; }

 protected:
  // initialize with all named palettes
  void addNamedPalettes();

  // add named palette
  void addNamedPalette(const QString &name);

 protected:
  using Palettes = std::vector<CQChartsGradientPalette*>;

  QString  name_;                       // theme name
  Palettes palettes_;                   // theme palette list
  QColor   selectColor_ { Qt::yellow }; // selection color
};

// default theme
class CQChartsDefaultTheme : public CQChartsTheme {
 public:
  CQChartsDefaultTheme();
};

// theme 1
class CQChartsTheme1 : public CQChartsTheme {
 public:
  CQChartsTheme1();
};

// theme 2
class CQChartsTheme2 : public CQChartsTheme {
 public:
  CQChartsTheme2();
};

#endif
