#ifndef CQChartsEditBase_H
#define CQChartsEditBase_H

#include <QFrame>
#include <QPointer>

class CQChartsPlot;
class CQChartsView;
class CQChartsColor;

class QGridLayout;

/*!
 * \brief base class for editor
 * \ingroup Charts
 */
class CQChartsEditBase : public QFrame {
  Q_OBJECT

 public:
  using View  = CQChartsView;
  using ViewP = QPointer<View>;
  using Plot  = CQChartsPlot;
  using PlotP = QPointer<Plot>;

 public:
  CQChartsEditBase(QWidget *parent=nullptr);

  virtual ~CQChartsEditBase() { }

  View *view() const;
  virtual void setView(View *view);

  Plot *plot() const;
  virtual void setPlot(Plot *plot);

  void drawCenteredText(QPainter *painter, const QString &text);

  void fixLabelWidth();

 protected:
  ViewP view_;
  PlotP plot_;
};

//------

/*!
 * \brief base class for edit preview
 * \ingroup Charts
 */
class CQChartsEditPreview : public QFrame {
  Q_OBJECT

 public:
  using View  = CQChartsView;
  using Plot  = CQChartsPlot;
  using Color = CQChartsColor;

 public:
  CQChartsEditPreview(CQChartsEditBase *edit);

  void paintEvent(QPaintEvent *) override;

  virtual void draw(QPainter *painter) = 0;

  QSize sizeHint() const override;

  static QColor interpColor(Plot *plot, View *view, const Color &color);

 protected:
  CQChartsEditBase *edit_ { nullptr };
};

#endif
