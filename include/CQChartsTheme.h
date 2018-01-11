#ifndef CQChartsTheme_H
#define CQChartsTheme_H

#include <CGradientPalette.h>
#include <QObject>

class CQChartsTheme;

#define CQChartsThemeMgrInst CQChartsThemeMgr::instance()

class CQChartsThemeMgr {
 public:
  static CQChartsThemeMgr *instance();

 ~CQChartsThemeMgr();

  void addTheme(const QString &name, CQChartsTheme *theme);

  CQChartsTheme *getTheme(const QString &name) const;

 private:
  CQChartsThemeMgr();

 private:
  using ThemeMap = std::map<QString,CQChartsTheme *>;

  ThemeMap themes_;
};

//------

class CQChartsTheme : public QObject {
  Q_OBJECT

 public:
  CQChartsTheme();
 ~CQChartsTheme();

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  CGradientPalette *theme() { return theme_; }

  CGradientPalette *palette() { return palette_; }

  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &v) { selectColor_ = v; }

 protected:
  QString           name_;
  CGradientPalette* theme_       { nullptr };
  CGradientPalette* palette_     { nullptr };
  QColor            selectColor_ { Qt::yellow };
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
