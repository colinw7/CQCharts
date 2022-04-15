#include <CQChartsLayerTable.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsVariant.h>

#include <QHeaderView>
#include <QPushButton>
#include <QDialog>

class CQChartsViewSettingsLayerImage : public QDialog {
 public:
  CQChartsViewSettingsLayerImage() {
    setWindowTitle("Layer Image");
  }

  void setImage(const QImage &image) {
    image_ = image;

    setFixedSize(image_.size());
  }

  void paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.drawImage(0, 0, image_);
  }

 private:
  QImage image_;
};

//------

CQChartsLayerTableControl::
CQChartsLayerTableControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("layerTableControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  viewLayerTable_ = new CQChartsViewLayerTable;

  viewLayerTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(viewLayerTable_);

  //---

  plotLayerTable_ = new CQChartsPlotLayerTable;

  plotLayerTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(plotLayerTable_);

  //---

  auto *controlFrame  = CQUtil::makeWidget<QFrame>("control");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layout->addWidget(controlFrame);

  //--

  auto *viewImageButton = CQUtil::makeLabelWidget<QPushButton>("View Image", "viewImage");

  viewImageButton->setToolTip("Show View Layer Image");

  controlLayout->addWidget(viewImageButton);

  connect(viewImageButton, SIGNAL(clicked()), this, SLOT(viewLayerImageSlot()));

  //--

  auto *plotImageButton = CQUtil::makeLabelWidget<QPushButton>("Plot Image", "plotImage");

  plotImageButton->setToolTip("Show Plot Layer Image");

  controlLayout->addWidget(plotImageButton);

  connect(plotImageButton, SIGNAL(clicked()), this, SLOT(plotLayerImageSlot()));

  //--

  controlLayout->addStretch(1);
}

void
CQChartsLayerTableControl::
setView(CQChartsView *view)
{
  view_ = view;

  viewLayerTable_->setView(view_);
}

void
CQChartsLayerTableControl::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  plotLayerTable_->setPlot(plot_);
}

void
CQChartsLayerTableControl::
viewLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  if (! view_) return;

  auto *image = viewLayerTable_->selectedImage(view_);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

void
CQChartsLayerTableControl::
plotLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  if (! plot_) return;

  auto *image = plotLayerTable_->selectedImage(plot_);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

//------

CQChartsViewLayerTable::
CQChartsViewLayerTable()
{
  setObjectName("viewLayerTable");

  horizontalHeader()->setStretchLastSection(true);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);

  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChangeSlot()));
  connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(clickedSlot(int, int)));
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

void
CQChartsViewLayerTable::
selectionChangeSlot()
{
}

void
CQChartsViewLayerTable::
clickedSlot(int row, int column)
{
  if (! view_ || column != 1)
    return;

  if (row == 0)
    view_->invalidateObjects();
  else
    view_->invalidateOverlay();
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

    auto *layer = (plot ? plot->getLayer(type) : nullptr);

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

  auto *layer = (plot ? plot->getLayer(type) : nullptr);
  if (! layer) return false;

  auto *stateItem = item(row, 1);

  active = (stateItem->checkState() == Qt::Checked);

  return true;
}

void
CQChartsPlotLayerTable::
selectionChangeSlot()
{
}

void
CQChartsPlotLayerTable::
clickedSlot(int row, int column)
{
  if (! plot_ || column != 1)
    return;

  CQChartsLayer::Type type;
  bool                active;

  if (! getLayerState(plot_, row, type, active))
    return;

  auto *layer = plot_->getLayer(type);
  if (! layer) return;

  plot_->setLayerActive(type, active);

  const auto *buffer = plot_->getBuffer(layer->buffer());

  if (buffer->type() != CQChartsBuffer::Type::MIDDLE)
    plot_->invalidateLayer(buffer->type());
  else
    plot_->drawObjs();
}
