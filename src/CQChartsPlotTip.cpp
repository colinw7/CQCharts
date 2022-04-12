#include <CQChartsPlotTip.h>

#if 0
CQChartsPlotTipLabel::
CQChartsPlotTipLabel()
{
  setWordWrap(true);
}

void
CQChartsPlotTipLabel::
setText(const QString &text)
{
  QLabel::setText(text);

  updateSize();
}

void
CQChartsPlotTipLabel::
updateSize()
{
  setWordWrap(false);

  QFontMetrics fm(font());

  auto s = QLabel::sizeHint();

  int w = std::min(s.width(), fm.horizontalAdvance("X")*50);

  setMaximumWidth(w);

  setWordWrap(true);

  size_ = QLabel::sizeHint();

  setMaximumWidth(QWIDGETSIZE_MAX);

  resize(size_);
}
#endif

#if 0
CQChartsPlotTip::
CQChartsPlotTip() :
 CQToolTipIFace()
{
}

CQChartsPlotTip::
~CQChartsPlotTip()
{
  delete widget_;
}

void
CQChartsPlotTip::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;
}

QWidget *
CQChartsPlotTip::
showWidget(const QPoint &p)
{
  if (! widget_)
    widget_ = new CQChartsPlotTipLabel;

  updateWidget(p);

  return widget_;
}

void
CQChartsPlotTip::
hideWidget()
{
  delete widget_;

  widget_ = nullptr;
}

bool
CQChartsPlotTip::
updateWidget(const QPoint &)
{
  if (! widget_) return false;

  if (plot_.isNull()) return false;

  QString text;

  auto desc = plot_->type()->description();

  int pos = desc.indexOf("</h2>");

  if (pos > 0) {
    auto lhs = desc.mid(0, pos);
    auto rhs = desc.mid(pos);

    if (! expanded_)
      text = lhs + " (" + plot_->id() + ")</h2>\n<p>(<b>PageDown</b> to expand)</p>";
    else
      text = lhs + " (" + plot_->id() + ")" + rhs + "\n<p>(<b>PageUp</b> to collapse)</p>";
  }
  else
    text = plot_->id();

  widget_->setText(text);

  return true;
}

bool
CQChartsPlotTip::
isHideKey(int key, Qt::KeyboardModifiers mod) const
{
  if (key == Qt::Key_PageUp || key == Qt::Key_PageDown)
    return false;

  return CQToolTipIFace::isHideKey(key, mod);
}

bool
CQChartsPlotTip::
keyPress(int key, Qt::KeyboardModifiers mod)
{
  if (! expanded_) {
    if (key == Qt::Key_PageDown) {
      expanded_ = true;
      return true;
    }
  }
  else {
    if (key == Qt::Key_PageUp) {
      expanded_ = false;
      return true;
    }
  }

  return CQToolTipIFace::keyPress(key, mod);
}
#endif
