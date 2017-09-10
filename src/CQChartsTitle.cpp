#include <CQChartsTitle.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQPropertyTree.h>
#include <CQUtil.h>
#include <QPainter>
#include <QRectF>

CQChartsTitle::
CQChartsTitle(CQChartsPlot *plot) :
 plot_(plot), text_("Title")
{
}

void
CQChartsTitle::
redraw()
{
  plot_->update();
}

QString
CQChartsTitle::
locationStr() const
{
  switch (location_) {
    case Location::TOP:    return "top";
    case Location::CENTER: return "center";
    case Location::BOTTOM: return "bottom";
    default:               return "none";
  }
}

void
CQChartsTitle::
setLocationStr(const QString &str)
{
  QString lstr = str.toLower();

  if      (lstr == "top"   ) location_ = Location::TOP;
  else if (lstr == "center") location_ = Location::CENTER;
  else if (lstr == "bottom") location_ = Location::BOTTOM;

  updatePosition();
}

void
CQChartsTitle::
updatePosition()
{
  plot_->updateTitlePosition();

  plot_->update();
}

void
CQChartsTitle::
addProperties(CQPropertyTree *tree, const QString &path)
{
  tree->addProperty(path, this, "visible" );
  tree->addProperty(path, this, "location");
  tree->addProperty(path, this, "inside"  );

  CQChartsBoxObj::addProperties(tree, path);

  QString textPath = path + "/text";

  tree->addProperty(textPath, this, "text" );
  tree->addProperty(textPath, this, "font" );
  tree->addProperty(textPath, this, "color");
}

QSizeF
CQChartsTitle::
calcSize()
{
  if (text().length()) {
    QFontMetrics fm(font());

    int pw = fm.width(text());
    int ph = fm.height();

    double ww = plot_->pixelToWindowWidth (pw);
    double wh = plot_->pixelToWindowHeight(ph);

    double xm = plot_->pixelToWindowWidth (margin_);
    double ym = plot_->pixelToWindowHeight(margin_);

    size_ = QSizeF(ww + 2*xm, wh + 2*ym);
  }
  else {
    size_ = QSizeF();
  }

  return size_;
}

bool
CQChartsTitle::
contains(const CPoint2D &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

void
CQChartsTitle::
draw(QPainter *p)
{
  if (! isVisible())
    return;

  if (! text().length())
     return;

  //---

  p->save();

  //---

  double x = position_.x(); // bottom
  double y = position_.y(); // top
  double w = size_.width ();
  double h = size_.height();

  //double xm = plot_->pixelToWindowWidth (margin_);
  //double ym = plot_->pixelToWindowHeight(margin_);

  bbox_ = CBBox2D(x, y, x + w, y + h);

  //---

  CBBox2D prect;

  plot_->windowToPixel(bbox_, prect);

  //---

  p->fillRect(CQUtil::toQRect(prect), QBrush(background()));

  if (isBorder()) {
    QPen pen(borderColor());

    pen.setWidth(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    p->drawRect(CQUtil::toQRect(prect));
  }

  //---

  QFontMetrics fm(font());

  p->setPen (color());
  p->setFont(font());

  p->drawText(prect.getXMin() + margin_, prect.getYMax() - margin_ - fm.descent(), text());

  //---

  if (plot_->showBoxes()) {
    CBBox2D prect;

    plot_->windowToPixel(bbox_, prect);

    p->setPen(Qt::red);
    p->setBrush(Qt::NoBrush);

    p->drawRect(CQUtil::toQRect(prect));
  }

  //---

  p->restore();
}
