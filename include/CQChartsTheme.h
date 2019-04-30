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

//! \brief manager class for named palettes and named themes
class CQChartsThemeMgr : public QObject {
  Q_OBJECT

 public:
  static CQChartsThemeMgr *instance();

 ~CQChartsThemeMgr();

  //! named palettes
  void addNamedPalette(const QString &name, CQChartsGradientPalette *palette);

  CQChartsGradientPalette *getNamedPalette(const QString &name) const;

  void getPaletteNames(QStringList &names) const;

  void resetPalette(const QString &name);

  //---

  //! named themes
  void addTheme(const QString &name, CQChartsTheme *theme);

  CQChartsTheme *getTheme(const QString &name) const;

  void getThemeNames(QStringList &names) const;

 signals:
  void palettesChanged();
  void themesChanged();
  void themeChanged(const QString &name);

 private:
  CQChartsThemeMgr();

  void init();

 private slots:
  void themeChangedSlot();

 private:
  struct PaletteData {
    CQChartsGradientPalette *original { nullptr };
    CQChartsGradientPalette *current  { nullptr };
  };

  using ThemeMap      = std::map<QString,CQChartsTheme*>;
  using NamedPalettes = std::map<QString,PaletteData>;

  NamedPalettes namedPalettes_; //!< named palettes
  ThemeMap      themes_;        //!< named themes
};

//------

//! \brief theme (ordered set of named palettes)
class CQChartsTheme : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name        READ name)
  Q_PROPERTY(QString desc        READ desc        WRITE setDesc       )
  Q_PROPERTY(QColor  selectColor READ selectColor WRITE setSelectColor)
  Q_PROPERTY(QColor  insideColor READ insideColor WRITE setInsideColor)

 public:
  CQChartsTheme();
 ~CQChartsTheme();

  // get/set theme name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  // get/set theme description
  const QString &desc() const { return desc_; }
  void setDesc(const QString &s) { desc_ = s; }

  // number of palettes
  int numPalettes() const { return palettes_.size(); }

  // get/set nth palette
  CQChartsGradientPalette *palette(int i=0) const;
  void setPalette(int i, CQChartsGradientPalette *palette);

  // add named palette
  void addNamedPalette(const QString &name);

  // remove named palette
  void removeNamedPalette(const QString &name);

  // set nth palette to palette of specified name
  void setNamedPalette(int i, const QString &name);

  // move palette to new position
  void movePalette(const QString &name, int pos);

  // get position of named palette
  int paletteInd(const QString &name) const;

#if 0
  // shift palettes by n places
  void shiftPalettes(int n);
#endif

  // get/set select color
  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c);

  // get/set inside color
  const QColor &insideColor() const { return insideColor_; }
  void setInsideColor(const QColor &c);

 signals:
  void themeChanged();

 protected:
  // initialize with all named palettes
  void addNamedPalettes();

 protected:
  using Palettes = std::vector<CQChartsGradientPalette*>;

  QString  name_;                       //!< theme name
  QString  desc_;                       //!< theme description
  Palettes palettes_;                   //!< theme palette list
  QColor   selectColor_ { Qt::yellow }; //!< selection color
  QColor   insideColor_ { Qt::cyan };   //!< inside color
};

//---

//! \brief default theme
class CQChartsDefaultTheme : public CQChartsTheme {
 public:
  CQChartsDefaultTheme();
};

//---

//! \brief theme 1
class CQChartsTheme1 : public CQChartsTheme {
 public:
  CQChartsTheme1();
};

//---

//! \brief theme 2
class CQChartsTheme2 : public CQChartsTheme {
 public:
  CQChartsTheme2();
};

#endif
