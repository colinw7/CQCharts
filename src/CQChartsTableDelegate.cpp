#include <CQChartsTableDelegate.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsModelView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQCharts.h>

#include <CQColorsPalette.h>

#include <QCheckBox>

CQChartsTableDelegate::
CQChartsTableDelegate(CQChartsTable *table) :
 table_(table) {
}

CQChartsTableDelegate::
CQChartsTableDelegate(CQChartsTree *tree) :
 tree_(tree) {
}

CQChartsTableDelegate::
CQChartsTableDelegate(CQChartsModelView *view) :
 view_(view) {
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
    QVariant var = modelP()->data(index, Qt::EditRole);

    if (! var.isValid())
      var = modelP()->data(index, Qt::DisplayRole);

    return var;
  };

  //---

  ColumnData columnData;

  getColumnData(index, columnData);

  CQBaseModelType type =
    (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  CQChartsModelColumnDetails::TableDrawType tableDrawType =
    (columnData.details ? columnData.details->tableDrawType() :
     CQChartsModelColumnDetails::TableDrawType::NORMAL);

  if      (type == CQBaseModelType::BOOLEAN) {
    // get bool
    QVariant var = modelP()->data(index);

    bool b = var.toBool();

    // draw checkbox
    drawCheckInside(painter, option, b, index);
  }
  else if (type == CQBaseModelType::COLOR) {
    // get color value
    QVariant var = getModelData(index);

    // remap if needed
    CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

    const CQChartsColumnColorType *colorType =
      dynamic_cast<const CQChartsColumnColorType *>(columnTypeMgr->getType(type));

    bool converted;

    QVariant cvar =
      colorType->userData(charts(), modelP().data(), index.column(),
                          var, columnData.details->nameValues(), converted);

    // get color
    bool ok;
    CQChartsColor c = CQChartsVariant::toColor(cvar, ok);
    if (! ok) return false;

    // draw
    drawColor(painter, option, c, index);
  }
  else if (type == CQBaseModelType::SYMBOL) {
    // get symbol value
    QVariant var = getModelData(index);

    // TODO: remap

    bool ok;

    CQChartsSymbol symbol = CQChartsVariant::toSymbol(var, ok);
    if (! ok) return false;

    drawSymbol(painter, option, symbol, index);
  }
  else if (type == CQBaseModelType::REAL ||
           type == CQBaseModelType::INTEGER ||
           type == CQBaseModelType::TIME) {
    // get model value (actual data if possible)
    QVariant var = getModelData(index);

    bool ok;
    double r = var.toReal(&ok);
    if (! ok) return false;

    //---

    QStyleOptionViewItem option1 = option;

    // align right for numeric
    if (type == CQBaseModelType::REAL || type == CQBaseModelType::INTEGER)
      option1.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

    auto initSelected = [&]() {
      QColor c = option.palette.color(QPalette::Highlight);

      option1.palette.setColor(QPalette::Highlight, QColor(0,0,0,0));
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

      QVariant dvar = modelP()->data(index, Qt::DisplayRole);

      if (dvar.isValid())
        dstr = dvar.toString();

      if (dstr.simplified() == "")
        dstr = QString("%1").arg(r);

      return dstr;
    };

    //---

    // get min, max
    CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

    const CQChartsColumnType *columnType = columnTypeMgr->getType(type);

    QVariant minVar = columnType->minValue(columnData.details->nameValues());
    if (! minVar.isValid()) minVar = columnData.details->minValue();

    QVariant maxVar = columnType->maxValue(columnData.details->nameValues());
    if (! maxVar.isValid()) maxVar = columnData.details->maxValue();

    double min = minVar.toReal(&ok);
    double max = maxVar.toReal(&ok);

    double norm = (max > min ? (r - min)/(max - min) : 0.0);

    //---

    const CQChartsColor      &drawColor = columnData.details->tableDrawColor();
    const CQChartsColorStops &drawStops = columnData.details->tableDrawStops();

    if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP) {
      QColor bg;

      if (drawColor.isValid()) {
        if (drawColor.isDirect()) {
          // blend fixed color with background color using normalized value
          QColor bg1 = charts()->interpColor(drawColor, 0, 1);
          QColor bg2 = option.palette.color(QPalette::Window);

          bg = CQChartsUtil::blendColors(bg1, bg2, norm);
        }
        else {
          bool hasStops = drawStops.isValid();
          bool relative = (hasStops ? drawStops.isPercent() : true);

          if (hasStops) {
            int ind = drawStops.ind(relative ? norm : r);

            CQChartsUtil::ColorInd colorInd(ind, drawStops.size() + 1);

            bg = charts()->interpColor(drawColor, colorInd);
          }
          else {
            // use interpolated color directly
            bg = charts()->interpColor(drawColor, norm);
          }
        }
      }
      else {
        // blend default color (red) with background color using normalized value
        QColor bg1 = QColor(255, 0, 0);
        QColor bg2 = option.palette.color(QPalette::Window);

        bg = CQChartsUtil::blendColors(bg1, bg2, norm);
      }

      //---

      // draw cell
      painter->fillRect(option.rect, bg);

      if (option.state & QStyle::State_Selected)
        initSelected();
      else {
        QColor tc = CQChartsUtil::bwColor(bg);

        option1.palette.setColor(QPalette::Text, tc);
      }

      QString dstr = realDisplayStr(index, r);

      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      if (option.state & QStyle::State_Selected)
        drawSelected();

      QItemDelegate::drawFocus(painter, option, option.rect);
    }
    else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
      // draw bar for normalized value
      double lw =      norm *option.rect.width();
      double rw = (1 - norm)*option.rect.width();

      QColor bg1(160, 160, 160);
      QColor bg2 = option.palette.color(QPalette::Window);

      if (drawColor.isValid())
        bg1 = charts()->interpColor(drawColor, 0, 1);

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

      QString dstr = realDisplayStr(index, r);

      painter->setClipRect(lrect);
      option1.palette.setColor(QPalette::Text, tc1);
      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      painter->setClipRect(rrect);
      option1.palette.setColor(QPalette::Text, tc2);
      QItemDelegate::drawDisplay(painter, option1, option.rect, dstr);

      painter->restore();

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
    QVariant var = modelP()->data(index);

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

  CQBaseModelType type =
    (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  if (type == CQBaseModelType::BOOLEAN) {
    QVariant var = modelP()->data(index);

    QCheckBox *check = CQUtil::makeLabelWidget<QCheckBox>(parent, "", "check");

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

  CQBaseModelType type =
    (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  if (type == CQBaseModelType::BOOLEAN) {
    QVariant var = modelP()->data(index);

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

  CQChartsModelDetails *details = getDetails();

  data.details = (details ? details->columnDetails(index.column()) : nullptr);

  CQChartsTableDelegate *th = const_cast<CQChartsTableDelegate *>(this);

  th->columnDataMap_[index.column()] = data;
}

void
CQChartsTableDelegate::
resetColumnData()
{
  std::unique_lock<std::mutex> lock(mutex_);

  CQChartsTableDelegate *th = const_cast<CQChartsTableDelegate *>(this);

  th->columnDataMap_.clear();
}

void
CQChartsTableDelegate::
drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                bool checked, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  Qt::CheckState checkState = (checked ? Qt::Checked : Qt::Unchecked);

  QRect rect = option.rect;

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
          const CQChartsColor &color, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QColor c = charts()->interpColor(color, CQChartsUtil::ColorInd());

  painter->fillRect(rect, QBrush(c));

  painter->setPen(QColor(0,0,0)); // TODO: contrast border

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
           const CQChartsSymbol &symbol, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(4, 4, -4, -4);

  painter->setPen(QColor(0,0,0)); // TODO: contrast border

  if (symbol.isValid()) {
    painter->setBrush(Qt::NoBrush);

    CQChartsPixelPainter device(painter);

    CQChartsLength symbolSize(rect.height(), CQChartsUnits::PIXEL);

    CQChartsDrawUtil::drawSymbol(&device, symbol, QPointF(rect.center()), symbolSize);
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
  QCheckBox *check = qobject_cast<QCheckBox *>(sender());
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
