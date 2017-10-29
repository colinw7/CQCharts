#include <CQIconCombo.h>

#include <QAbstractItemView>
#include <QAbstractItemDelegate>
#include <QListView>
#include <QStylePainter>
#include <QLayout>

#include <cassert>
#include <iostream>

class CQIconComboDelegate : public QAbstractItemDelegate {
 public:
  CQIconComboDelegate(CQIconCombo *combo);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

 private:
  CQIconCombo *combo_ { nullptr };
};

//---

class CQIconComboModel : public QAbstractListModel {
 public:
  CQIconComboModel(CQIconCombo *combo) :
   combo_(combo) {
  }

 ~CQIconComboModel() { }

  int rowCount(const QModelIndex &) const { return data_.size(); }

  // icon, text
  int columnCount(const QModelIndex &) const { return 2; }

  QVariant data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();

    if (row < 0 || col < 0) return QVariant();

    if      (role == Qt::DisplayRole || role == Qt::DecorationRole) {
      if      (col == 0)
        return QVariant(data_[row].icon);
      else if (col == 1)
        return QVariant(data_[row].text);
      else
        return QVariant();
    }
#if 0
    else if (role == Qt::ToolTipRole)
      return QVariant(data_[row].text);
#endif
    else if (role == Qt::SizeHintRole) {
      int is = combo_->style()->pixelMetric(QStyle::PM_SmallIconSize);

      if      (col == 0) {
        return QVariant(QSize(is + 4, is + 4));
      }
      else if (col == 1) {
        QString str = data_[row].text;

        QFont font = combo_->font();

        QFontMetrics fm(font);

        int width  = fm.width(str);
        int height = std::max(fm.height() + 2, is + 4);

        return QSize(width, height);
      }
      else
        return QVariant();
    }
    else if (role == Qt::BackgroundRole)
      return QVariant(QColor(255,255,255));
    else if (role == Qt::UserRole) {
      return data_[row].var;
    }
    else
      return QVariant();
  }

  void addValue(const QIcon &icon, const QString &text, const QVariant &var) {
    data_.push_back(IconText(icon, text, var));

    QModelIndex idx = index(data_.size() - 1, 0);

    emit dataChanged(idx, idx);
  }

  void clear() {
    data_.clear();

    QModelIndex idx = index(data_.size() - 1, 0);

    emit dataChanged(idx, idx);
  }

 private:
  struct IconText {
    QIcon    icon;
    QString  text;
    QVariant var;

    IconText(const QIcon &icon1, const QString &text1, const QVariant &var1) {
      icon = icon1;
      text = text1;
      var  = var1;
    }
  };

  typedef std::vector<IconText> Data;

  CQIconCombo *combo_ { nullptr };
  Data         data_;
};

//---------

CQIconCombo::
CQIconCombo(QWidget *parent) :
 QComboBox(parent)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQIconComboDelegate *delegate = new CQIconComboDelegate(this);

  setItemDelegate(delegate);

  //-----

  model_ = new CQIconComboModel(this);

  setModel(model_);

  setModelColumn(0);

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTip()));
}

void
CQIconCombo::
calcMenuTextWidth()
{
  // calc text width (for menu)
  QFontMetrics fm(font());

  int textWidth = 0;

  for (int i = 0; i < count(); ++i) {
    QModelIndex ind = model()->index(i, 1);

    QString text = model()->data(ind, Qt::DisplayRole).toString();

    textWidth = std::max(textWidth, fm.width(text));
  }

  setMenuTextWidth(textWidth);
}

void
CQIconCombo::
setMenuTextWidth(int w)
{
  textWidth_ = w;
}

void
CQIconCombo::
updateTip()
{
  int row = currentIndex();

  QString tip;

  if (row >= 0) {
    QModelIndex ind = model()->index(row, 1);

    tip = model()->data(ind, Qt::DisplayRole).toString();
  }

  setToolTip(tip);
}

void
CQIconCombo::
addItem(const QIcon &icon, const QString &text, const QVariant &var)
{
  assert(! icon.isNull());

  model_->addValue(icon, text, var);

  if (currentIndex() < 0)
    setCurrentIndex(0);

  setModel(model_);

  updateTip();
}

QVariant
CQIconCombo::
itemData(int row) const
{
  QModelIndex ind = model()->index(row, 0);

  return model()->data(ind, Qt::UserRole);
}

void
CQIconCombo::
showPopup()
{
  int border = 3;

  int iw = style()->pixelMetric(QStyle::PM_SmallIconSize);

  if (iconWidth() > 0)
    iw = iconWidth();

  if (menuTextWidth() < 0)
    calcMenuTextWidth();

  int w = menuTextWidth() + iw + 3*border;

  view()->setMinimumWidth(w);

  QComboBox::showPopup();
}

QSize
CQIconCombo::
sizeHint() const
{
  QFontMetrics fm(font());

  QStyleOptionComboBox opt;

  initStyleOption(&opt);

  QRect popupRect =
    style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  int is = style()->pixelMetric(QStyle::PM_SmallIconSize);

  QSize cs(popupRect.width() + is + 8,
    std::max(std::max(popupRect.width(), is + 8), fm.height() + 2));

  return cs;
}

QSize
CQIconCombo::
minimumSizeHint() const
{
  return sizeHint();
}

#if 0
QSize
CQIconCombo::
calcSize(bool includeText) const
{
  int w = 0;
  int h = 0;

  QStyleOptionViewItem item;

  int numRows = model()->rowCount();

  for (int row = 0; row < numRows; ++row) {
    QModelIndex ind1 = model()->index(row, 0);
    QSize s1 = itemDelegate()->sizeHint(item, ind1); // icon

    if (includeText) {
      QModelIndex ind2 = model()->index(row, 1);
      QSize s2 = itemDelegate()->sizeHint(item, ind2); // text

      w = std::max(w, s1.width() + s2.width());
      h = std::max(h, std::max(s1.height(), s2.height()));
    }
    else {
      w = std::max(w, s1.width());
      h = std::max(h, s1.height());
    }
  }

  return QSize(w, h);
}
#endif

void
CQIconCombo::
paintEvent(QPaintEvent *)
{
  //QComboBox::paintEvent(e);

  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focus rect and selected etc.
  QStyleOptionComboBox opt;

  initStyleOption(&opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the control (no text)
  opt.currentText = "";

  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);

  //------

  QRect popupRect =
    style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  // draw the icon
  QRect rect = opt.rect.adjusted(2, 2, -(popupRect.width() + 2), -2);

  painter.fillRect(rect, QColor(255,255,255));

  int border = 3;

  int x  = border;
  int yc = rect.y() + rect.height()/2;

  int is = style()->pixelMetric(QStyle::PM_SmallIconSize);

  int row = currentIndex();
  if (row < 0) return;

  QModelIndex ind = model()->index(row, 0);

  QIcon icon = model()->data(ind, Qt::DisplayRole).value<QIcon>();
  assert(! icon.isNull());

  QPixmap pm = icon.pixmap(QSize(rect.width(), is));
  assert(! pm.isNull());

  painter.drawPixmap(x, yc - is/2, pm);

  //------

  // calc text width (for menu)
  calcMenuTextWidth();
}

//---------

CQIconComboDelegate::
CQIconComboDelegate(CQIconCombo *combo) :
 QAbstractItemDelegate(combo), combo_(combo)
{
}

void
CQIconComboDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
  QAbstractItemModel *model = combo_->model();

  bool active = (opt.state & QStyle::State_Selected);

  // draw active background
  if (active)
    painter->fillRect(opt.rect, opt.palette.highlight());
  else {
    QColor c = model->data(index, Qt::BackgroundRole).value<QColor>();

    if (! c.isValid())
      c = opt.palette.window().color();

    painter->fillRect(opt.rect, c);
  }

  //---

  // only draw first column
  int row = index.row();
  int col = index.column();

  if (col != 0) return;

  //---

  // get icon data
  QModelIndex ind1 = model->index(row, 0);
  QModelIndex ind2 = model->index(row, 1);

  QIcon icon = model->data(ind1, Qt::DisplayRole).value<QIcon>();
  assert(! icon.isNull());

  QString text = model->data(ind2, Qt::DisplayRole).toString();

  //---

  int border = 3;

  // draw icon
  int yc = opt.rect.y() + opt.rect.height()/2;

  int x = opt.rect.x() + border;

  int iw = opt.rect.width() - combo_->menuTextWidth() - 3*border;
  int ih = combo_->style()->pixelMetric(QStyle::PM_SmallIconSize);

  QIcon::Mode iconMode = (active ? QIcon::Selected : QIcon::Normal);

  QPixmap pm = icon.pixmap(QSize(iw, ih), iconMode);
  assert(! pm.isNull());

  painter->drawPixmap(x, yc - ih/2, pm);

  // draw text
  x += iw + border;

  QFontMetrics fm(combo_->font());

  if (active)
    painter->setPen(opt.palette.highlightedText().color());
  else {
    QColor c = opt.palette.highlight().color();

    painter->setPen(c);
  }

  painter->drawText(x, yc - fm.height()/2 + fm.ascent(), text);
}

QSize
CQIconComboDelegate::
sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
  QAbstractItemModel *model = combo_->model();

  int row = index.row();
  int col = index.column();

  // only first column
  if (col != 0) return QSize();

  // get column sizes
  QModelIndex ind1 = model->index(row, 0);
  QModelIndex ind2 = model->index(row, 1);

  QSize s1 = model->data(ind1, Qt::SizeHintRole).value<QSize>();
  QSize s2 = model->data(ind2, Qt::SizeHintRole).value<QSize>();

  // calc size
  int border = 3;

  QSize sh(s1.width() + s2.width() + 3*border, std::max(s1.height(), s2.height()) + 2*border);

  return sh;
}
