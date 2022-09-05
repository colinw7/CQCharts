#include <CQChartsColumnsEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsPlot.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQIconButton.h>
#include <CQTclUtil.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

CQChartsColumnsLineEdit::
CQChartsColumnsLineEdit(QWidget *parent, bool basic) :
 CQChartsLineEditBase(parent), basic_(basic)
{
  setObjectName("columnsLineEdit");

  setToolTip("Columns");

  //---

  menuEdit_ = dataEdit_ = new CQChartsColumnsEdit(nullptr, basic);

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);
}

void
CQChartsColumnsLineEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsLineEditBase::setPlot(plot);

  if (plot)
    setModelData(plot->getModelData());
}

CQChartsModelData *
CQChartsColumnsLineEdit::
modelData() const
{
  return modelData_;
}

void
CQChartsColumnsLineEdit::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

  dataEdit_->setModelData(modelData_);
}

const CQChartsColumns &
CQChartsColumnsLineEdit::
columns() const
{
  return dataEdit_->columns();
}

void
CQChartsColumnsLineEdit::
setColumns(const CQChartsColumns &columns)
{
  updateColumns(columns, /*updateText*/true);
}

void
CQChartsColumnsLineEdit::
setBasic(bool b)
{
  basic_ = b;

  dataEdit_->setBasic(b);

  columnsToWidgets();
}

void
CQChartsColumnsLineEdit::
setProxy(bool b)
{
  proxy_ = b;

  columnsToWidgets();
}

void
CQChartsColumnsLineEdit::
setNumericOnly(bool b)
{
  numericOnly_ = b;

  dataEdit_->setNumericOnly(b);
}

void
CQChartsColumnsLineEdit::
updateColumns(const CQChartsColumns &columns, bool updateText)
{
  connectSlots(false);

  dataEdit_->setColumns(columns);

  connectSlots(true);

  if (updateText)
    columnsToWidgets();

  Q_EMIT columnsChanged();
}

void
CQChartsColumnsLineEdit::
textChanged()
{
  CQChartsColumns columns;

  if (! textToColumns(edit_->text(), columns))
    return;

  updateColumns(columns, /*updateText*/ false);
}

void
CQChartsColumnsLineEdit::
columnsToWidgets()
{
  connectSlots(false);

  QString text;

  if (columns().isValid()) {
    if (modelData()) {
      auto *model = modelData()->currentModel(isProxy()).data();

      bool ok;
      text = CQChartsModelUtil::columnsToString(model, columns(), ok);
    }
    else
      text = columns().toString();
  }

  edit_->setText(text);

  auto tip = QString("%1 (%2)").arg(toolTip()).arg(columns().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsColumnsLineEdit::
menuEditChanged()
{
  updateMenu();

  columnsToWidgets();

  Q_EMIT columnsChanged();
}

void
CQChartsColumnsLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(columnsChanged()), this, SLOT(menuEditChanged()));
}

bool
CQChartsColumnsLineEdit::
textToColumns(const QString &str, CQChartsColumns &columns) const
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  bool ok = true;

  for (int i = 0; i < strs.size(); ++i) {
    const auto &str = strs[i];

    if (str.left(1) != "(") {
      // support column numeric range <n>-<m>
      auto strs1 = str.split("-", Qt::SkipEmptyParts);

      if (strs1.size() == 2) {
        bool ok1, ok2;

        long startCol = CQChartsUtil::toInt(strs1[0], ok1);
        long endCol   = CQChartsUtil::toInt(strs1[1], ok2);

        if (ok1 && ok2) {
          for (long col = startCol; col <= endCol; ++col)
            columns.addColumn(CQChartsColumn(int(col)));
        }
        else
          ok = false;
      }
      else {
        CQChartsColumn col;

        if (modelData()) {
          auto *model = modelData()->currentModel(isProxy()).data();

          if (! CQChartsModelUtil::stringToColumn(model, str, col))
            col = CQChartsColumn();
        }
        else
          col = CQChartsColumn(str);

        if (col.isValid())
          columns.addColumn(col);
        else
          ok = false;
      }
    }
    else {
      CQChartsColumn col;

      if (modelData()) {
        auto *model = modelData()->currentModel(isProxy()).data();

        if (! CQChartsModelUtil::stringToColumn(model, str, col))
          col = CQChartsColumn();
      }
      else
        col = CQChartsColumn(str);

      if (col.isValid())
        columns.addColumn(col);
      else
        ok = false;
    }
  }

  return ok;
}

void
CQChartsColumnsLineEdit::
updateMenu()
{
  CQChartsLineEditBase::updateMenu();

  //---

  auto s = dataEdit_->sizeHint();

  int w = std::max(this->width(), s.width());
  int h = s.height();

  //---

  dataEdit_->setFixedSize(w, h);

  int bw = 2;

  menu_->setFixedSize(QSize(w + 2*bw, h + 2*bw));

  menu_->updateAreaSize();
}

void
CQChartsColumnsLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  auto str = (columns().isValid() ? columns().toString() : "<none>");

  drawCenteredText(painter, str);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsColumnsPropertyViewType::
CQChartsColumnsPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsColumnsPropertyViewType::
getEditor() const
{
  return new CQChartsColumnsPropertyViewEditor;
}

bool
CQChartsColumnsPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsColumnsPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  auto str = valueString(item, value, ok);

  auto font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.horizontalAdvance(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsColumnsPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsColumnsPropertyViewType::
valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const
{
  auto columns = CQChartsColumns::fromVariant(value);

  QString str;

  if (columns.isValid()) {
    str = columns.toString();

    auto *plot = (item ? qobject_cast<CQChartsPlot *>(item->object()) : nullptr);

    if (plot) {
      QString str1;

      for (const auto &column : columns.columns()) {
        auto headerName = plot->columnHeaderName(column);

        if (headerName.length()) {
          if (str1.length())
            str1 += ", ";

          str1 += headerName;
        }
      }

      if (str1.length())
        str += " (" + str1 + ")";
    }

    ok = true;
  }
  else {
    str = "<none>";
    ok  = false;
  }

  return str;
}

//------

CQChartsColumnsPropertyViewEditor::
CQChartsColumnsPropertyViewEditor()
{
}

QWidget *
CQChartsColumnsPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  auto *plot = qobject_cast<CQChartsPlot *>(obj);

  //---

  auto *edit = new CQChartsColumnsLineEdit(parent);

  if (plot)
    edit->setModelData(plot->getModelData());

  return edit;
}

void
CQChartsColumnsPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnsChanged()), obj, method);
}

QVariant
CQChartsColumnsPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  return CQChartsColumns::toVariant(edit->columns());
}

void
CQChartsColumnsPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  auto columns = CQChartsColumns::fromVariant(var);

  edit->setColumns(columns);
}

//------

CQChartsColumnsEdit::
CQChartsColumnsEdit(QWidget *parent, bool basic) :
 CQChartsEditBase(parent), basic_(basic)
{
  setObjectName("columnsEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  controlFrame_ = CQUtil::makeWidget<QFrame>("controlFrame");

  auto *controlFrameLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame_, 2, 2);

  countLabel_ = CQUtil::makeLabelWidget<QLabel>("", "countLabel");

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    auto *button = CQUtil::makeWidget<CQIconButton>(name);

    button->setIcon(iconName);

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  auto *addButton    = createButton("add"   , "ADD"   , "Add column"   , SLOT(addSlot()));
  auto *removeButton = createButton("remove", "REMOVE", "Remove column", SLOT(removeSlot()));

  controlFrameLayout->addWidget(countLabel_);
  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  layout->addWidget(controlFrame_);

  columnsFrame_ = CQUtil::makeWidget<QFrame>("columnsFrame");

  (void) CQUtil::makeLayout<QVBoxLayout>(columnsFrame_, 2, 2);

  layout->addWidget(columnsFrame_);

  layout->addStretch(1);

  //---

  updateEdits();
}

void
CQChartsColumnsEdit::
setModelData(CQChartsModelData *modelData)
{
  connectSlots(false);

  if (modelData_)
    disconnect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

  modelData_ = modelData;

  if (modelData_)
    connect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

  if (isBasic()) {
    auto ne = columnCombos_.size();

    for (size_t i = 0; i < ne; ++i) {
      columnCombos_[i]->setProxy(isProxy());
      columnCombos_[i]->setModelData(modelData_);
    }
  }
  else {
    auto ne = columnEdits_.size();

    for (size_t i = 0; i < ne; ++i) {
      columnEdits_[i]->setProxy(isProxy());
      columnEdits_[i]->setModelData(modelData_);
    }
  }

  connectSlots(true);
}

void
CQChartsColumnsEdit::
resetModelData()
{
  modelData_ = nullptr;
}

const CQChartsColumns &
CQChartsColumnsEdit::
columns() const
{
  return columns_;
}

void
CQChartsColumnsEdit::
setColumns(const CQChartsColumns &columns)
{
  columns_ = columns;

  columnsToWidgets();

  Q_EMIT columnsChanged();
}

void
CQChartsColumnsEdit::
setBasic(bool b)
{
  basic_ = b;

  columnsToWidgets();
}

void
CQChartsColumnsEdit::
setProxy(bool b)
{
  proxy_ = b;

  if (isBasic()) {
    auto ne = columnCombos_.size();

    for (size_t i = 0; i < ne; ++i) {
      columnCombos_[i]->setProxy(isProxy());
      columnCombos_[i]->setModelData(modelData_);
    }
  }
  else {
    auto ne = columnEdits_.size();

    for (size_t i = 0; i < ne; ++i) {
      columnEdits_[i]->setProxy(isProxy());
      columnEdits_[i]->setModelData(modelData_);
    }
  }

  columnsToWidgets();
}

void
CQChartsColumnsEdit::
setNumericOnly(bool b)
{
  numericOnly_ = b;

  if (isBasic()) {
    auto ne = columnCombos_.size();

    for (size_t i = 0; i < ne; ++i)
      columnCombos_[i]->setNumericOnly(b);
  }
  else {
    auto ne = columnEdits_.size();

    for (size_t i = 0; i < ne; ++i)
      columnEdits_[i]->setNumericOnly(b);
  }
}

void
CQChartsColumnsEdit::
columnsToWidgets()
{
  updateEdits();

  //---

  connectSlots(false);

  int n = columns_.count();

  if (isBasic()) {
    auto ne = columnCombos_.size();
    assert(size_t(n) == ne);

    for (size_t i = 0; i < ne; ++i)
      columnCombos_[i]->setColumn(columns_.getColumn(int(i)));
  }
  else {
    auto ne = columnEdits_.size();
    assert(size_t(n) == ne);

    for (size_t i = 0; i < ne; ++i)
      columnEdits_[i]->setColumn(columns_.getColumn(int(i)));
  }

  connectSlots(true);
}

void
CQChartsColumnsEdit::
widgetsToColumn()
{
  auto columns = columns_;

  int n = columns_.count();

  if (isBasic()) {
    auto ne = columnCombos_.size();
    assert(size_t(n) == ne);

    for (size_t i = 0; i < size_t(n); ++i)
      columns_.setColumn(int(i), columnCombos_[i]->getColumn());
  }
  else {
    auto ne = columnEdits_.size();
    assert(size_t(n) == ne);

    for (size_t i = 0; i < size_t(n); ++i)
      columns_.setColumn(int(i), columnEdits_[i]->column());
  }

  if (columns_ != columns)
    Q_EMIT columnsChanged();
}

void
CQChartsColumnsEdit::
addSlot()
{
  columns_.addColumn(CQChartsColumn());

  columnsToWidgets();

  Q_EMIT columnsChanged();
}

void
CQChartsColumnsEdit::
removeSlot()
{
  columns_.removeColumn();

  columnsToWidgets();

  Q_EMIT columnsChanged();
}

void
CQChartsColumnsEdit::
updateEdits()
{
  connectSlots(false);

  auto n = size_t(columns_.count());

  if (isBasic()) {
    CQChartsUtil::makeArraySize(columnCombos_, n, [&]() {
      auto *combo = new CQChartsColumnCombo;

      combo->setModelData(modelData());

      qobject_cast<QVBoxLayout *>(columnsFrame_->layout())->addWidget(combo);

      return combo;
    } );

    while (! columnEdits_.empty()) {
      delete columnEdits_.back();

      columnEdits_.pop_back();
    }
  }
  else {
    CQChartsUtil::makeArraySize(columnEdits_, n, [&]() {
      auto *edit = new CQChartsColumnLineEdit;

      edit->setModelData(modelData());

      qobject_cast<QVBoxLayout *>(columnsFrame_->layout())->addWidget(edit);

      return edit;
    } );

    while (! columnCombos_.empty()) {
      delete columnCombos_.back();

      columnCombos_.pop_back();
    }
  }

  countLabel_->setText(QString("%1 Columns").arg(n));

  connectSlots(true);
}

void
CQChartsColumnsEdit::
connectSlots(bool b)
{
  if (isBasic()) {
    for (auto &edit : columnCombos_)
      CQChartsWidgetUtil::connectDisconnect(b,
        edit, SIGNAL(columnChanged()), this, SLOT(widgetsToColumn()));
  }
  else {
    for (auto &edit : columnEdits_)
      CQChartsWidgetUtil::connectDisconnect(b,
        edit, SIGNAL(columnChanged()), this, SLOT(widgetsToColumn()));
  }
}

QSize
CQChartsColumnsEdit::
sizeHint() const
{
  auto s1 = controlFrame_->sizeHint();

  int w1 = s1.width();
  int h1 = s1.height() + 2;

  if (isBasic()) {
    auto ne = columnCombos_.size();

    for (uint i = 0; i < ne; ++i) {
      auto s2 = columnCombos_[i]->sizeHint();

      w1  = std::max(w1, s2.width());
      h1 += s2.height() + 2;
    }
  }
  else {
    auto ne = columnEdits_.size();

    for (uint i = 0; i < ne; ++i) {
      auto s2 = columnEdits_[i]->sizeHint();

      w1  = std::max(w1, s2.width());
      h1 += s2.height() + 2;
    }
  }

  return QSize(w1 + 4, h1 + 2);
}
