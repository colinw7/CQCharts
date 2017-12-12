#include <CQChartsDataLabel.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

CQChartsDataLabel::
CQChartsDataLabel(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
}

void
CQChartsDataLabel::
addProperties(const QString &path)
{
  plot_->addProperty(path, this, "visible" );
  plot_->addProperty(path, this, "position");
  plot_->addProperty(path, this, "clip"    );
  plot_->addProperty(path, this, "angle"   );
  plot_->addProperty(path, this, "font"    );
  plot_->addProperty(path, this, "color"   );

  QString boxPath = plot_->id() + "/" + path + "/box";

  CQChartsBoxObj::addProperties(plot_->propertyModel(), boxPath);
}

void
CQChartsDataLabel::
update()
{
  plot_->update();
}

void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr)
{
  if (! isVisible())
    return;

  painter->save();

  painter->setFont(font());

  QFontMetricsF fm(painter->font());

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  if (CQChartsUtil::isZero(angle())) {
    double tw = fm.width(ystr);

    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();

      if (! plot_->isInvertY())
        y = qrect.top() + fm.ascent() + ym + b2;
      else
        y = qrect.bottom() - fm.descent() - ym - b2;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();

      if (! plot_->isInvertY())
        y = qrect.top() - fm.descent() - ym - b2;
      else
        y = qrect.bottom() + fm.ascent() + ym + b2;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();

      if (! plot_->isInvertY())
        y = qrect.bottom() - fm.descent() - ym - b2;
      else
        y = qrect.top() + fm.ascent() + ym + b2;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();

      if (! plot_->isInvertY())
        y = qrect.bottom() + fm.ascent() + ym + b2;
      else
        y = qrect.top() - fm.descent() - ym - b2;
    }
    else if (position() == Position::CENTER) {
      x = qrect.center().x();
      y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    bool clipped = false;

    if (isClip()) {
      if (tw >= qrect.width())
        clipped = true;
    }

    if (isClip()) {
      if (position() == Position::TOP_INSIDE ||
          position() == Position::BOTTOM_INSIDE ||
          position() == Position::CENTER) {
        painter->setClipRect(qrect);
      }
    }

    QRectF brect(x - tw/2 - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);

    CQChartsBoxObj::draw(painter, brect);

    if (! clipped) {
      painter->setPen(interpColor(0, 1));

      if (ystr.length())
        painter->drawText(QPointF(x - tw/2, y), ystr);
    }
  }
  else {
    double x = 0.0, y = 0.0;

    Qt::Alignment align;

    if      (position() == Position::TOP_INSIDE) {
      x     = qrect.center().x();
      y     = qrect.top() + 2*b1;
      align = Qt::AlignHCenter | Qt::AlignTop;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x     = qrect.center().x();
      y     = qrect.top() - 2*b1;
      align = Qt::AlignHCenter | Qt::AlignBottom;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x     = qrect.center().x();
      y     = qrect.bottom() - 2*b1;
      align = Qt::AlignHCenter | Qt::AlignBottom;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x     = qrect.center().x();
      y     = qrect.bottom() + 2*b1;
      align = Qt::AlignHCenter | Qt::AlignTop;
    }
    else if (position() == Position::CENTER) {
      x     = qrect.center().x();
      y     = qrect.center().y();
      align = Qt::AlignHCenter | Qt::AlignVCenter;
    }

    QRectF                      bbox;
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(x, y, ystr, painter->font(), angle(), b1,
                                  bbox, points, align, /*alignBBox*/ true);

    painter->setPen(interpColor(0, 1));

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);

    painter->setPen(interpColor(0, 1));

    if (ystr.length())
      CQChartsRotatedText::drawRotatedText(painter, x, y, ystr, angle(), align,
                                           /*alignBBox*/ true);
  }

  painter->restore();
}
