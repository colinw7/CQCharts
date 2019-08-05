#ifndef CQChartsEditBase_H
#define CQChartsEditBase_H

#include <QFrame>

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
  CQChartsEditBase(QWidget *parent=nullptr) :
   QFrame(parent) {
  }

  virtual ~CQChartsEditBase() { }

  CQChartsPlot *plot() const { return plot_; }
  virtual void setPlot(CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view() const { return view_; }
  virtual void setView(CQChartsView *view) { view_ = view; }

  void drawCenteredText(QPainter *painter, const QString &text);

 protected:
  CQChartsPlot* plot_ { nullptr };
  CQChartsView* view_ { nullptr };
};

//------

/*!
 * \brief base class for edit preview
 * \ingroup Charts
 */
class CQChartsEditPreview : public QFrame {
  Q_OBJECT

 public:
  CQChartsEditPreview(CQChartsEditBase *edit);

  void paintEvent(QPaintEvent *);

  virtual void draw(QPainter *painter) = 0;

  QSize sizeHint() const;

  static QColor interpColor(CQChartsPlot *plot, CQChartsView *view, const CQChartsColor &color);

 protected:
  CQChartsEditBase *edit_ { nullptr };
};

#endif
