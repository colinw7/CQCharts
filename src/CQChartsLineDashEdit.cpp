#include <CQChartsLineDashEdit.h>
#include <CQChartsUtil.h>
#include <CQChartsLineEdit.h>

#include <CQIconCombo.h>
#include <CQUtil.h>
#include <CQStyleMgr.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QIconEngine>

#if 0
#include <QProxyStyle>

class CQChartsLineDashEditProxyStyle : public QProxyStyle {
 public:
  CQChartsLineDashEditProxyStyle(int is) :
   is_(is) {
  }

  int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const {
    if      (metric == QStyle::PM_MenuHMargin  ) return 0;
    else if (metric == QStyle::PM_MenuVMargin  ) return 0;
    else if (metric == QStyle::PM_SmallIconSize) return 4*is_;

    return QProxyStyle::pixelMetric(metric, option, widget);
  }

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size,
                         const QWidget *widget) const {
    if (type == QStyle::CT_MenuItem)
      return QSize(5*is_ + size.width(), 2*is_);

    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }

 private:
  int is_;
};
#endif

// draw line icon pixmap
// TODO: cache pixmap ?
class CQChartsLineDashEditIconEngine : public QIconEngine {
 public:
  CQChartsLineDashEditIconEngine(const CQChartsLineDash &dash, bool bg) :
   dash_(dash), bg_(bg) {
  }

  QSize actualSize(const QSize & size, QIcon::Mode mode, QIcon::State state);

  QPixmap pixmap(const QSize & size, QIcon::Mode mode, QIcon::State state);

  void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);

  QIconEngine *clone() const { return new CQChartsLineDashEditIconEngine(dash_, bg_); }

 private:
  CQChartsLineDash dash_;
  bool             bg_ { true };
};

//------

CQChartsLineDashEdit::
CQChartsLineDashEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("lineDash");

  setToolTip("Line Dash");

  //---

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  // editable controls

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setToolTip("Line Dash\n(List of Dash Lengths)");

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(dashChangedSlot()));

  layout->addWidget(edit_);

  button_ = CQUtil::makeWidget<QToolButton>("button");

  menu_ = new QMenu(this);

  //menu_->setStyle(new CQChartsLineDashEditProxyStyle);

  button_->setMenu(menu_);

  connect(menu_, SIGNAL(triggered(QAction *)), this, SLOT(menuItemActivated(QAction *)));

  int is = CQStyleMgrInst->iconSize();

  button_->setPopupMode(QToolButton::InstantPopup);
  button_->setAutoRaise(true);
  button_->setFixedSize(QSize(is, is + 4));

  layout->addWidget(button_);

  //---

  // combo control

  combo_ = CQUtil::makeWidget<CQIconCombo>("combo");

  combo_->setIconWidth(5*is);

  combo_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout->addWidget(combo_);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboItemChanged()));

  //---

  addDashOption("solid" , CQChartsLineDash());
  addDashOption("1, 1"  , CQChartsLineDash("1, 1"));
  addDashOption("1, 2"  , CQChartsLineDash("1, 2"));
  addDashOption("1, 3"  , CQChartsLineDash("1, 3"));
  addDashOption("1, 4"  , CQChartsLineDash("1, 4"));
  addDashOption("1, 6"  , CQChartsLineDash("1, 6"));
  addDashOption("1, 8"  , CQChartsLineDash("1, 8"));
  addDashOption("1, 12" , CQChartsLineDash("1, 12"));
  addDashOption("1, 24" , CQChartsLineDash("1, 24"));
  addDashOption("1, 48" , CQChartsLineDash("1, 48"));
  addDashOption("2, 1"  , CQChartsLineDash("2, 1"));
  addDashOption("3, 1"  , CQChartsLineDash("3, 1"));
  addDashOption("4, 1"  , CQChartsLineDash("4, 1"));
  addDashOption("6, 1"  , CQChartsLineDash("6, 1"));
  addDashOption("8, 1"  , CQChartsLineDash("8, 1"));
  addDashOption("12, 1" , CQChartsLineDash("12, 1"));
  addDashOption("24, 1" , CQChartsLineDash("24, 1"));
  addDashOption("2, 2"  , CQChartsLineDash("2, 2"));
  addDashOption("3, 3"  , CQChartsLineDash("3, 3"));
  addDashOption("4, 4"  , CQChartsLineDash("4, 4"));
  addDashOption("6, 6"  , CQChartsLineDash("6, 6"));
  addDashOption("8, 8"  , CQChartsLineDash("8, 8"));
  addDashOption("12, 12", CQChartsLineDash("12, 12"));
  addDashOption("24, 24", CQChartsLineDash("24, 24"));
  addDashOption("2, 4"  , CQChartsLineDash("2, 4"));
  addDashOption("4, 2"  , CQChartsLineDash("4, 2"));
  addDashOption("2, 6"  , CQChartsLineDash("2, 6"));
  addDashOption("6, 2"  , CQChartsLineDash("6, 2"));
  addDashOption("4, 8"  , CQChartsLineDash("4, 8"));
  addDashOption("8, 4"  , CQChartsLineDash("8, 4"));

  addDashOption("2,1,0.5,1", CQChartsLineDash("2,1,0.5,1"));
  addDashOption("4,2,1,2"  , CQChartsLineDash("4,2,1,2"));
  addDashOption("8,2,1,2"  , CQChartsLineDash("8,2,1,2"));
//addDashOption("0.5,0.5"  , CQChartsLineDash("0.5,0.5"));
//addDashOption("0.25,0.25", CQChartsLineDash("0.25,0.25"));
//addDashOption("0.1,0.1"  , CQChartsLineDash("0.1,0.1"));

  //---

  updateState();
}

CQChartsLineDashEdit::
~CQChartsLineDashEdit()
{
  //delete menu_;
}

void
CQChartsLineDashEdit::
setEditable(bool edit)
{
  if (edit == editable_) return;

  editable_ = edit;

  updateState();
}

void
CQChartsLineDashEdit::
setLineDash(const CQChartsLineDash &dash)
{
  dash_ = dash;

  updateState();
}

void
CQChartsLineDashEdit::
updateState()
{
  edit_  ->setVisible(editable_);
  button_->setVisible(editable_);
  combo_ ->setVisible(! editable_);

  edit_->setText(dash_.toString());

  for (int i = 0; i < combo_->count(); ++i) {
    auto var = combo_->itemData(i);

    auto dash = var.value<CQChartsLineDash>();

    if (dash_ == dash) {
      if (i != combo_->currentIndex())
        combo_->setCurrentIndex(i);

      break;
    }
  }
}

void
CQChartsLineDashEdit::
dashChangedSlot()
{
  // line edit changed
  CQChartsLineDash dash;

  auto str = edit_->text();

  if (dash.fromString(str)) {
    dash_ = dash;

    emit valueChanged(dash_);
  }
}

void
CQChartsLineDashEdit::
menuItemActivated(QAction *action)
{
  // edit menu changed
  edit_->setText(action->text());

  dashChangedSlot();
}

void
CQChartsLineDashEdit::
comboItemChanged()
{
  // combo (non editable) changed
  int ind = combo_->currentIndex();

  auto var = combo_->itemData(ind);

  dash_ = var.value<CQChartsLineDash>();

  emit valueChanged(dash_);
}

void
CQChartsLineDashEdit::
addDashOption(const std::string &id, const CQChartsLineDash &dash)
{
  auto p = actions_.find(id);
  if (p != actions_.end()) return;

  auto icon = dashIcon(dash);

  auto *action = new CQChartsLineDashEditAction(this, id, dash, icon);

  actions_[id] = action;

  menu_->addAction(action);

  auto var = QVariant::fromValue(dash);

  combo_->addItem(icon, action->text(), var);
}

QIcon
CQChartsLineDashEdit::
dashIcon(const CQChartsLineDash &dash, bool bg)
{
  return QIcon(new CQChartsLineDashEditIconEngine(dash, bg));
}

//---

CQChartsLineDashEditAction::
CQChartsLineDashEditAction(CQChartsLineDashEdit *parent, const std::string &id,
                           const CQChartsLineDash &dash, const QIcon &icon) :
 QAction(parent), parent_(parent), id_(id), dash_(dash)
{
  setIcon(icon);
  setText(id_.c_str());

  setIconVisibleInMenu(true);
}

//---

QSize
CQChartsLineDashEditIconEngine::
actualSize(const QSize &size, QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
  return size;
}

QPixmap
CQChartsLineDashEditIconEngine::
pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
  QPixmap pixmap(size);

  pixmap.fill(QColor(0, 0, 0, 0));

  QPainter painter(&pixmap);

  paint(&painter, QRect(QPoint(0, 0), size), mode, state);

  painter.end();

  return pixmap;
}

void
CQChartsLineDashEditIconEngine::
paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
  painter->setRenderHints(QPainter::Antialiasing);

  QColor bg, fg;

  if      (state == QIcon::On) {
    bg = qApp->palette().highlight().color();
    fg = qApp->palette().highlightedText().color();
  }
  else if (mode == QIcon::Active || mode == QIcon::Selected) {
    bg = qApp->palette().highlight().color();
    fg = qApp->palette().highlightedText().color();
  }
  else {
    bg = qApp->palette().window().color();
    fg = qApp->palette().text().color();
  }

  if (bg_)
    painter->fillRect(rect, bg);

  QPen pen;

  pen.setColor(fg);
  pen.setWidthF(1.5);

  CQChartsUtil::penSetLineDash(pen, dash_);

  int y = rect.height()/2;

  painter->setPen(pen);

  painter->drawLine(2, y, rect.width() - 2, y);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsLineDashPropertyViewType::
CQChartsLineDashPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsLineDashPropertyViewType::
getEditor() const
{
  return new CQChartsLineDashPropertyViewEditor;
}

bool
CQChartsLineDashPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsLineDashPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto dash = value.value<CQChartsLineDash>();

  auto icon = CQChartsLineDashEdit::dashIcon(dash, /*bg*/false);

  auto str = dash.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto irect = option.rect;

  irect.setLeft(irect.left() + w + 2*margin());

  painter->drawPixmap(irect, icon.pixmap(option.rect.size()));

  //--

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsLineDashPropertyViewType::
tip(const QVariant &value) const
{
  auto str = value.value<CQChartsLineDash>().toString();

  return str;
}

//------

CQChartsLineDashPropertyViewEditor::
CQChartsLineDashPropertyViewEditor()
{
}

QWidget *
CQChartsLineDashPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsLineDashEdit(parent);

  return edit;
}

void
CQChartsLineDashPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsLineDashEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(const CQChartsLineDash &)), obj, method);
}

QVariant
CQChartsLineDashPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsLineDashEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getLineDash());
}

void
CQChartsLineDashPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsLineDashEdit *>(w);
  assert(edit);

  auto dash = var.value<CQChartsLineDash>();

  edit->setLineDash(dash);
}
