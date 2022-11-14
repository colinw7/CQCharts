#ifndef CQChartsPaletteCanvas_H
#define CQChartsPaletteCanvas_H

#include <CQColorsEditCanvas.h>
#include <CQChartsWidgetIFace.h>

#include <QPointer>

class CQChartsView;

class CQChartsPaletteCanvas : public CQColorsEditCanvas, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(QString paletteName READ paletteName WRITE setPaletteName)
  Q_PROPERTY(bool    interface   READ isInterface WRITE setInterface)

 public:
  using View = CQChartsView;

 public:
  CQChartsPaletteCanvas(QWidget *parent=nullptr);

  View *view() const;
  void setView(View *view) override;

  const QString &paletteName() const { return paletteName_; }
  void setPaletteName(const QString &name);

  bool isInterface() const { return interface_; }
  void setInterface(bool b);

 private:
  void updatePalette();

 private:
  using ViewP = QPointer<View>;

  ViewP   view_;
  QString paletteName_;
  bool    interface_ { false };
};

#endif
