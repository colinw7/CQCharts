#include <CQChartsEditBase.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsColor.h>
#include <CQCharts.h>

#include <CQWidgetUtil.h>

#include <QLabel>
#include <QGridLayout>
#include <QPainter>

CQChartsEditBase::
CQChartsEditBase(QWidget *parent) :
 QFrame(parent)
{
}

void
CQChartsEditBase::
drawCenteredText(QPainter *painter, const QString &text)
{
  auto c = palette().color(QPalette::Window);

//auto tc = CQChartsUtil::invColor(c);
  auto tc = CQChartsUtil::bwColor(c);

  painter->setPen(tc);

  QFontMetrics fm(font());

  int fa = fm.ascent();
  int fd = fm.descent();

  painter->drawText(rect().left() + 2, rect().center().y() + (fa - fd)/2, text);
}

void
CQChartsEditBase::
fixLabelWidth()
{
  auto labels = this->findChildren<QLabel *>();

  std::vector<QLabel *> visibleLabels;

  for (auto *label : labels) {
    if (label->isVisible())
      visibleLabels.push_back(label);
  }

  for (auto *label : visibleLabels)
    CQWidgetUtil::resetWidgetMinMaxWidth(label);

  int labelWidth = 0;

  for (auto *label : visibleLabels)
    labelWidth = std::max(labelWidth, label->sizeHint().width());

  for (auto *label : visibleLabels)
    CQWidgetUtil::setWidgetFixedWidth(label, labelWidth);
}

//------

CQChartsEditPreview::
CQChartsEditPreview(CQChartsEditBase *edit) :
 edit_(edit)
{
  setObjectName("preview");

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void
CQChartsEditPreview::
paintEvent(QPaintEvent *e)
{
  QFrame::paintEvent(e);

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  draw(&painter);
}

QColor
CQChartsEditPreview::
interpColor(CQChartsPlot *plot, CQChartsView *view, const CQChartsColor &color)
{
  QColor c;

  if      (plot)
    return plot->interpColor(color, CQChartsUtil::ColorInd());
  else if (view)
    return view->interpColor(color, CQChartsUtil::ColorInd());
  else
    return color.color();
}

QSize
CQChartsEditPreview::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.horizontalAdvance("XXXX"), fm.height() + 4);
}
