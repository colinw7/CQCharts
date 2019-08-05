#include <CQChartsTableDelegate.h>
#include <CQChartsTable.h>
#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>

#include <CQColorsPalette.h>

#include <QCheckBox>
#include <QPainter>

CQChartsTableDelegate::
CQChartsTableDelegate(CQChartsTable *table) :
 table_(table) {
}

void
CQChartsTableDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  ColumnData columnData;

  getColumnData(index, columnData);

  CQBaseModelType type =
    (columnData.details ? columnData.details->type() : CQBaseModelType::STRING);

  CQChartsModelColumnDetails::TableDrawType tableDrawType =
    (columnData.details ? columnData.details->tableDrawType() :
     CQChartsModelColumnDetails::TableDrawType::NORMAL);

  if      (type == CQBaseModelType::BOOLEAN) {
    QVariant var = table_->modelP()->data(index);

    drawCheckInside(painter, option, var.toBool(), index);
  }
  else if (type == CQBaseModelType::COLOR) {
    QVariant var = table_->modelP()->data(index, Qt::EditRole);

    if (! var.isValid())
      var = table_->modelP()->data(index, Qt::DisplayRole);

    CQChartsColumnTypeMgr *columnTypeMgr = table_->charts()->columnTypeMgr();

    const CQChartsColumnColorType *colorType =
      dynamic_cast<const CQChartsColumnColorType *>(columnTypeMgr->getType(type));

    bool converted;

    QVariant cvar =
      colorType->userData(table_->charts(), table_->modelP().data(), index.column(),
                          var, columnData.details->nameValues(), converted);

    bool ok;

    CQChartsColor c = CQChartsVariant::toColor(cvar, ok);

    if (ok)
      drawColor(painter, option, c, index);
    else
      QItemDelegate::paint(painter, option, index);
  }
  else if (type == CQBaseModelType::SYMBOL) {
    QVariant var = table_->modelP()->data(index, Qt::EditRole);

    if (! var.isValid())
      var = table_->modelP()->data(index, Qt::DisplayRole);

    bool ok;

    CQChartsSymbol symbol = CQChartsVariant::toSymbol(var, ok);

    if (ok)
      drawSymbol(painter, option, symbol, index);
    else
      QItemDelegate::paint(painter, option, index);
  }
  else if (type == CQBaseModelType::REAL || type == CQBaseModelType::INTEGER) {
    QVariant var = table_->modelP()->data(index);

    bool ok;

    double r = var.toReal(&ok);

    if (ok) {
      QStyleOptionViewItem option1 = option;

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

      const CQChartsColumnType::ColorPalette &colorPalette =
        columnData.details->tableDrawPalette();

      double min = columnData.details->minValue().toReal(&ok);
      double max = columnData.details->maxValue().toReal(&ok);

      double norm = (max > min ? (r - min)/(max - min) : 0.0);

      if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::COL_HEATMAP) {
        QColor bg(255, 0, 0);

        if (colorPalette.palette) {
          bg = colorPalette.palette->getColor(norm);
        }
        else if (colorPalette.color.isValid()) {
          QColor bg1 = table_->charts()->interpColor(colorPalette.color, 0, 1);
          QColor bg2 = option.palette.color(QPalette::Window);

          bg = CQChartsUtil::blendColors(bg1, bg2, norm);
        }

        painter->fillRect(option.rect, bg);

        if (option.state & QStyle::State_Selected)
          initSelected();
        else {
          QColor tc = CQChartsUtil::bwColor(bg);

          option1.palette.setColor(QPalette::Text, tc);
        }

        QItemDelegate::drawDisplay(painter, option1, option.rect, QString("%1").arg(r));

        if (option.state & QStyle::State_Selected)
          drawSelected();

        QItemDelegate::drawFocus(painter, option, option.rect);
      }
      else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART) {
        double lw =      norm *option.rect.width();
        double rw = (1 - norm)*option.rect.width();

        // draw bar
        QColor bg1(160, 160, 160);
        QColor bg2 = option.palette.color(QPalette::Window);

        if      (colorPalette.palette)
          bg1 = colorPalette.palette->getColor(0.0);
        else if (colorPalette.color.isValid())
          bg1 = table_->charts()->interpColor(colorPalette.color, 0, 1);

        QRectF lrect(option.rect.left()     , option.rect.top(), lw, option.rect.height());
        QRectF rrect(option.rect.left() + lw, option.rect.top(), rw, option.rect.height());

        painter->fillRect(lrect.adjusted(1, 1, -1, -1), bg1);
        painter->fillRect(rrect.adjusted(1, 1, -1, -1), bg2);

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

        painter->setClipRect(lrect);
        option1.palette.setColor(QPalette::Text, tc1);
        QItemDelegate::drawDisplay(painter, option1, option.rect, QString("%1").arg(r));

        painter->setClipRect(rrect);
        option1.palette.setColor(QPalette::Text, tc2);
        QItemDelegate::drawDisplay(painter, option1, option.rect, QString("%1").arg(r));

        painter->restore();

        if (option.state & QStyle::State_Selected)
          drawSelected();

        QItemDelegate::drawFocus(painter, option, option.rect);
      }
      else
        QItemDelegate::paint(painter, option, index);
    }
    else {
      QItemDelegate::paint(painter, option, index);
    }
  }
  else {
    QVariant var = table_->modelP()->data(index);

    if (var.type() == QVariant::List) {
      QString str;

      CQChartsVariant::toString(var, str);

      drawString(painter, option, str, index);
    }
    else
      QItemDelegate::paint(painter, option, index);
  }
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
    QVariant var = table_->modelP()->data(index);

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
    QVariant var = table_->modelP()->data(index);

    table_->modelP()->setData(index, ! var.toBool());
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

  CQChartsModelDetails *details = table_->getDetails();

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

  QColor c = table_->charts()->interpColor(color, CQChartsUtil::ColorInd());

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

    CQChartsDrawUtil::drawSymbol(painter, symbol, QPointF(rect.center()),
                                 QSizeF(rect.height(), rect.height()));
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

  table_->modelP()->setData(currentIndex_, check->isChecked());
}
