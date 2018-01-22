#ifndef CQChartsTheme_H
#define CQChartsTheme_H

#include <CQChartsGradientPalette.h>
#include <QObject>

class CQChartsTheme;

#define CQChartsThemeMgrInst CQChartsThemeMgr::instance()

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

  NamedPalettes namedPalettes_;
  ThemeMap      themes_;
};

//------

class CQChartsTheme : public QObject {
  Q_OBJECT

 public:
  CQChartsTheme();
 ~CQChartsTheme();

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  CQChartsGradientPalette *theme() { return theme_; }

  CQChartsGradientPalette *palette(int i=0);
  void setPalette(int i, CQChartsGradientPalette *palette);

  void addNamedPalettes();

  void addNamedPalette(const QString &name);

  void setNamedPalette(int i, const QString &name);

  int numPalettes() const { return palettes_.size(); }

  void shiftPalettes(int n);

  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &v) { selectColor_ = v; }

 protected:
  void initPalette(CQChartsGradientPalette *palette);

  void initPalette1(CQChartsGradientPalette *palette);
  void initPalette2(CQChartsGradientPalette *palette);

 protected:
  using Palettes = std::vector<CQChartsGradientPalette*>;

  QString                  name_;
  CQChartsGradientPalette* theme_       { nullptr };
  Palettes                 palettes_;
  QColor                   selectColor_ { Qt::yellow };
};

class CQChartsDefaultTheme : public CQChartsTheme {
 public:
  CQChartsDefaultTheme();
};

class CQChartsLightTheme1 : public CQChartsTheme {
 public:
  CQChartsLightTheme1();
};

class CQChartsLightTheme2 : public CQChartsTheme {
 public:
  CQChartsLightTheme2();
};

class CQChartsDarkTheme1 : public CQChartsTheme {
 public:
  CQChartsDarkTheme1();
};

class CQChartsDarkTheme2 : public CQChartsTheme {
 public:
  CQChartsDarkTheme2();
};

#endif
