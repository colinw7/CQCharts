#include <CQChartsPiePlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPiePlot::
CQChartsPiePlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
}

void
CQChartsPiePlot::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QFontMetrics fm(font());

  p.fillRect(rect(), QBrush(QColor(255,255,255)));

  //---

  int w = width ();
  int h = height();

  double xc = w/2.0;
  double yc = h/2.0;
  double r  = 0.9*std::min(w, h)/2.0;

  //---

  static QColor colors[] = {
    QColor(0x98,0xAB,0xC5),
    QColor(0x8A,0x89,0xA6),
    QColor(0x7B,0x68,0x88),
    QColor(0x6B,0x48,0x6B),
    QColor(0xA0,0x5D,0x56),
    QColor(0xD0,0x74,0x3C),
    QColor(0xFF,0x8C,0x00),
  };

  static uint num_colors = 7;

  double angle1 = 90.0;

  QModelIndex ind;

  int n = model_->rowCount(ind);

  double total = 0.0;

  for (int i = 0; i < n; ++i) {
    QModelIndex yind = model_->index(i, yColumn_);

    double value = CQChartsUtil::toReal(model_->data(yind));

    total += value;
  }

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model_->index(i, xColumn_);
    QModelIndex yind = model_->index(i, yColumn_);

    QString name  = model_->data(xind).toString();
    double  value = CQChartsUtil::toReal(model_->data(yind));

    double angle  = 360.0*value/total;
    double angle2 = angle1 - angle;

    QPainterPath path;

    if (donut_) {
      double dwidth  = 0.4;
      double dwidth1 = (1 - dwidth);

      double a1 = angle1*M_PI/180.0;
      double a2 = angle2*M_PI/180.0;

      double x1 = xc + dwidth1*r*cos(a1);
      double y1 = yc - dwidth1*r*sin(a1);
      double x2 = xc + 1.0    *r*cos(a1);
      double y2 = yc - 1.0    *r*sin(a1);

      double x3 = xc + dwidth1*r*cos(a2);
      double y3 = yc - dwidth1*r*sin(a2);
    //double x4 = xc + 1.0    *r*cos(a2);
    //double y4 = yc - 1.0    *r*sin(a2);

      path.moveTo(QPointF(x1, y1));
      path.lineTo(QPointF(x2, y2));

      path.arcTo(QRectF(xc - r, yc - r, 2.0*r, 2.0*r), angle1, -angle);

      //path.lineTo(QPointF(x4, y4));
      path.lineTo(QPointF(x3, y3));

      path.arcTo(QRectF(xc - dwidth1*r, yc - dwidth1*r, dwidth1*r*2.0, dwidth1*r*2.0),
                 angle2, angle);
    }
    else {
      path.moveTo(QPointF(xc, yc));

      path.arcTo(QRectF(xc - r, yc - r, 2.0*r, 2.0*r), angle1, -angle);
    }

    path.closeSubpath();

    p.setPen(QColor(255,255,255));

    if (palette()) {
      CRGBA rgba = palette()->getColor((1.0*i)/(n - 1)).rgba();

      p.setBrush(CQUtil::toQColor(rgba));
    }
    else
      p.setBrush(colors[i % num_colors]);

    p.drawPath(path);

    angle1 = angle2;
  }

  angle1 = M_PI/2.0;

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model_->index(i, xColumn_);
    QModelIndex yind = model_->index(i, yColumn_);

    QString name  = model_->data(xind).toString();
    double  value = CQChartsUtil::toReal(model_->data(yind));

    double angle = 2.0*M_PI*value/total;

    double tangle = angle1 - angle/2;

    double dwidth1 = (donut_ ? 0.6 : 0.0);
    double dwidth2 = 1.0;

    double x = xc + 0.5*(dwidth1 + dwidth2)*r*cos(tangle);
    double y = yc - 0.5*(dwidth1 + dwidth2)*r*sin(tangle);

    int tw = fm.width(name);

    p.setPen(QColor(0,0,0));

    p.drawText(x - tw/2, y + fm.ascent(), name);

    angle1 -= angle;
  }
}
