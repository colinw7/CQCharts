#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <QFrame>
#include <CDisplayRange2D.h>
#include <CRange2D.h>

class CQChartsPlotExpander;
class CQPropertyTree;
class CGradientPalette;
class QAbstractItemModel;

class CQChartsPlot : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlot(QWidget *parent, QAbstractItemModel *model);

  QAbstractItemModel *model() const { return model_; }

  const CRange2D &dataRange() const { return dataRange_; }

  CGradientPalette *palette() { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; }

  void windowToPixel(double wx, double wy, double &px, double &py);
  void pixelToWindow(double px, double py, double &wx, double &wy);

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void resizeEvent(QResizeEvent *) override;

  void updateGeometry();

  QSize sizeHint() const;

 protected:
  QWidget*              parent_       { nullptr };
  QAbstractItemModel*   model_        { nullptr };
  CDisplayRange2D       displayRange_;
  CRange2D              dataRange_;
  CGradientPalette*     palette_      { nullptr };
  CQChartsPlotExpander* expander_     { nullptr };
  CQPropertyTree*       propertyTree_ { nullptr };
};

//---

class CQChartsPlotExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsPlotExpander(CQChartsPlot *plot);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsPlot *plot_     { nullptr };
  bool          expanded_ { false };
};

#endif
