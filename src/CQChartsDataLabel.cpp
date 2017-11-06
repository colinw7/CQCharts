#include <CQChartsDataLabel.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <QPainter>

CQChartsDataLabel::
CQChartsDataLabel(CQChartsPlot *plot) :
 plot_(plot)
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
redrawBoxObj()
{
  update();
}

void
CQChartsDataLabel::
update()
{
  plot_->update();
}

void
CQChartsDataLabel::
draw(QPainter *p, const QRectF &qrect, const QString &ystr)
{
  if (! isVisible())
    return;

  p->save();

  p->setFont(font());

  QFontMetricsF fm(p->font());

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  double b = b1 + b2;

  if (CQChartsUtil::isZero(angle())) {
    double tw = fm.width(ystr);

    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top() + fm.ascent() + ym;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() + fm.ascent() + ym;
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
        p->setClipRect(qrect, Qt::ReplaceClip);
      }
    }

    QRectF brect(x - tw/2 - b, y - fm.ascent() - b, tw + 2*b, fm.height() + 2*b);

    CQChartsBoxObj::draw(p, brect);

    if (! clipped) {
      p->setPen(color());

      p->drawText(x - tw/2, y, ystr);
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

    QRectF                bbox;
    CQRotatedText::Points points;

    CQRotatedText::bboxData(x, y, ystr, p->font(), angle(), b1,
                            bbox, points, align, /*alignBBox*/ true);

    p->setPen(color());

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(p, poly);

    p->setPen(color());

    CQRotatedText::drawRotatedText(p, x, y, ystr, angle(), align, /*alignBBox*/ true);
  }

  p->restore();
}
