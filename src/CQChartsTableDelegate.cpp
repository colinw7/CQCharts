#include <CQChartsTableDelegate.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsModelView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQChartsSymbol.h>
#include <CQChartsImage.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQCharts.h>

#include <CQColorsPalette.h>

#include <QCheckBox>

CQChartsTableDelegate::
CQChartsTableDelegate(Table *table) :
 table_(table)
{
  init();
}

CQChartsTableDelegate::
CQChartsTableDelegate(Tree *tree) :
 tree_(tree)
{
  init();
}

CQChartsTableDelegate::
CQChartsTableDelegate(ModelView *view) :
 view_(view)
{
  init();
}

void
CQChartsTableDelegate::
init()
{
  //setClipping(false);
}

void
CQChartsTableDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (! drawType(painter, option, index))
    QItemDelegate::paint(painter, option, index);
}

bool
CQChartsTableDelegate::
drawType(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto getModelData = [&](const QModelIndex &index) {
    auto var = modelP()->data(index, Qt::EditRole);

    if (! var.isValid())
      var = modelP()->data(index, Qt::DisplayRole);

    return var;
  };

  //---

  ColumnData columnData;

  getColumnData(index, columnData);

  QString nullStr;

  if (columnData.details)
    nullStr = columnData.details->nullValue();

  //---

  auto isNullVar = [&](const QVariant &var) {
    return (nullStr != "" && var.toString() == nullStr);
  };

  //---

  auto type = (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  if      (type == CQBaseModelType::BOOLEAN) {
    // get bool
    auto var = modelP()->data(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    bool b = var.toBool();

    // draw checkbox
    drawCheckInside(painter, option, b, index);
  }
  else if (type == CQBaseModelType::COLOR) {
    // get color value
    auto var = getModelData(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    // remap if needed
    auto *columnTypeMgr = charts()->columnTypeMgr();

    const auto *colorType =
      dynamic_cast<const CQChartsColumnColorType *>(columnTypeMgr->getType(type));

    bool converted;

    auto cvar = colorType->userData(charts(), modelP().data(), CQChartsColumn(index.column()),
                                    var, columnData.details->typeData(), converted);

    // get color
    bool ok;
    auto c = CQChartsVariant::toColor(cvar, ok);
    if (! ok) return false;

    // draw
    drawColor(painter, option, c, index);
  }
  else if (type == CQBaseModelType::SYMBOL) {
    // get symbol value
    auto var = getModelData(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    // TODO: remap

    bool ok;

    auto symbol = CQChartsVariant::toSymbol(var, ok);
    if (! ok) return false;

    drawSymbol(painter, option, symbol, index);
  }
  else if (type == CQBaseModelType::IMAGE) {
    // get symbol value
    auto var = getModelData(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    bool ok;

    auto image = CQChartsVariant::toImage(var, ok);
    if (! ok) return false;

    drawImage(painter, option, image, index);
  }
  else if (type == CQBaseModelType::REAL ||
           type == CQBaseModelType::INTEGER ||
           type == CQBaseModelType::TIME) {
    // get model value (actual data if possible)
    auto var = getModelData(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    //---

    auto tableDrawType = (columnData.details ? columnData.details->tableDrawType() :
                           CQChartsModelColumnDetails::TableDrawType::NORMAL);

    if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::NORMAL)
      return false;

    //---

    bool ok;
    double r = var.toReal(&ok);
    if (! ok) return false;

    //---

    auto option1 = option;

    // align right for numeric
    if (type == CQBaseModelType::REAL || type == CQBaseModelType::INTEGER)
      option1.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

    auto initSelected = [&]() {
      auto c = option.palette.color(QPalette::Highlight);

      option1.palette.setColor(QPalette::Highlight, Qt::transparent);
      option1.palette.setColor(QPalette::HighlightedText, c);
      option1.palette.setColor(QPalette::Text, c);

      option1.state &= ~QStyle::State_Selected;
    };

    auto drawSelected = [&]() {
      painter->setPen(option.palette.color(QPalette::Highlight));

      painter->drawRect(option.rect);
    };

    auto realDisplayStr = [&](const QModelIndex &index, double r) {
      QString dstr;

      auto dvar = modelP()->data(index, Qt::DisplayRole);

      if (dvar.isValid())
        dstr = dvar.toString();

      if (dstr.trimmed() == "")
        dstr = QString::number(r);

      return dstr;
    };

    //---

    double min { 0.0 }, max { 0.0 }, norm { 0.0 };

    if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP ||
        tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
      // get min, max
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *columnType = columnTypeMgr->getType(type);

      auto minVar = columnType->minValue(columnData.details->nameValues());
      if (! minVar.isValid()) minVar = columnData.details->minValue();

      auto maxVar = columnType->maxValue(columnData.details->nameValues());
      if (! maxVar.isValid()) maxVar = columnData.details->maxValue();

      min = minVar.toReal(&ok);
      max = maxVar.toReal(&ok);

      norm = (max > min ? (r - min)/(max - min) : 0.0);
    }

    //---

    if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP) {
      // calc background color
      auto bg = columnData.details->
        heatmapColor(r, min, max, option.palette.color(QPalette::Window));

      // draw background
      painter->fillRect(option.rect, bg);

      if (option.state & QStyle::State_Selected)
        initSelected();
      else {
        auto tc = CQChartsUtil::bwColor(bg);

        option1.palette.setColor(QPalette::Text, tc);
      }

      // draw value
      auto dstr = realDisplayStr(index, r);

      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      // draw selection/focus
      if (option.state & QStyle::State_Selected)
        drawSelected();

      QItemDelegate::drawFocus(painter, option, option.rect);
    }
    else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
      // draw bar for normalized value
      double lw =      norm *option.rect.width();
      double rw = (1 - norm)*option.rect.width();

      auto bg1 = columnData.details->barchartColor();
      auto bg2 = option.palette.color(QPalette::Window);

      QRectF lrect(option.rect.left()     , option.rect.top(), lw, option.rect.height());
      QRectF rrect(option.rect.left() + lw, option.rect.top(), rw, option.rect.height());

      painter->fillRect(lrect.adjusted(1, 1, -1, -1), bg1);
      painter->fillRect(rrect.adjusted(1, 1, -1, -1), bg2);

      //---

      // draw text (may need two colors so draw twice with clip)
      QColor tc1, tc2;

      if (option.state & QStyle::State_Selected) {
        initSelected();

        tc1 = option1.palette.color(QPalette::Text);
        tc2 = tc1;
      }
      else {
        tc1 = CQChartsUtil::bwColor(bg1);
        tc2 = CQChartsUtil::bwColor(bg2);
      }

      painter->save();

      auto dstr = realDisplayStr(index, r);

      painter->setClipRect(lrect);
      option1.palette.setColor(QPalette::Text, tc1);
      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      painter->setClipRect(rrect);
      option1.palette.setColor(QPalette::Text, tc2);
      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      painter->restore();

      // draw selection/focus
      if (option.state & QStyle::State_Selected)
        drawSelected();

      QItemDelegate::drawFocus(painter, option, option.rect);
    }
    else {
      return false;
    }
  }
  else {
    // handle list
    auto var = modelP()->data(index);

    if (isNullVar(var))
      return drawNullValue(painter, option, index);

    if (var.type() == QVariant::List) {
      QString str;

      CQChartsVariant::toString(var, str);

      drawString(painter, option, str, index);
    }
    else
      return false;
  }

  return true;
}

QWidget *
CQChartsTableDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &item, const QModelIndex &index) const
{
  ColumnData columnData;

  getColumnData(index, columnData);

  auto type = (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  if (type == CQBaseModelType::BOOLEAN) {
    auto var = modelP()->data(index);

    auto *check = CQUtil::makeLabelWidget<QCheckBox>(parent, "", "check");

    check->setChecked(var.toBool());

    check->setText(check->isChecked() ? "true" : "false");

    check->setAutoFillBackground(true);
    //check->setLayoutDirection(Qt::RightToLeft);

    connect(check, SIGNAL(stateChanged(int)), this, SLOT(updateBoolean()));

    currentIndex_ = index;

    return check;
  }
  else {
    return QItemDelegate::createEditor(parent, item, index);
  }
}

void
CQChartsTableDelegate::
click(const QModelIndex &index) const
{
  ColumnData columnData;

  getColumnData(index, columnData);

  auto type = (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  if (type == CQBaseModelType::BOOLEAN) {
    auto var = modelP()->data(index);

    modelP()->setData(index, ! var.toBool());
  }
}

void
CQChartsTableDelegate::
getColumnData(const QModelIndex &index, ColumnData &data) const
{
  auto p = columnDataMap_.find(index.column());

  if (p != columnDataMap_.end()) {
    data = (*p).second;
    return;
  }

  //---

  std::unique_lock<std::mutex> lock(mutex_);

  auto *details = getDetails();

  data.details = (details ? details->columnDetails(CQChartsColumn(index.column())) : nullptr);

  auto *th = const_cast<CQChartsTableDelegate *>(this);

  th->columnDataMap_[index.column()] = data;
}

void
CQChartsTableDelegate::
resetColumnData()
{
  std::unique_lock<std::mutex> lock(mutex_);

  auto *th = const_cast<CQChartsTableDelegate *>(this);

  th->columnDataMap_.clear();
}

void
CQChartsTableDelegate::
drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                bool checked, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  auto checkState = (checked ? Qt::Checked : Qt::Unchecked);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QItemDelegate::drawCheck(painter, option, rect, checkState);

  QFontMetrics fm(painter->font());

  int x = rect.right() + 4;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  //painter->drawText(x, y, (checked ? "true" : "false"));
  QItemDelegate::drawDisplay(painter, option, rect1, checked ? "true" : "false");
}

void
CQChartsTableDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const Color &color, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  auto c = charts()->interpColor(color, CQChartsUtil::ColorInd());

  painter->setBrush(QBrush(c));
  painter->setPen(QColor(Qt::black)); // TODO: contrast border

//painter->fillRect(rect, QBrush(c));
  painter->drawRect(rect);

//QFontMetrics fm(painter->font());

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  QItemDelegate::drawDisplay(painter, option, rect1, c.name());
}

void
CQChartsTableDelegate::
drawSymbol(QPainter *painter, const QStyleOptionViewItem &option,
           const Symbol &symbol, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(4, 4, -4, -4);

  painter->setPen(QColor(Qt::black)); // TODO: contrast border

  if (symbol.isValid()) {
    painter->setBrush(Qt::NoBrush);

    CQChartsPixelPaintDevice device(painter);

    auto symbolSize = CQChartsLength::pixel(rect.height()/2.0);

    if (symbol.isValid())
      CQChartsDrawUtil::drawSymbol(&device, symbol, CQChartsGeom::Point(rect.center()), symbolSize);
  }

  QFontMetrics fm(painter->font());

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  QItemDelegate::drawDisplay(painter, option, rect1, symbol.toString());
}

void
CQChartsTableDelegate::
drawImage(QPainter *painter, const QStyleOptionViewItem &option,
           const Image &image, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(4, 4, -4, -4);

  painter->setPen(QColor(Qt::black)); // TODO: contrast border

  if (image.isValid()) {
    image.resolve(charts());

    painter->setBrush(Qt::NoBrush);

    CQChartsPixelPaintDevice device(painter);

    CQChartsGeom::BBox pbbox(option.rect);

    device.drawImageInRect(device.pixelToWindow(pbbox), image, /*stretch*/false);
  }
}

bool
CQChartsTableDelegate::
drawNullValue(QPainter *painter, const QStyleOptionViewItem &option,
              const QModelIndex &index) const
{
  drawString(painter, option, "<null>", index);

  return true;
}

void
CQChartsTableDelegate::
drawString(QPainter *painter, const QStyleOptionViewItem &option, const QString &str,
           const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  QItemDelegate::drawDisplay(painter, option, option.rect, str);
}

void
CQChartsTableDelegate::
clearColumnTypes()
{
  columnDataMap_.clear();
}

void
CQChartsTableDelegate::
updateBoolean()
{
  auto *check = qobject_cast<QCheckBox *>(sender());
  assert(check);

  check->setText(check->isChecked() ? "true" : "false");

  modelP()->setData(currentIndex_, check->isChecked());
}

CQCharts *
CQChartsTableDelegate::
charts() const
{
  if (table_) return table_->charts();
  if (tree_ ) return tree_ ->charts();
  if (view_ ) return view_ ->charts();
  return nullptr;
}

CQChartsTableDelegate::ModelP
CQChartsTableDelegate::
modelP() const
{
  if (table_) return table_->modelP();
  if (tree_ ) return tree_ ->modelP();
  if (view_ ) return view_ ->modelP();
  return ModelP();
}

CQChartsModelDetails *
CQChartsTableDelegate::
getDetails() const
{
  if (table_) return table_->getDetails();
  if (tree_ ) return tree_ ->getDetails();
  if (view_ ) return view_ ->getDetails();
  return nullptr;
}
