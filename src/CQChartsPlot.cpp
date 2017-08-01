#include <CQChartsPlot.h>
#include <CQPropertyTree.h>
#include <QPainter>

CQChartsPlot::
CQChartsPlot(QWidget *parent, QAbstractItemModel *model) :
 parent_(parent), model_(model)
{
  displayRange_.setWindowRange(0, 0, 1, 1);

  expander_ = new CQChartsPlotExpander(this);

  propertyTree_ = new CQPropertyTree(this);

  setContentsMargins(0, 0, expander_->width(), 0);
}

void
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyTree_->addProperty(path, object, name, alias);
}

void
CQChartsPlot::
resizeEvent(QResizeEvent *)
{
  updateGeometry();
}

void
CQChartsPlot::
updateGeometry()
{
  double m = std::min(width(), height())/10;

  displayRange_.setPixelRange(m, m, width() - m, height() - m);

  propertyTree_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    propertyTree_->move  (width() - propertyTree_->width() - expander_->width(), 0);
    propertyTree_->resize(propertyTree_->width(), height());
  }

  expander_->setVisible(true);

  expander_->move  (width() - expander_->width(), 0);
  expander_->resize(expander_->width(), height());
}

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py)
{
  displayRange_.windowToPixel(wx, wy, &px, &py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy)
{
  displayRange_.pixelToWindow(px, py, &wx, &wy);
}

QSize
CQChartsPlot::
sizeHint() const
{
  return QSize(500, 500);
}

//---

CQChartsPlotExpander::
CQChartsPlotExpander(CQChartsPlot *plot) :
 QFrame(plot), plot_(plot)
{
  setObjectName("expander");

  setFixedWidth(8);
}

void
CQChartsPlotExpander::
mousePressEvent(QMouseEvent *)
{
  setExpanded(! isExpanded());

  plot_->updateGeometry();
}

void
CQChartsPlotExpander::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int ym = height()/2;

  p.setBrush(QColor(0,0,0));

  int s = width();

  QPolygonF poly;

  if (! expanded_) {
    poly << QPoint(0, ym    );
    poly << QPoint(s, ym - s);
    poly << QPoint(s, ym + s);
  }
  else {
    poly << QPoint(s, ym    );
    poly << QPoint(0, ym - s);
    poly << QPoint(0, ym + s);
  }

  p.drawPolygon(poly);
}
