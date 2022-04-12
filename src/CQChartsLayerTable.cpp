#include <CQChartsLayerTable.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsVariant.h>

#include <QHeaderView>

CQChartsViewLayerTable::
CQChartsViewLayerTable()
{
  setObjectName("viewLayerTable");

  horizontalHeader()->setStretchLastSection(true);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);
}

void
CQChartsViewLayerTable::
setView(CQChartsView *view)
{
  view_ = view;

  updateLayers(view_);
}

QImage *
CQChartsViewLayerTable::
selectedImage(CQChartsView *view) const
{
  if (! view) return nullptr;

  auto items = selectedItems();
  if (items.length() <= 0) return nullptr;

  auto *item = items[0];

  bool ok;

  long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
  if (! ok) return nullptr;

  CQChartsBuffer *buffer = nullptr;

  if      (l == 0)
    buffer = view->bgBuffer();
  else if (l == 1)
    buffer = view->fgBuffer();
  else if (l == 2)
    buffer = view->overlayBuffer();

  if (! buffer) return nullptr;

  auto *image = buffer->image();

  return image;
}

void
CQChartsViewLayerTable::
initLayers()
{
  if (rowCount() != 0)
    return;

  clear();

  setColumnCount(3);
  setRowCount(3);

  setHorizontalHeaderItem(0, new QTableWidgetItem("Buffer"));
  setHorizontalHeaderItem(1, new QTableWidgetItem("State" ));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Rect"  ));

  auto createItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    setItem(r, c, item);

    return item;
  };

  auto names = QStringList() << "Bg" << "Fg" << "Overlay";

  for (int l = 0; l < 3; ++l) {
    auto *item = createItem(names[l], l, 0);

    item->setData(Qt::UserRole, l);

    (void) createItem("", l, 1);
    (void) createItem("", l, 2);
  }
}

void
CQChartsViewLayerTable::
updateLayers(CQChartsView *view)
{
  initLayers();

  //---

  for (int l = 0; l < 3; ++l) {
    CQChartsBuffer *buffer = nullptr;

    if      (l == 0)
      buffer = view->bgBuffer();
    else if (l == 1)
      buffer = view->fgBuffer();
    else if (l == 2)
      buffer = view->overlayBuffer();

//  auto *idItem    = item(l, 0);
    auto *stateItem = item(l, 1);
    auto *rectItem  = item(l, 2);

    stateItem->setText("");

    auto rect = (buffer ? buffer->rect() : QRectF());

    auto rectStr = QString("X:%1, Y:%2, W:%3, H:%4").
                     arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());

    rectItem->setText(rectStr);
  }
}

//---

CQChartsPlotLayerTable::
CQChartsPlotLayerTable()
{
  setObjectName("plotLayerTable");

  horizontalHeader()->setStretchLastSection(true);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);
}

void
CQChartsPlotLayerTable::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  updateLayers(plot_);
}

QImage *
CQChartsPlotLayerTable::
selectedImage(CQChartsPlot *plot) const
{
  if (! plot) return nullptr;

  auto items = selectedItems();
  if (items.length() <= 0) return nullptr;

  auto *item = items[0];

  bool ok;

  long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
  if (! ok) return nullptr;

  auto *layer = plot->getLayer(static_cast<CQChartsLayer::Type>(l));
  if (! layer) return nullptr;

  auto *buffer = plot->getBuffer(layer->buffer());
  if (! buffer) return nullptr;

  auto *image = buffer->image();

  return image;
}

void
CQChartsPlotLayerTable::
initLayers()
{
  if (rowCount() != 0)
    return;

  int l1 = static_cast<int>(CQChartsLayer::firstLayer());
  int l2 = static_cast<int>(CQChartsLayer::lastLayer ());

  clear();

  setColumnCount(3);
  setRowCount(l2 - l1 + 1);

  setHorizontalHeaderItem(0, new QTableWidgetItem("Layer"));
  setHorizontalHeaderItem(1, new QTableWidgetItem("State"));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Rect" ));

  auto createItem = [&](const QString &name, int r, int c, bool editable=false) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);

    if (! editable)
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    setItem(r, c, item);

    return item;
  };

  for (int l = l1; l <= l2; ++l) {
    int i = l - l1;

    auto type = CQChartsLayer::Type(l);

    auto *idItem = createItem(CQChartsLayer::typeName(type), i, 0);

    idItem->setData(Qt::UserRole, l);

    auto *stateItem = createItem("", i, 1, /*editable*/false);

  //stateItem->setFlags(stateItem->flags() | Qt::ItemIsEnabled);
    stateItem->setFlags(stateItem->flags() | Qt::ItemIsUserCheckable);

    (void) createItem("", i, 2);
  }
}

void
CQChartsPlotLayerTable::
updateLayers(CQChartsPlot *plot)
{
  initLayers();

  //---

  int l1 = static_cast<int>(CQChartsLayer::firstLayer());
  int l2 = static_cast<int>(CQChartsLayer::lastLayer ());

  for (int l = l1; l <= l2; ++l) {
    int i = l - l1;

    auto type = CQChartsLayer::Type(l);

    auto *layer = plot->getLayer(type);

    const auto *buffer = (layer ? plot->getBuffer(layer->buffer()) : nullptr);

//  auto *idItem    = item(i, 0);
    auto *stateItem = item(i, 1);
    auto *rectItem  = item(i, 2);

    QStringList states;

    if (layer  && layer ->isActive()) states += "active";
    if (buffer && buffer->isValid ()) states += "valid";

    stateItem->setText(states.join("|"));

    stateItem->setCheckState((layer && layer->isActive()) ? Qt::Checked : Qt::Unchecked);

    auto rect = (buffer ? buffer->rect() : QRectF());

    auto rectStr = QString("X:%1, Y:%2, W:%3, H:%4").
                    arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());

    rectItem->setText(rectStr);
  }
}

bool
CQChartsPlotLayerTable::
getLayerState(CQChartsPlot *plot, int row, CQChartsLayer::Type &type, bool &active)
{
  auto *nameItem = item(row, 0);

  auto name = nameItem->text();

  type = CQChartsLayer::nameType(name);

  auto *layer = plot->getLayer(type);
  if (! layer) return false;

  auto *stateItem = item(row, 1);

  active = (stateItem->checkState() == Qt::Checked);

  return true;
}
