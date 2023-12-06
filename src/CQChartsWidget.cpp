#include <CQChartsWidget.h>
#include <CQPropertyView.h>
#include <CQUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsWidget, toString, fromString)

int CQChartsWidget::metaTypeId;

//---

void
CQChartsWidget::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsWidget);

  CQPropertyViewMgrInst->setUserName("CQChartsWidget", "symbol");
}

CQChartsWidget::
CQChartsWidget(QWidget *widget) :
 widget_(widget)
{
  updatePath();
}

CQChartsWidget::
CQChartsWidget(const QString &s)
{
  fromString(s);
}

QWidget *
CQChartsWidget::
widget() const
{
  return widget_;
}

void
CQChartsWidget::
setWidget(QWidget *widget)
{
  widget_ = widget;

  updatePath();
}

QWidget *
CQChartsWidget::
parent() const
{
  if (! widget_)
    return nullptr;

  return widget_->parentWidget();
}

void
CQChartsWidget::
setParent(QWidget *parent)
{
  if (! widget_)
    return;

  widget_->setParent(parent);

  updatePath();
}

bool
CQChartsWidget::
isVisible() const
{
  if (! widget_)
    return false;

  return widget_->isVisible();
}

void
CQChartsWidget::
setVisible(bool b)
{
  if (! widget_)
    return;

  widget_->setVisible(b);
}

int
CQChartsWidget::
width() const
{
  if (widget_)
    return widget_->width();

  return 100;
}

int
CQChartsWidget::
height() const
{
  if (widget_)
    return widget_->height();

  return 100;
}

QString
CQChartsWidget::
id() const
{
  if (widget_)
    return widget_->objectName();

  return "";
}

void
CQChartsWidget::
setId(const QString &id)
{
  if (widget_)
    return widget_->setObjectName(id);
}

QString
CQChartsWidget::
toString() const
{
  return path();
}

bool
CQChartsWidget::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    setWidget(WidgetP());
    return true;
  }

  auto *obj = CQUtil::nameToObject(str);

  auto *w = qobject_cast<QWidget *>(obj);
  if (! w) return false;

  setWidget(w);

  return true;
}

void
CQChartsWidget::
initSize()
{
  if (! widget_)
    return;

  widget_->resize(widget_->sizeHint());
}

void
CQChartsWidget::
move(double x, double y)
{
  if (widget_)
    widget_->move(int(x), int(y));
}

void
CQChartsWidget::
resize(double w, double h)
{
  if (widget_)
    widget_->resize(int(w), int(h));
}

void
CQChartsWidget::
updatePath()
{
  path_ = CQUtil::fullName(widget_);
}
