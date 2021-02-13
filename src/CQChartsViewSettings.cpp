#include <CQChartsViewSettings.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsLoadModelDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsCreateAnnotationDlg.h>
#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsAnnotation.h>
#include <CQChartsTitleEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsAxisEdit.h>
#include <CQChartsKey.h>
#include <CQChartsViewError.h>
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsUtil.h>

#include <CQChartsPlotControlWidgets.h>

#include <CQColorsEditCanvas.h>
#include <CQColorsEditControl.h>
#include <CQColorsEditList.h>
#include <CQColors.h>
#include <CQColorsTheme.h>

#include <CQPropertyViewItem.h>
#include <CQPropertyViewModel.h>

#include <CQTableWidget.h>
#include <CQTabWidget.h>
#include <CQTabSplit.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>
#include <CQGroupBox.h>
#include <CQToolTip.h>

#include <CQDoubleRangeSlider.h>
#include <CQIntRangeSlider.h>
#include <CQTimeRangeSlider.h>

#include <QHeaderView>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QTextEdit>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QPainter>
#include <QItemDelegate>
#include <QFileDialog>
#include <QDir>
#include <QTimer>

#include <svg/info_svg.h>
#include <fstream>
#include <iostream>

class CQChartsPlotTipLabel : public QLabel {
 public:
  CQChartsPlotTipLabel() {
    setWordWrap(true);
  }

  void setText(const QString &text) {
    QLabel::setText(text);

    updateSize();
  }

  void updateSize() {
    setWordWrap(false);

    QFontMetrics fm(font());

    auto s = QLabel::sizeHint();

    int w = std::min(s.width(), fm.width("X")*50);

    setMaximumWidth(w);

    setWordWrap(true);

    size_ = QLabel::sizeHint();

    setMaximumWidth(QWIDGETSIZE_MAX);

    resize(size_);
  }

  QSize sizeHint() const {
    return size_;
  }

 public:
  QSize size_;
};

#if 0
class CQChartsPlotTip : public CQToolTipIFace {
 public:
  CQChartsPlotTip() :
   CQToolTipIFace() {
  }

 ~CQChartsPlotTip() {
    delete widget_;
  }

  void setPlot(CQChartsPlot *plot) {
    plot_ = plot;
  }

  QWidget *showWidget(const QPoint &p) override {
    if (! widget_)
      widget_ = new CQChartsPlotTipLabel;

    updateWidget(p);

    return widget_;
  }

  void hideWidget() override {
    delete widget_;

    widget_ = nullptr;
  }

  //bool trackMouse() const override { return true; }

  bool updateWidget(const QPoint &) override {
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

  bool isHideKey(int key, Qt::KeyboardModifiers mod) const override {
    if (key == Qt::Key_PageUp || key == Qt::Key_PageDown)
      return false;

    return CQToolTipIFace::isHideKey(key, mod);
  }

  bool keyPress(int key, Qt::KeyboardModifiers mod) override {
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

 private:
  using PlotP = QPointer<CQChartsPlot>;

  PlotP                 plot_;
  CQChartsPlotTipLabel* widget_   { nullptr };
  bool                  expanded_ { false };
};
#endif

//------

class CQChartsViewSettingsModelTable : public CQTableWidget {
 public:
  CQChartsViewSettingsModelTable() {
    setObjectName("modelTable");

    horizontalHeader()->setStretchLastSection(true);

  //setSelectionMode(ExtendedSelection);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updateModels(CQCharts *charts) {
    CQCharts::ModelDatas modelDatas;

    charts->getModelDatas(modelDatas);

    clear();

    setColumnCount(4);
    setRowCount(modelDatas.size());

    setHorizontalHeaderItem(0, new QTableWidgetItem("Name"       ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Index"      ));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Filename"   ));
    setHorizontalHeaderItem(3, new QTableWidgetItem("Object Name"));

    auto createItem = [&](const QString &name, int r, int c) {
      auto *item = new QTableWidgetItem(name);

      item->setToolTip(name);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);

      setItem(r, c, item);

      return item;
    };

    int i = 0;

    for (const auto &modelData : modelDatas) {
      auto *nameItem = createItem(modelData->id(), i, 0);

      nameItem->setData(Qt::UserRole, modelData->ind());

      (void) createItem(QString("%1").arg(modelData->ind()), i, 1);
      (void) createItem(modelData->filename()              , i, 2);
      (void) createItem(modelData->model()->objectName()   , i, 3);

      ++i;
    }
  }

  long selectedModel() const {
    auto items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      auto *item = items[i];
      if (item->column() != 0) continue;

      bool ok;

      long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

      if (ok)
        return ind;
    }

    return -1;
  }
};

//---

class CQChartsViewSettingsPlotTable : public CQTableWidget {
 public:
  CQChartsViewSettingsPlotTable() {
    setObjectName("plotTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionMode(ExtendedSelection);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updatePlots(CQChartsView *view) {
    // add plots to plot table (id, type and state)
    clear();

    int np = view->numPlots();

    setColumnCount(4);
    setRowCount(np);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Id"     ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Type"   ));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Connect"));
    setHorizontalHeaderItem(3, new QTableWidgetItem("Model"  ));

    auto createItem = [&](const QString &name, int r, int c) {
      auto *item = new QTableWidgetItem(name);

      item->setToolTip(name);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);

      setItem(r, c, item);

      return item;
    };

    for (int i = 0; i < np; ++i) {
      auto *plot = view->plot(i);

      //--

      // set id item store plot index in user data
      auto *idItem = createItem(plot->id(), i, 0);

      idItem->setData(Qt::UserRole, view->getIndForPlot(plot));

      // set type item
      (void) createItem(plot->type()->name(), i, 1);

      // set state item
      (void) createItem(plot->connectionStateStr(), i, 2);

      // set model item
      auto *modelData = plot->getModelData();

      if (modelData)
        (void) createItem(modelData->id(), i, 3);
    }
  }

  void setCurrentInd(int ind) {
    int nr = rowCount();

    for (int i = 0; i < nr; ++i) {
      auto *item = this->item(i, 0);

      bool ok;

      long ind1 = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

      item->setSelected(ind1 == ind);
    }
  }

  void getSelectedPlots(CQChartsView *view, std::vector<CQChartsPlot *> &plots) {
    auto items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      auto *item = items[i];
      if (item->column() != 0) continue;

      auto id = item->text();

      auto *plot = view->getPlotForId(id);

      plots.push_back(plot);
    }
  }
};

//---

class CQChartsViewSettingsAnnotationsTable;

class CQChartsViewSettingsAnnotationDelegate : public QItemDelegate {
 public:
  CQChartsViewSettingsAnnotationDelegate(CQChartsViewSettingsAnnotationsTable *table) :
   table_(table) {
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  CQChartsViewSettingsAnnotationsTable *table() const { return table_; }

 private:
  CQChartsViewSettingsAnnotationsTable *table_ { nullptr };
};

//--

class CQChartsViewSettingsAnnotationsTable : public CQTableWidget {
 public:
  CQChartsViewSettingsAnnotationsTable(CQChartsView *view=nullptr, CQChartsPlot *plot=nullptr) :
   view_(view), plot_(plot) {
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

  //setSelectionMode(ExtendedSelection);

    setSelectionBehavior(QAbstractItemView::SelectRows);

    //---

    delegate_ = new CQChartsViewSettingsAnnotationDelegate(this);

    setItemDelegate(delegate_);
  }

 ~CQChartsViewSettingsAnnotationsTable() {
    delete delegate_;
  }

  void setView(CQChartsView *view) { view_ = view; plot_ = nullptr; }
  void setPlot(CQChartsPlot *plot) { plot_ = plot; view_ = nullptr; }

  void addHeaderItems() {
    setColumnCount(3);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Id"   ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Type" ));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Style"));
  }

  QTableWidgetItem *createItem(const QString &name, int r, int c,
                               CQChartsAnnotation *annotation) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    item->setData(Qt::UserRole, annotation->ind());

    setItem(r, c, item);

    return item;
  }

  void getSelectedAnnotations(std::vector<CQChartsAnnotation *> &annotations) {
    auto items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      auto *item = items[i];
      if (item->column() != 0) continue;

      CQChartsAnnotation *annotation = itemAnnotation(item);

      if (annotation)
        annotations.push_back(annotation);
    }
  }

  CQChartsAnnotation *itemAnnotation(QTableWidgetItem *item) const {
    bool ok;

    long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

    CQChartsAnnotation *annotation = nullptr;

    if      (view_)
      annotation = view_->getAnnotationByInd(int(ind));
    else if (plot_)
      annotation = plot_->getAnnotationByInd(int(ind));

    return annotation;
  }

 protected:
  CQChartsView*                           view_     { nullptr };
  CQChartsPlot*                           plot_     { nullptr };
  CQChartsViewSettingsAnnotationDelegate* delegate_ { nullptr };
};

//--

void
CQChartsViewSettingsAnnotationDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.column() == 2) {
    auto *item = table_->item(index.row(), index.column());
    if (! item) return;

    auto *annotation = table_->itemAnnotation(item);
    if (! annotation) return;

    QItemDelegate::drawBackground(painter, option, index);

    auto rect = option.rect;

    rect.setWidth(option.rect.height());

    rect.adjust(0, 1, -3, -2);

    CQChartsPenBrush penBrush;

    annotation->calcPenBrush(penBrush);

    painter->fillRect(rect, penBrush.brush);

    painter->setPen(penBrush.pen);

    painter->drawRect(rect);
  }
  else {
    QItemDelegate::paint(painter, option, index);
  }
}

//--

class CQChartsViewSettingsViewAnnotationsTable : public CQChartsViewSettingsAnnotationsTable {
 public:
  CQChartsViewSettingsViewAnnotationsTable() {
    setObjectName("viewAnnotationsTable");
  }

  void updateAnnotations(CQChartsView *view) {
    setView(view);

    clear();

    if (! view)
      return;

    const auto &viewAnnotations = view->annotations();

    int nv = viewAnnotations.size();

    setRowCount(nv);

    addHeaderItems();

    for (int i = 0; i < nv; ++i) {
      auto *annotation = viewAnnotations[i];

      (void) createItem(annotation->id()      , i, 0, annotation);
      (void) createItem(annotation->typeName(), i, 1, annotation);
      (void) createItem(""                    , i, 2, annotation);
    }
  }
};

//--

class CQChartsViewSettingsPlotAnnotationsTable : public CQChartsViewSettingsAnnotationsTable {
 public:
  CQChartsViewSettingsPlotAnnotationsTable() {
    setObjectName("plotAnnotationsTable");
  }

  void updateAnnotations(CQChartsPlot *plot) {
    setPlot(plot);

    clear();

    if (! plot)
      return;

    const auto &plotAnnotations = plot->annotations();

    int np = plotAnnotations.size();

    setRowCount(np);

    addHeaderItems();

    for (int i = 0; i < np; ++i) {
      auto *annotation = plotAnnotations[i];

      (void) createItem(annotation->id()      , i, 0, annotation);
      (void) createItem(annotation->typeName(), i, 1, annotation);
      (void) createItem(""                    , i, 2, annotation);
    }
  }
};

//---

class CQChartsViewSettingsViewLayerTable : public CQTableWidget {
 public:
  CQChartsViewSettingsViewLayerTable() {
    setObjectName("viewLayerTable");

    horizontalHeader()->setStretchLastSection(true);

  //setSelectionMode(ExtendedSelection);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  QImage *selectedImage(CQChartsView *view) const {
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

  void initLayers() {
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

    QStringList names = QStringList() << "Bg" << "Fg" << "Overlay";

    for (int l = 0; l < 3; ++l) {
      auto *item = createItem(names[l], l, 0);

      item->setData(Qt::UserRole, l);

      (void) createItem("", l, 1);
      (void) createItem("", l, 2);
    }
  }

  void updateLayers(CQChartsView *view) {
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
};

//---

class CQChartsViewSettingsPlotLayerTable : public CQTableWidget {
 public:
  CQChartsViewSettingsPlotLayerTable() {
    setObjectName("plotLayerTable");

    horizontalHeader()->setStretchLastSection(true);

  //setSelectionMode(ExtendedSelection);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  QImage *selectedImage(CQChartsPlot *plot) const {
    if (! plot) return nullptr;

    auto items = selectedItems();
    if (items.length() <= 0) return nullptr;

    auto *item = items[0];

    bool ok;

    long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
    if (! ok) return nullptr;

    auto *layer = plot->getLayer((CQChartsLayer::Type) l);
    if (! layer) return nullptr;

    auto *buffer = plot->getBuffer(layer->buffer());
    if (! buffer) return nullptr;

    auto *image = buffer->image();

    return image;
  }

  void initLayers() {
    if (rowCount() != 0)
      return;

    int l1 = (int) CQChartsLayer::firstLayer();
    int l2 = (int) CQChartsLayer::lastLayer ();

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

  void updateLayers(CQChartsPlot *plot) {
    int l1 = (int) CQChartsLayer::firstLayer();
    int l2 = (int) CQChartsLayer::lastLayer ();

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

  bool getLayerState(CQChartsPlot *plot, int row, CQChartsLayer::Type &type, bool &active) {
    auto *nameItem = item(row, 0);

    auto name = nameItem->text();

    type = CQChartsLayer::nameType(name);

    auto *layer = plot->getLayer(type);
    if (! layer) return false;

    auto *stateItem = item(row, 1);

    active = (stateItem->checkState() == Qt::Checked);

    return true;
  }
};

//------

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  connect(charts, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(invalidateModelDetails()));
  connect(charts, SIGNAL(modelDataDataChanged()), this, SLOT(updateModelsData()));

  connect(view, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(view, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  connect(view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

  connect(window, SIGNAL(themePalettesChanged()), this, SLOT(updatePalettes()));
  connect(charts, SIGNAL(interfaceThemeChanged()), this, SLOT(updateInterface()));

  //--

  setObjectName("settings");

  setAutoFillBackground(true);

  addWidgets();

  //--

  updateErrorsTimer_ = new QTimer;

  updateErrorsTimer_->setSingleShot(true);

  connect(updateErrorsTimer_, SIGNAL(timeout()), this, SLOT(updateErrors()));
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
//delete propertiesWidgets_.plotTip;

  delete updateErrorsTimer_;
}

void
CQChartsViewSettings::
addWidgets()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  tab_ = CQUtil::makeWidget<CQTabWidget>("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //----

  auto addTab = [&](const QString &name) {
    auto objectName = name + "Frame";

    objectName[0] = objectName[0].toLower();

    auto *frame = CQUtil::makeWidget<QFrame>(objectName);

    tab_->addTab(frame, name);

    return frame;
  };

  //--

  // Properties Tab
  initPropertiesFrame(addTab("Properties"));

  // Models Tab
  initModelsFrame(addTab("Models"));

  // Plots Tab
  initPlotsFrame(addTab("Plots"));

  // Annotations Tab
  initAnnotationsFrame(addTab("Annotations"));

  // Objects Tab
  initObjectsFrame(addTab("Objects"));

  // Theme Tab
  initThemeFrame(addTab("Colors"));

  // Layers Tab
  initLayersFrame(addTab("Layers"));

  // Query Tab
  initQueryFrame(addTab("Query"));

  // Errors Tab
  initErrorsFrame(addTab("Errors"));

  //----

  updateModelsData();

  updatePlotControls();

  updateAnnotations();

  updatePlotObjects();

  updateErrors();
}

//------

void
CQChartsViewSettings::
initPropertiesFrame(QFrame *propertiesFrame)
{
  auto createPushButton = [&](const QString &label, const QString &objName,
                              const QString &tipStr, const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tipStr);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  //---

  auto *view = window_->view();

  auto *propertiesLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //--

  propertiesWidgets_.propertiesSplit = CQUtil::makeWidget<CQTabSplit>("propertiesSplit");

  propertiesWidgets_.propertiesSplit->setOrientation(Qt::Vertical);
  propertiesWidgets_.propertiesSplit->setGrouped(true);

  propertiesLayout->addWidget(propertiesWidgets_.propertiesSplit);

  //----

  // Create View Properties Frame
  auto *viewFrame       = CQUtil::makeWidget<QFrame>("viewFrame");
  auto *viewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(viewFrame, 2, 2);

  propertiesWidgets_.propertiesSplit->addWidget(viewFrame, "View");

  //--

  propertiesWidgets_.viewPropertyTree = new ViewPropertiesWidget(this, view);

  viewFrameLayout->addWidget(propertiesWidgets_.viewPropertyTree);

  //--

  auto *viewEditFrame       = CQUtil::makeWidget<QFrame>("viewEditFrame");
  auto *viewEditFrameLayout = CQUtil::makeLayout<QHBoxLayout>(viewEditFrame, 2, 2);

  auto *viewKeyButton =
    createPushButton("Edit Key...", "key", "Edit View Key", SLOT(editViewKeySlot()));
  auto *viewWriteButton =
    createPushButton("Write", "write", "Write View Script", SLOT(writeViewSlot()));

  viewEditFrameLayout->addWidget(viewKeyButton);
  viewEditFrameLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  viewEditFrameLayout->addWidget(viewWriteButton);
  viewEditFrameLayout->addStretch(1);

  viewFrameLayout->addWidget(viewEditFrame);

  //----

  // Create Plots Properties Frame
  auto *plotsFrame       = CQUtil::makeWidget<QFrame>("plotsFrame");
  auto *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  propertiesWidgets_.propertiesSplit->addWidget(plotsFrame, "Plots");

  //--

  propertiesWidgets_.plotsTab = CQUtil::makeWidget<CQTabWidget>("tab");

  plotsFrameLayout->addWidget(propertiesWidgets_.plotsTab);

  connect(propertiesWidgets_.plotsTab, SIGNAL(currentChanged(int)),
          this, SLOT(plotsTabChangedSlot()));
  //--

  auto *plotEditFrame       = CQUtil::makeWidget<QFrame>("plotEditFrame");
  auto *plotEditFrameLayout = CQUtil::makeLayout<QHBoxLayout>(plotEditFrame, 2, 2);

  auto *plotTitleButton =
    createPushButton("Edit Title..." , "title", "Edit Plot Title"  , SLOT(editPlotTitleSlot()));
  auto *plotKeyButton   =
    createPushButton("Edit Key..."   , "key"  , "Edit Plot Key"    , SLOT(editPlotKeySlot()));
  auto *plotXAxisButton =
    createPushButton("Edit X Axis...", "xaxis", "Edit Plot X Axis" , SLOT(editPlotXAxisSlot()));
  auto *plotYAxisButton =
    createPushButton("Edit Y Axis...", "yaxis", "Edit Plot Y Axis" , SLOT(editPlotYAxisSlot()));
  auto *plotWriteButton =
    createPushButton("Write"         , "write", "Write Plot Script", SLOT(writePlotSlot()));

  plotEditFrameLayout->addWidget(plotTitleButton);
  plotEditFrameLayout->addWidget(plotKeyButton);
  plotEditFrameLayout->addWidget(plotXAxisButton);
  plotEditFrameLayout->addWidget(plotYAxisButton);
  plotEditFrameLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  plotEditFrameLayout->addWidget(plotWriteButton);
  plotEditFrameLayout->addStretch(1);

  plotsFrameLayout->addWidget(plotEditFrame);

  //--

#if 0
  propertiesWidgets_.plotTip = new CQChartsPlotTip;

  propertiesWidgets_.plotTipButton = new QToolButton;

  propertiesWidgets_.plotTipButton->setIcon(CQPixmapCacheInst->getIcon("INFO"));

  plotsFrame->setCornerWidget(propertiesWidgets_.plotTipButton);

  CQToolTip::setToolTip(propertiesWidgets_.plotTipButton, propertiesWidgets_.plotTip);
#endif

  //---

  controlFrame_ = CQUtil::makeWidget<CQChartsPlotControlFrame>("controlFrame");

  propertiesWidgets_.propertiesSplit->addWidget(controlFrame_, "Quick Controls");

  //---

  int i1 = INT_MAX*0.4;
  int i3 = INT_MAX*0.1;
  int i2 = INT_MAX - i1 - i3;

  propertiesWidgets_.propertiesSplit->setSizes(QList<int>({i1, i2, i3}));
}

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  auto *modelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsFrame, 2, 2);

  //----

  auto *modelsSplit = CQUtil::makeWidget<CQTabSplit>("modelsSplit");

  modelsSplit->setOrientation(Qt::Vertical);
  modelsSplit->setGrouped(true);

  modelsFrameLayout->addWidget(modelsSplit);

  //----

  // Models Frame
  auto *modelsModelsFrame       = CQUtil::makeWidget<QFrame>("modelsModelsFrame");
  auto *modelsModelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsModelsFrame, 2, 2);

  modelsSplit->addWidget(modelsModelsFrame, "Models");

  //--

  modelsWidgets_.modelTable = new CQChartsViewSettingsModelTable;

  connect(modelsWidgets_.modelTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(modelsSelectionChangeSlot()));

  modelsModelsFrameLayout->addWidget(modelsWidgets_.modelTable);

  //----

  // Model Details
  modelsWidgets_.detailsFrame = CQUtil::makeWidget<QFrame>("detailsFrame");

  auto *detailsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsWidgets_.detailsFrame, 2, 2);

  modelsSplit->addWidget(modelsWidgets_.detailsFrame, "Details");

  //--

  auto *view   = window_->view();
  auto *charts = view->charts();

  modelsWidgets_.detailsWidget = new CQChartsModelDetailsWidget(charts);

  detailsFrameLayout->addWidget(modelsWidgets_.detailsWidget);

  //----

  // Model Buttons
  auto *modelControlLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  modelsModelsFrameLayout->addLayout(modelControlLayout);

  //--

  auto createButton = [&](const QString &label, const QString &objName, const QString &tip,
                          const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  modelsWidgets_.loadButton =
    createButton("Load...", "load"  , "Load Model", SLOT(loadModelSlot()));
  modelsWidgets_.editButton =
    createButton("Edit...", "edit"  , "Edit Selected Model", SLOT(editModelSlot()));
  modelsWidgets_.removeButton =
    createButton("Remove" , "remove", "Remove Selected Model", SLOT(removeModelSlot()));

  modelsWidgets_.plotButton =
    createButton("Create Plot...", "plot", "Create Plot from Selected Model",
                 SLOT(createPlotModelSlot()));

  modelControlLayout->addWidget(modelsWidgets_.loadButton);
  modelControlLayout->addWidget(modelsWidgets_.editButton);
  modelControlLayout->addWidget(modelsWidgets_.removeButton);
  modelControlLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  modelControlLayout->addWidget(modelsWidgets_.plotButton);

  modelsWidgets_.editButton  ->setEnabled(false);
  modelsWidgets_.removeButton->setEnabled(false);
  modelsWidgets_.plotButton  ->setEnabled(false);

  //--

  modelControlLayout->addStretch(1);

  //--

  modelsSplit->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initPlotsFrame(QFrame *plotsFrame)
{
  auto *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  //----

  auto *plotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Plots", "plotsGroup");
  auto *plotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotsGroup, 2, 2);

  plotsFrameLayout->addWidget(plotsGroup);

  //--

  plotsWidgets_.plotTable = new CQChartsViewSettingsPlotTable;

  connect(plotsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotsSelectionChangeSlot()));

  plotsGroupLayout->addWidget(plotsWidgets_.plotTable);

  //----

#if 0
  auto createButton = [&](const QString &label, const QString &objName, const QString &tip,
                          const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  //--

  auto *controlFrame  = CQUtil::makeWidget<QFrame>("controlFrame");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  plotsWidgets_.createButton =
    createButton("Plot...", "plot", "Create Plot from Current Model",
                 SLOT(createPlotSlot()));

  controlLayout->addWidget(plotsWidgets_.createButton);
  controlLayout->addStretch(1);

  //plotsWidgets_.createButton->setEnabled(false);

  plotsFrameLayout->addWidget(controlFrame);
#endif

  //----

#if 0
  auto *editFrame  = CQUtil::makeWidget<QFrame>("editFrame");
  auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(editFrame, 2, 2);

  auto *titleButton = createButton("Title" , "title", "Edit Title" , SLOT(editPlotTitleSlot()));
  auto *keyButton   = createButton("Key"   , "key"  , "Edit Key"   , SLOT(editPlotKeySlot()));
  auto *xAxisButton = createButton("X Axis", "xaxis", "Edit X Axis", SLOT(editPlotXAxisSlot()));
  auto *yAxisButton = createButton("Y Axis", "yaxis", "Edit Y Axis", SLOT(editPlotYAxisSlot()));

  editLayout->addWidget(titleButton);
  editLayout->addWidget(keyButton);
  editLayout->addWidget(xAxisButton);
  editLayout->addWidget(yAxisButton);
  editLayout->addStretch(1);

  plotsFrameLayout->addWidget(editFrame);
#endif

  //----

  auto *groupPlotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Connect", "groupPlotsGroup");
  auto *groupPlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(groupPlotsGroup, 2, 2);

  plotsFrameLayout->addWidget(groupPlotsGroup);

  //----

  auto *groupPlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsCheckLayout);

  //--

  plotsWidgets_.overlayCheck = CQUtil::makeLabelWidget<QCheckBox>("Overlay", "overlay");
  plotsWidgets_.x1x2Check    = CQUtil::makeLabelWidget<QCheckBox>("X1/X2"  , "x1x2");
  plotsWidgets_.y1y2Check    = CQUtil::makeLabelWidget<QCheckBox>("Y1/Y2"  , "y1y2");

  plotsWidgets_.overlayCheck->setToolTip("Overlay plots so they shared the same range");
  plotsWidgets_.x1x2Check   ->setToolTip("Plot shares Y axis with another plot");
  plotsWidgets_.y1y2Check   ->setToolTip("Plot shares X axis with another plot");

  connect(plotsWidgets_.x1x2Check, SIGNAL(stateChanged(int)),
          this, SLOT(updatePlotOverlayState()));
  connect(plotsWidgets_.y1y2Check, SIGNAL(stateChanged(int)),
          this, SLOT(updatePlotOverlayState()));

  groupPlotsCheckLayout->addWidget(plotsWidgets_.overlayCheck);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.x1x2Check);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.y1y2Check);
  groupPlotsCheckLayout->addStretch(1);

  //----

  auto *groupPlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsButtonsLayout);

  //--

  auto *groupApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  groupApplyButton->setToolTip("Apply connection options to all plots");

  connect(groupApplyButton, SIGNAL(clicked()), this, SLOT(groupPlotsSlot()));

  groupPlotsButtonsLayout->addWidget(groupApplyButton);

  //--

  groupPlotsButtonsLayout->addStretch(1);

  //----

  auto *placePlotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Place", "placePlotsGroup");
  auto *placePlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(placePlotsGroup, 2, 2);

  plotsFrameLayout->addWidget(placePlotsGroup);

  //--

  auto *placePlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsCheckLayout);

  plotsWidgets_.placeVerticalRadio   =
    CQUtil::makeLabelWidget<QRadioButton>("Vertical", "vertical");
  plotsWidgets_.placeHorizontalRadio =
    CQUtil::makeLabelWidget<QRadioButton>("Horizontal", "horizontal");
  plotsWidgets_.placeGridRadio       =
    CQUtil::makeLabelWidget<QRadioButton>("Grid", "grid");

  plotsWidgets_.placeVerticalRadio  ->setToolTip("Place places vertically");
  plotsWidgets_.placeHorizontalRadio->setToolTip("Place places horizontally");
  plotsWidgets_.placeGridRadio      ->setToolTip("Place places in grid");

  plotsWidgets_.placeVerticalRadio->setChecked(true);

  placePlotsCheckLayout->addWidget(plotsWidgets_.placeVerticalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeHorizontalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeGridRadio);

  placePlotsCheckLayout->addStretch(1);

  //--

  auto *placePlotsGridLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsGridLayout);

  plotsWidgets_.placeRowsEdit    = CQUtil::makeWidget<CQIntegerSpin>("rowsEdit");
  plotsWidgets_.placeColumnsEdit = CQUtil::makeWidget<CQIntegerSpin>("columnsEdit");

  plotsWidgets_.placeRowsEdit->setValue(1);
  plotsWidgets_.placeRowsEdit->setMinimum(1);
  plotsWidgets_.placeRowsEdit->setToolTip("Number of Rows");

  plotsWidgets_.placeColumnsEdit->setValue(1);
  plotsWidgets_.placeColumnsEdit->setMinimum(1);
  plotsWidgets_.placeColumnsEdit->setToolTip("Number of Columns");

  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Rows", "row"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeRowsEdit);
  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Columns", "columns"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeColumnsEdit);
  placePlotsGridLayout->addStretch(1);

  //--

  auto *placePlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsButtonsLayout);

  auto *placeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  placeApplyButton->setToolTip("Apply placement options to all plots");

  placePlotsButtonsLayout->addWidget(placeApplyButton);
  placePlotsButtonsLayout->addStretch(1);

  connect(placeApplyButton, SIGNAL(clicked()), this, SLOT(placePlotsSlot()));

  //----

  auto createPushButton = [&](const QString &label, const QString &objName, const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *controlPlotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Control", "controlPlotsGroup");

  plotsFrameLayout->addWidget(controlPlotsGroup);

  auto *controlPlotsGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlPlotsGroup, 2, 2);

  plotsWidgets_.raiseButton  = createPushButton("Raise"    , "raise" , SLOT(raisePlotSlot()));
  plotsWidgets_.lowerButton  = createPushButton("Lower"    , "lower" , SLOT(lowerPlotSlot()));
  plotsWidgets_.createButton = createPushButton("Create...", "create", SLOT(createPlotSlot()));
  plotsWidgets_.removeButton = createPushButton("Remove"   , "remove", SLOT(removePlotsSlot()));

//auto *writePlotButton = createPushButton("Write" , "write" , SLOT(writePlotSlot()));

  plotsWidgets_.raiseButton ->setToolTip("Raise selected plot");
  plotsWidgets_.lowerButton ->setToolTip("Lower selected plot");
  plotsWidgets_.createButton->setToolTip("Create new plot");
  plotsWidgets_.removeButton->setToolTip("Remove selected plot");

  plotsWidgets_.raiseButton ->setEnabled(false);
  plotsWidgets_.lowerButton ->setEnabled(false);
  plotsWidgets_.removeButton->setEnabled(false);

  controlPlotsGroupLayout->addWidget(plotsWidgets_.raiseButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.lowerButton);
  controlPlotsGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  controlPlotsGroupLayout->addWidget(plotsWidgets_.createButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.removeButton);
//controlPlotsGroupLayout->addWidget(writePlotButton);

  //---

  controlPlotsGroupLayout->addStretch(1);

  //---

  updatePlotOverlayState();
}

void
CQChartsViewSettings::
initAnnotationsFrame(QFrame *annotationsFrame)
{
  auto *annotationsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(annotationsFrame, 2, 2);

  //---

  annotationsWidgets_.split = CQUtil::makeWidget<CQTabSplit>("annotationsSplit");

  annotationsWidgets_.split->setOrientation(Qt::Vertical);
  annotationsWidgets_.split->setGrouped(true);

  annotationsFrameLayout->addWidget(annotationsWidgets_.split);

  //----

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const char *slotName, const QString &tip) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    button->setToolTip(tip);

    return button;
  };

  //---

  // view annotations
  annotationsWidgets_.viewFrame = CQUtil::makeWidget<QFrame>("viewFrame");

  auto *viewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(annotationsWidgets_.viewFrame, 2, 2);

  annotationsWidgets_.split->addWidget(annotationsWidgets_.viewFrame, "View");

  //--

  annotationsWidgets_.viewTable = new CQChartsViewSettingsViewAnnotationsTable;

  connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewAnnotationSelectionChangeSlot()));

  viewFrameLayout->addWidget(annotationsWidgets_.viewTable);

  //---

  // create view annotation buttons
  auto *viewControlGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("View Control", "viewControlGroup");

  viewFrameLayout->addWidget(viewControlGroup);

  auto *viewControlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(viewControlGroup, 2, 2);

  annotationsWidgets_.viewRaiseButton =
    createPushButton("Raise"    , "raise" , SLOT(raiseViewAnnotationSlot()),
                     "Raise View Annotation");
  annotationsWidgets_.viewLowerButton =
    createPushButton("Lower"    , "lower" , SLOT(lowerViewAnnotationSlot()),
                     "Lower View Annotation");
  annotationsWidgets_.viewCreateButton =
    createPushButton("Create...", "create", SLOT(createViewAnnotationSlot()),
                     "Create View Annotation");
  annotationsWidgets_.viewEditButton =
    createPushButton("Edit...", "edit", SLOT(editViewAnnotationSlot()),
                     "Edit Selected View Annotation");
  annotationsWidgets_.viewRemoveButton =
    createPushButton("Remove", "remove", SLOT(removeViewAnnotationsSlot()),
                     "Remove Selected View Annotation");

  annotationsWidgets_.viewEditButton  ->setEnabled(false);
  annotationsWidgets_.viewRemoveButton->setEnabled(false);

  viewControlGroupLayout->addWidget(annotationsWidgets_.viewRaiseButton);
  viewControlGroupLayout->addWidget(annotationsWidgets_.viewLowerButton);
  viewControlGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  viewControlGroupLayout->addWidget(annotationsWidgets_.viewCreateButton);
  viewControlGroupLayout->addWidget(annotationsWidgets_.viewEditButton);
  viewControlGroupLayout->addWidget(annotationsWidgets_.viewRemoveButton);
  viewControlGroupLayout->addStretch(1);

  //----

  // plot annotations
  annotationsWidgets_.plotFrame = CQUtil::makeWidget<QFrame>("plotFrame");

  auto *plotFrameLayout = CQUtil::makeLayout<QVBoxLayout>(annotationsWidgets_.plotFrame, 2, 2);

  annotationsWidgets_.split->addWidget(annotationsWidgets_.plotFrame, "Plot");

  //--

  annotationsWidgets_.plotTable = new CQChartsViewSettingsPlotAnnotationsTable;

  connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotAnnotationSelectionChangeSlot()));

  plotFrameLayout->addWidget(annotationsWidgets_.plotTable);

  //--

  // create plot annotation buttons
  auto *plotControlGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("Plot Control", "plotControlGroup");

  plotFrameLayout->addWidget(plotControlGroup);

  auto *plotControlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(plotControlGroup, 2, 2);

  annotationsWidgets_.plotRaiseButton =
    createPushButton("Raise"    , "raise" , SLOT(raisePlotAnnotationSlot()),
                     "Raise View Annotation");
  annotationsWidgets_.plotLowerButton =
    createPushButton("Lower"    , "lower" , SLOT(lowerPlotAnnotationSlot()),
                     "Lower View Annotation");
  annotationsWidgets_.plotCreateButton =
    createPushButton("Create...", "create", SLOT(createPlotAnnotationSlot()),
                     "Create Plot Annotation");
  annotationsWidgets_.plotEditButton =
    createPushButton("Edit...", "edit", SLOT(editPlotAnnotationSlot()),
                     "Edit Selected Plot Annotation");
  annotationsWidgets_.plotRemoveButton =
    createPushButton("Remove", "remove", SLOT(removePlotAnnotationsSlot()),
                     "Remove Selected Plot Annotation");

  annotationsWidgets_.plotEditButton  ->setEnabled(false);
  annotationsWidgets_.plotRemoveButton->setEnabled(false);

  plotControlGroupLayout->addWidget(annotationsWidgets_.plotRaiseButton);
  plotControlGroupLayout->addWidget(annotationsWidgets_.plotLowerButton);
  plotControlGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  plotControlGroupLayout->addWidget(annotationsWidgets_.plotCreateButton);
  plotControlGroupLayout->addWidget(annotationsWidgets_.plotEditButton);
  plotControlGroupLayout->addWidget(annotationsWidgets_.plotRemoveButton);
  plotControlGroupLayout->addStretch(1);

  //--

  annotationsWidgets_.split->setSizes(QList<int>({INT_MAX, INT_MAX}));

  //--

  // create view/plot annotation buttons
  auto *controlGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("View/Plot Control", "controlGroup");

  annotationsFrameLayout->addWidget(controlGroup);

  auto *controlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlGroup, 2, 2);

  annotationsWidgets_.writeButton =
    createPushButton("Write", "write", SLOT(writeAnnotationSlot()),
                     "Write View and Plot Annotations");

  controlGroupLayout->addWidget(annotationsWidgets_.writeButton);
  controlGroupLayout->addStretch(1);
}

void
CQChartsViewSettings::
initObjectsFrame(QFrame *objectsFrame)
{
  auto *objectsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(objectsFrame, 2, 2);

  objectsWidgets_.propertyTree = new ObjectPropertiesWidget(this);

  objectsFrameLayout->addWidget(objectsWidgets_.propertyTree);
}

void
CQChartsViewSettings::
initThemeFrame(QFrame *themeFrame)
{
  auto *themeFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themeFrame, 2, 2);

  //--

  auto *themeSubTab = CQUtil::makeWidget<CQTabWidget>("themeSubTab");

  themeFrameLayout->addWidget(themeSubTab);

  // tab for theme
  auto *themePalettesFrame       = CQUtil::makeWidget<QFrame>("themePalettesFrame");
  auto *themePalettesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themePalettesFrame, 2, 2);

  themeSubTab->addTab(themePalettesFrame, "Theme");

  // tab for theme palettes
  auto *palettesFrame       = CQUtil::makeWidget<QFrame>("palettesFrame");
  auto *palettesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(palettesFrame, 2, 2);

  themeSubTab->addTab(palettesFrame, "Palettes");

  // tab for interface palette
  auto *interfaceFrame       = CQUtil::makeWidget<QFrame>("interfaceFrame");
  auto *interfaceFrameLayout = CQUtil::makeLayout<QHBoxLayout>(interfaceFrame, 2, 2);

  themeSubTab->addTab(interfaceFrame, "Interface");

  //--

#if 0
  auto *themeColorsFrame  = CQUtil::makeWidget<QFrame>("themeColorsFrame");
  auto *themeColorsLayout = CQUtil::makeLayout<QGridLayout>(themeColorsFrame, 2, 2);

  auto *selColorLabel = CQUtil::makeLabelWidget("Selection", "selColorLabel");
  auto *selColorEdit  = CQUtil::makeWidget<CQChartsLineEdit>("selColorEdit");

  selColorLabel->setToolTip("Selection Color");

  themeColorsLayout->addWidget(selColorLabel, 0, 0);
  themeColorsLayout->addWidget(selColorEdit , 0, 1);

  auto *insColorLabel = CQUtil::makeLabelWidget("Inside", "insColorLabel");
  auto *insColorEdit  = CQUtil::makeWidget<CQChartsLineEdit>("insColorEdit");

  insColorEdit ->setToolTip("Inside Color");

  themeColorsLayout->addWidget(insColorLabel, 1, 0);
  themeColorsLayout->addWidget(insColorEdit , 1, 1);

  paletteLayout->addWidget(themeColorsFrame);
#endif

  //--

  auto *view  = window_->view();
  auto *theme = view->theme();

  //--

  // create palettes list
  themeWidgets_.palettesList = new CQColorsEditList(this);

  themePalettesFrameLayout->addWidget(themeWidgets_.palettesList);

  //connect(themeWidgets_.palettesList, SIGNAL(palettesChanged()), this, SLOT(updateView()));

  //--

  // create palettes frame
  auto *palettesControlFrame       = CQUtil::makeWidget<QFrame>("control");
  auto *palettesControlFrameLayout = CQUtil::makeLayout<QHBoxLayout>(palettesControlFrame, 2, 2);

  //--

  auto *paletteNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "paletteNameLabel");

  themeWidgets_.palettesCombo = CQUtil::makeWidget<QComboBox>("palettesCombo");

  QStringList paletteNames;

  CQColorsMgrInst->getPaletteNames(paletteNames);

  themeWidgets_.palettesCombo->addItems(paletteNames);

  themeWidgets_.palettesCombo->setToolTip("Name of Palette to View/Edit");

  palettesControlFrameLayout->addWidget(paletteNameLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesCombo);

  connect(themeWidgets_.palettesCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(palettesComboSlot(int)));

  //--

  auto *resetButton = CQUtil::makeLabelWidget<QPushButton>("Reset", "resetButton");

  resetButton->setToolTip("Reset palette to default (initial) values");

  palettesControlFrameLayout->addWidget(resetButton);

  connect(resetButton, SIGNAL(clicked()), this, SLOT(palettesResetSlot()));

  //--

  palettesControlFrameLayout->addStretch(1);

  palettesFrameLayout->addWidget(palettesControlFrame);

  //----

  // create palettes splitter and add canvas and control
  auto *palettesSplitter = CQUtil::makeWidget<QSplitter>("splitter");

  palettesSplitter->setOrientation(Qt::Vertical);

  palettesFrameLayout->addWidget(palettesSplitter);

  themeWidgets_.palettesPlot    = new CQColorsEditCanvas(this, theme->palette());
  themeWidgets_.palettesControl = new CQColorsEditControl(themeWidgets_.palettesPlot);

  palettesSplitter->addWidget(themeWidgets_.palettesPlot);
  palettesSplitter->addWidget(themeWidgets_.palettesControl);

  connect(themeWidgets_.palettesControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));

  connect(themeWidgets_.palettesPlot, SIGNAL(colorsChanged()),
          this, SLOT(paletteColorsChangedSlot()));
  //----

  // create interface splitter and add canvas and control
  auto *interfaceSplitter = CQUtil::makeWidget<QSplitter>("splitter");

  interfaceSplitter->setOrientation(Qt::Vertical);

  interfaceFrameLayout->addWidget(interfaceSplitter);

  themeWidgets_.interfacePlot    = new CQColorsEditCanvas(this, view->interfacePalette());
  themeWidgets_.interfacePlot->setGray(true);
  themeWidgets_.interfaceControl = new CQColorsEditControl(themeWidgets_.interfacePlot);

  interfaceSplitter->addWidget(themeWidgets_.interfacePlot);
  interfaceSplitter->addWidget(themeWidgets_.interfaceControl);

  connect(themeWidgets_.interfaceControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));

  connect(themeWidgets_.interfacePlot, SIGNAL(colorsChanged()),
          this, SLOT(paletteColorsChangedSlot()));

  //---

  updatePalettes();
  //updatePaletteWidgets();
}

void
CQChartsViewSettings::
initLayersFrame(QFrame *layersFrame)
{
  auto *layersFrameLayout = CQUtil::makeLayout<QVBoxLayout>(layersFrame, 2, 2);

  //---

  layersWidgets_.viewLayerTable = new CQChartsViewSettingsViewLayerTable;

  layersWidgets_.viewLayerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layersFrameLayout->addWidget(layersWidgets_.viewLayerTable);

  connect(layersWidgets_.viewLayerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewLayersSelectionChangeSlot()));
  connect(layersWidgets_.viewLayerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(viewLayersClickedSlot(int, int)));

  //---

  layersWidgets_.plotLayerTable = new CQChartsViewSettingsPlotLayerTable;

  layersWidgets_.plotLayerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layersFrameLayout->addWidget(layersWidgets_.plotLayerTable);

  connect(layersWidgets_.plotLayerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotLayersSelectionChangeSlot()));
  connect(layersWidgets_.plotLayerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(plotLayersClickedSlot(int, int)));

  //---

  auto *controlFrame  = CQUtil::makeWidget<QFrame>("control");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layersFrameLayout->addWidget(controlFrame);

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

//------

void
CQChartsViewSettings::
initQueryFrame(QFrame *queryFrame)
{
  auto *queryFrameLayout = CQUtil::makeLayout<QVBoxLayout>(queryFrame, 2, 2);

  //---

  queryText_ = CQUtil::makeWidget<QTextEdit>("queryText");

  queryFrameLayout->addWidget(queryText_);

  //---

  auto *view = window_->view();

  connect(view, SIGNAL(showQueryText(const QString &)),
          this, SLOT(showQueryText(const QString &)));
}

void
CQChartsViewSettings::
showQueryText(const QString &text)
{
  queryText_->setText(text);

  showQueryTab();
}

void
CQChartsViewSettings::
showQueryTab()
{
  tab_->setCurrentIndex(7);
}

//------

void
CQChartsViewSettings::
initErrorsFrame(QFrame *errorsFrame)
{
  auto *errorsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(errorsFrame, 2, 2);

  //---

  auto *view = window_->view();

  error_ = new CQChartsViewError(view);

  errorsFrameLayout->addWidget(error_);

  //---

  connect(view, SIGNAL(updateErrors()), this, SLOT(updateErrorsSlot()));
}

void
CQChartsViewSettings::
updateErrorsSlot()
{
  updateErrorsTimer_->start(250);
}

void
CQChartsViewSettings::
updateErrors()
{
  auto *view = window_->view();

  bool hasErrors = false;

  for (auto &plot : view->plots()) {
    if (plot->hasErrors()) {
      hasErrors = true;
      break;
    }
  }

  if (window_)
    window_->setHasErrors(hasErrors);

  error_->updatePlots();
}

void
CQChartsViewSettings::
showErrorsTab()
{
  tab_->setCurrentIndex(8);
}

//------

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

void
CQChartsViewSettings::
viewLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  auto *view = window_->view();
  if (! view) return;

  auto *image = layersWidgets_.viewLayerTable->selectedImage(view);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

void
CQChartsViewSettings::
plotLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  auto *plot = currentPlot();
  if (! plot) return;

  auto *image = layersWidgets_.plotLayerTable->selectedImage(plot);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

//------

CQChartsPropertyViewTree *
CQChartsViewSettings::
viewPropertyTree() const
{
  return propertiesWidgets_.viewPropertyTree->propertyTree();
}

CQChartsPropertyViewTree *
CQChartsViewSettings::
plotPropertyTree(CQChartsPlot *plot) const
{
  for (int i = 0; i < propertiesWidgets_.plotsTab->count(); ++i) {
    auto *plotWidget =
      qobject_cast<PlotPropertiesWidget *>(propertiesWidgets_.plotsTab->widget(i));
    assert(plotWidget);

    if (plotWidget->plot() == plot)
      return plotWidget->propertyTree();
  }

  return nullptr;
}

//------

// models changed
void
CQChartsViewSettings::
updateModels()
{
  auto *charts = window_->view()->charts();

  modelsWidgets_.modelTable->updateModels(charts);

  //---

  invalidateModelDetails(false);
}

// model data changed
void
CQChartsViewSettings::
updateModelsData()
{
  auto *charts = window_->view()->charts();

  modelsWidgets_.modelTable->updateModels(charts);

  //---

  invalidateModelDetails(true);
}

void
CQChartsViewSettings::
invalidateModelDetails(bool changed)
{
  auto *charts = window_->view()->charts();

  auto *modelData = charts->currentModelData();

  modelsWidgets_.detailsWidget->setModelData(modelData, /*invalidate*/changed);
}

void
CQChartsViewSettings::
modelsSelectionChangeSlot()
{
  auto *charts = window_->view()->charts();

  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind >= 0)
    charts->setCurrentModelInd(int(ind));

  modelsWidgets_.editButton  ->setEnabled(ind >= 0);
  modelsWidgets_.removeButton->setEnabled(ind >= 0);
  modelsWidgets_.plotButton  ->setEnabled(ind >= 0);
}

void
CQChartsViewSettings::
loadModelSlot()
{
  auto *charts = window_->view()->charts();

  (void) charts->loadModelDlg();
}

void
CQChartsViewSettings::
editModelSlot()
{
  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind < 0)
    return;

  auto *charts = window_->view()->charts();

  auto *modelData = charts->getModelDataByInd(int(ind));
  if (! modelData) return;

  charts->editModelDlg(modelData);
}

void
CQChartsViewSettings::
removeModelSlot()
{
  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind < 0)
    return;

  auto *charts = window_->view()->charts();

  auto *modelData = charts->getModelDataByInd(int(ind));
  if (! modelData) return;

  charts->removeModelData(modelData);
}

void
CQChartsViewSettings::
createPlotModelSlot()
{
  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind < 0)
    return;

  auto *charts = window_->view()->charts();

  auto *modelData = charts->getModelDataByInd(int(ind));
  if (! modelData) return;

  auto *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(window_->view()->id());
}

//------

void
CQChartsViewSettings::
createPlotSlot()
{
  auto *charts = window_->view()->charts();

  auto *modelData = charts->currentModelData();
  if (! modelData) return;

  auto *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(window_->view()->id());
}

//------

void
CQChartsViewSettings::
writeViewSlot()
{
  auto *view = window_->view();
  if (! view) return;

  auto dir = QDir::current().dirName() + "/view.tcl";

  auto fileName = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");
  if (! fileName.length()) return; // cancelled

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  //---

  view->writeAll(fs);
}

//------

void
CQChartsViewSettings::
plotsTabChangedSlot()
{
  int ind = propertiesWidgets_.plotsTab->currentIndex();
  if (ind < 0) return; // no plot

  auto *plotWidget =
    qobject_cast<PlotPropertiesWidget *>(propertiesWidgets_.plotsTab->widget(ind));
  assert(plotWidget);

//propertiesWidgets_.plotTip->setPlot(plotWidget->plot());
}

//------

void
CQChartsViewSettings::
updatePlots()
{
  auto *view = window_->view();

  //---

  using PlotSet = std::set<CQChartsPlot *>;

  int np = view->numPlots();

  PlotSet plotSet;

  for (int i = 0; i < np; ++i)
    plotSet.insert(view->plot(i));

  //---

  // add plots to plot table (id, type and state)
  plotsWidgets_.plotTable->updatePlots(view);

  //---

  auto findPlotTab = [&](CQChartsPlot *plot) {
    for (int i = 0; i < propertiesWidgets_.plotsTab->count(); ++i) {
      auto *plotWidget =
        qobject_cast<PlotPropertiesWidget *>(propertiesWidgets_.plotsTab->widget(i));
      assert(plotWidget);

      if (plotWidget->plot() == plot)
        return i;
    }

    return -1;
  };

  // add new plots to tabbed property view
  for (int i = 0; i < np; ++i) {
    auto *plot = view->plot(i);

    int ind = findPlotTab(plot);

    if (ind < 0) {
      auto *plotWidget = new PlotPropertiesWidget(this, plot);

      plotWidget->setObjectName(QString("plotTabWidget_%1").
        arg(propertiesWidgets_.plotsTab->count() + 1));

      connect(plotWidget, SIGNAL(propertyItemSelected(QObject *, const QString &)),
              this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

      ind = propertiesWidgets_.plotsTab->addTab(plotWidget, plot->id());

      //propertiesWidgets_.plotsTab->setTabToolTip(ind, plot->type()->description());
    }
  }

  // remove deleted plots from tab
  using PlotWidgets = std::vector<PlotPropertiesWidget *>;

  PlotWidgets plotWidgets;

  for (int i = 0; i < propertiesWidgets_.plotsTab->count(); ++i) {
    auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(propertiesWidgets_.plotsTab->widget(i));
    assert(plotWidget);

    if (plotSet.find(plotWidget->plot()) == plotSet.end())
      plotWidgets.push_back(plotWidget);
  }

  for (auto &plotWidget : plotWidgets) {
    int ind = propertiesWidgets_.plotsTab->indexOf(plotWidget);
    assert(ind >= 0);

    propertiesWidgets_.plotsTab->removeTab(ind);

    delete plotWidget;
  }

  //---

  plotsSelectionChangeSlot();
}

void
CQChartsViewSettings::
updateCurrentPlot()
{
  auto *view = window_->view();
  assert(view);

  if (plotId_.length()) {
    auto *plot = view->getPlotForId(plotId_);

    if (plot) {
      disconnect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
      disconnect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
      disconnect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
      disconnect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
    }
  }

  //---

  disconnect(plotsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
             this, SLOT(plotsSelectionChangeSlot()));

  int ind = view->currentPlotInd();

  plotsWidgets_.plotTable->setCurrentInd(ind);

  connect(plotsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotsSelectionChangeSlot()));

  //---

  auto *plot = view->currentPlot();

  plotId_ = (plot ? plot->id() : "");

  if (plot) {
    connect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
    connect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    connect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
    connect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
  }

  //---

  updatePlotControls();

  //---

  updateAnnotations();

  updatePlotObjects();

  updateLayers();
}

CQChartsPlot *
CQChartsViewSettings::
currentPlot(bool remap) const
{
  auto *view = window_->view();
  if (! view) return nullptr;

  return view->currentPlot(remap);
}

//---

CQChartsPlot *
CQChartsViewSettings::
getPropertiesPlot() const
{
  auto *plotWidget =
    qobject_cast<PlotPropertiesWidget *>(propertiesWidgets_.plotsTab->currentWidget());
  if (! plotWidget) return nullptr;

  return plotWidget->plot();
}

CQChartsPlot *
CQChartsViewSettings::
getSelectedPlot() const
{
  Plots plots;

  getSelectedPlots(plots);

  if (plots.size() == 1)
    return plots[0];

  return nullptr;
}

void
CQChartsViewSettings::
getSelectedPlots(Plots &plots) const
{
  auto *view = window_->view();

  plotsWidgets_.plotTable->getSelectedPlots(view, plots);
}

void
CQChartsViewSettings::
plotsSelectionChangeSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  plotsWidgets_.raiseButton ->setEnabled(plots.size() == 1);
  plotsWidgets_.lowerButton ->setEnabled(plots.size() == 1);
//plotsWidgets_.createButton->setEnabled(true);
  plotsWidgets_.removeButton->setEnabled(plots.size() > 0);

  //---

  auto *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &plot : plots) {
    if (! plot->isSelectable())
      continue;

    plot->setSelected(true);
  }

  view->endSelection();
}

void
CQChartsViewSettings::
updatePlotOverlayState()
{
  auto *obj = sender();

  if      (obj == plotsWidgets_.x1x2Check) {
    if (plotsWidgets_.x1x2Check->isChecked())
      plotsWidgets_.y1y2Check->setChecked(false);
  }
  else if (obj == plotsWidgets_.y1y2Check) {
    if (plotsWidgets_.y1y2Check->isChecked())
      plotsWidgets_.x1x2Check->setChecked(false);
  }
  else {
    if (plotsWidgets_.x1x2Check->isChecked() && plotsWidgets_.y1y2Check->isChecked())
      plotsWidgets_.y1y2Check->setChecked(false);
  }
}

void
CQChartsViewSettings::
editViewKeySlot()
{
  auto *view = window_->view();

  if (! view->key())
    return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(this, view->key());

  editKeyDlg_->show();
  editKeyDlg_->raise();
}

void
CQChartsViewSettings::
editPlotTitleSlot()
{
//auto *plot = getSelectedPlot();
  auto *plot = getPropertiesPlot();

  if (! plot || ! plot->title())
    return;

  if (editTitleDlg_)
    delete editTitleDlg_;

  editTitleDlg_ = new CQChartsEditTitleDlg(this, plot->title());

  editTitleDlg_->show();
  editTitleDlg_->raise();
}

void
CQChartsViewSettings::
editPlotKeySlot()
{
//auto *plot = getSelectedPlot();
  auto *plot = getPropertiesPlot();

  if (! plot || ! plot->key())
    return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(this, plot->key());

  editKeyDlg_->show();
  editKeyDlg_->raise();
}

void
CQChartsViewSettings::
editPlotXAxisSlot()
{
//auto *plot = getSelectedPlot();
  auto *plot = getPropertiesPlot();

  if (! plot || ! plot->xAxis())
    return;

  if (editXAxisDlg_)
    delete editXAxisDlg_;

  editXAxisDlg_ = new CQChartsEditAxisDlg(this, plot->xAxis());

  editXAxisDlg_->show();
  editXAxisDlg_->raise();
}

void
CQChartsViewSettings::
editPlotYAxisSlot()
{
//auto *plot = getSelectedPlot();
  auto *plot = getPropertiesPlot();

  if (! plot || ! plot->yAxis())
    return;

  if (editYAxisDlg_)
    delete editYAxisDlg_;

  editYAxisDlg_ = new CQChartsEditAxisDlg(this, plot->yAxis());

  editYAxisDlg_->show();
  editYAxisDlg_->raise();
}

void
CQChartsViewSettings::
groupPlotsSlot()
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool overlay = plotsWidgets_.overlayCheck->isChecked();
  bool x1x2    = plotsWidgets_.x1x2Check   ->isChecked();
  bool y1y2    = plotsWidgets_.y1y2Check   ->isChecked();

  if      (x1x2) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 (or more) plots for x1x2");
      return;
    }

    view->initX1X2(plots, overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 (or more) plots for y1y2");
      return;
    }

    view->initY1Y2(plots, overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 or more plots for overlay");
      return;
    }

    view->initOverlay(plots, /*reset*/true);
  }
  else {
    view->resetGrouping();
  }
}

void
CQChartsViewSettings::
placePlotsSlot()
{
  auto *view = window_->view();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool vertical   = plotsWidgets_.placeVerticalRadio  ->isChecked();
  bool horizontal = plotsWidgets_.placeHorizontalRadio->isChecked();
  int  rows       = plotsWidgets_.placeRowsEdit       ->value();
  int  columns    = plotsWidgets_.placeColumnsEdit    ->value();

  view->placePlots(plots, vertical, horizontal, rows, columns, /*reset*/false);
}

void
CQChartsViewSettings::
raisePlotSlot()
{
  auto *view = window_->view();
  auto *plot = getSelectedPlot();

  if (plot)
    view->raisePlot(plot);
}

void
CQChartsViewSettings::
lowerPlotSlot()
{
  auto *view = window_->view();
  auto *plot = getSelectedPlot();

  if (plot)
    view->lowerPlot(plot);
}

void
CQChartsViewSettings::
removePlotsSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  auto *view = window_->view();

  for (auto &plot : plots)
    view->removePlot(plot);

  //updateView();
}

void
CQChartsViewSettings::
writePlotSlot()
{
  auto *plot = getPropertiesPlot();
  if (! plot) return;

  auto dir = QDir::current().dirName() + "/plot.tcl";

  auto fileName = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");
  if (! fileName.length()) return; // cancelled

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  //---

  auto *view = plot->view();

  view->write(fs);

  auto *modelData = plot->getModelData();

  if (modelData)
    modelData->write(fs);

  plot->write(fs);
}

//------

void
CQChartsViewSettings::
updatePlotControls()
{
  // add controls for plot and child plots
  auto *plot = currentPlot();

  controlFrame_->setPlot(plot);

  controlFrame_->setPlotControls();

  //---

  if (controlFrame_->numIFaces() > 0) {
    if (! propertiesWidgets_.propertiesSplit->hasWidget(controlFrame_)) {
      propertiesWidgets_.propertiesSplit->addWidget(controlFrame_, "Quick Controls");

      controlFrame_->setVisible(true);

      int i1 = INT_MAX*0.4;
      int i3 = INT_MAX*0.1;
      int i2 = INT_MAX - i1 - i3;

      propertiesWidgets_.propertiesSplit->setSizes(QList<int>({i1, i2, i3}));
    }
  }
  else {
    if (propertiesWidgets_.propertiesSplit->hasWidget(controlFrame_)) {
      propertiesWidgets_.propertiesSplit->removeWidget(controlFrame_, /*delete*/false);

      controlFrame_->setVisible(false);

      int i1 = INT_MAX*0.4;
      int i2 = INT_MAX - i1;

      propertiesWidgets_.propertiesSplit->setSizes(QList<int>({i1, i2}));
    }
  }
}

//------

void
CQChartsViewSettings::
updateAnnotations()
{
  updateViewAnnotations();
  updatePlotAnnotations();
}

void
CQChartsViewSettings::
updateViewAnnotations()
{
  auto *view = window_->view();

  annotationsWidgets_.viewTable->updateAnnotations(view);
}

void
CQChartsViewSettings::
updatePlotAnnotations()
{
  auto *plot = currentPlot(/*remap*/false);

  QString plotName = (plot ? QString("Plot %1").arg(plot->id()) : "Plot");

  annotationsWidgets_.split->setWidgetName(annotationsWidgets_.plotFrame, plotName);

  annotationsWidgets_.plotTable->updateAnnotations(plot);
}

void
CQChartsViewSettings::
viewAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  bool anyViewAnnotations = (viewAnnotations.size() > 0);

  annotationsWidgets_.viewRaiseButton ->setEnabled(anyViewAnnotations);
  annotationsWidgets_.viewLowerButton ->setEnabled(anyViewAnnotations);
  annotationsWidgets_.viewEditButton  ->setEnabled(anyViewAnnotations);
  annotationsWidgets_.viewRemoveButton->setEnabled(anyViewAnnotations);

  // deselect all plot annotations (needed ?)
  if (anyViewAnnotations) {
    CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
      annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
      this, SLOT(plotAnnotationSelectionChangeSlot()));

    annotationsWidgets_.plotTable->selectionModel()->clear();
  }

  //---

  {
  auto *view = window_->view();

  CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
    annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
    this, SLOT(viewAnnotationSelectionChangeSlot()));
  CQChartsWidgetUtil::AutoDisconnect annotationsDisconnect(
    view, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : viewAnnotations) {
    if (annotation->isSelectable())
      annotation->setSelected(true);
  }

  view->endSelection();
  }
}

void
CQChartsViewSettings::
plotAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  bool anyPlotAnnotations = (plotAnnotations.size() > 0);

  annotationsWidgets_.plotRaiseButton ->setEnabled(anyPlotAnnotations);
  annotationsWidgets_.plotLowerButton ->setEnabled(anyPlotAnnotations);
  annotationsWidgets_.plotEditButton  ->setEnabled(anyPlotAnnotations);
  annotationsWidgets_.plotRemoveButton->setEnabled(anyPlotAnnotations);

  // deselect all view annotations (needed ?)
  if (anyPlotAnnotations) {
    CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
      annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
      this, SLOT(viewAnnotationSelectionChangeSlot()));

    annotationsWidgets_.viewTable->selectionModel()->clear();
  }

  //---

  {
  auto *view = window_->view();
  auto *plot = currentPlot(/*remap*/false);

  CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
    annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
    this, SLOT(plotAnnotationSelectionChangeSlot()));
  CQChartsWidgetUtil::AutoDisconnect annotationsDisconnect(
    plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : plotAnnotations) {
    if (annotation->isSelectable())
      annotation->setSelected(true);
  }

  view->endSelection();
  }
}

void
CQChartsViewSettings::
getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const
{
  annotationsWidgets_.viewTable->getSelectedAnnotations(viewAnnotations);

  //---

  auto *plot = currentPlot(/*remap*/false);

  if (plot)
    annotationsWidgets_.plotTable->getSelectedAnnotations(plotAnnotations);
}

CQChartsAnnotation *
CQChartsViewSettings::
getSelectedViewAnnotation() const
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  return (! viewAnnotations.empty() ? viewAnnotations[0] : nullptr);
}

CQChartsAnnotation *
CQChartsViewSettings::
getSelectedPlotAnnotation() const
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  return (! plotAnnotations.empty() ? plotAnnotations[0] : nullptr);
}

//---

void
CQChartsViewSettings::
raiseViewAnnotationSlot()
{
  auto *annotation = getSelectedViewAnnotation();
  if (! annotation) return;

  annotation->view()->raiseAnnotation(annotation);

  updateViewAnnotations();
}

void
CQChartsViewSettings::
lowerViewAnnotationSlot()
{
  auto *annotation = getSelectedViewAnnotation();
  if (! annotation) return;

  annotation->view()->lowerAnnotation(annotation);

  updateViewAnnotations();
}

void
CQChartsViewSettings::
raisePlotAnnotationSlot()
{
  auto *annotation = getSelectedPlotAnnotation();
  if (! annotation) return;

  annotation->plot()->raiseAnnotation(annotation);

  updatePlotAnnotations();
}

void
CQChartsViewSettings::
lowerPlotAnnotationSlot()
{
  auto *annotation = getSelectedPlotAnnotation();
  if (! annotation) return;

  annotation->plot()->lowerAnnotation(annotation);

  updatePlotAnnotations();
}

void
CQChartsViewSettings::
createViewAnnotationSlot()
{
  auto *view = window_->view();

  if (createAnnotationDlg_)
    delete createAnnotationDlg_;

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(this, view);

  createAnnotationDlg_->show();
  createAnnotationDlg_->raise();
}

void
CQChartsViewSettings::
createPlotAnnotationSlot()
{
  auto *plot = currentPlot(/*remap*/false);
  if (! plot) return;

  if (createAnnotationDlg_)
    delete createAnnotationDlg_;

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(this, plot);

  createAnnotationDlg_->show();
  createAnnotationDlg_->raise();
}

void
CQChartsViewSettings::
editViewAnnotationSlot()
{
  auto *annotation = getSelectedViewAnnotation();
  if (! annotation) return;

  if (editAnnotationDlg_)
    delete editAnnotationDlg_;

  editAnnotationDlg_ = new CQChartsEditAnnotationDlg(this, annotation);

  editAnnotationDlg_->show();
  editAnnotationDlg_->raise();
}

void
CQChartsViewSettings::
editPlotAnnotationSlot()
{
  auto *annotation = getSelectedPlotAnnotation();
  if (! annotation) return;

  if (editAnnotationDlg_)
    delete editAnnotationDlg_;

  editAnnotationDlg_ = new CQChartsEditAnnotationDlg(this, annotation);

  editAnnotationDlg_->show();
  editAnnotationDlg_->raise();
}

void
CQChartsViewSettings::
removeViewAnnotationsSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  auto *view = window_->view();
  if (! view) return;

  for (const auto &annotation : viewAnnotations)
    view->removeAnnotation(annotation);

  updateView();
}

void
CQChartsViewSettings::
removePlotAnnotationsSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  auto *plot = currentPlot(/*remap*/false);
  if (! plot) return;

  for (const auto &annotation : plotAnnotations)
    plot->removeAnnotation(annotation);

  updateView();
}

void
CQChartsViewSettings::
writeAnnotationSlot()
{
  auto *view = window_->view();
  if (! view) return;

  auto dir = QDir::current().dirName() + "/annotation.tcl";

  auto fileName = QFileDialog::getSaveFileName(this, "Write Annotations", dir, "Files (*.tcl)");
  if (! fileName.length()) return; // cancelled

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  //---

  const auto &viewAnnotations = view->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(fs);

  //---

  CQChartsView::Plots plots;

  view->getPlots(plots);

  for (const auto &plot : plots) {
    const auto &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(fs);
  }
}

//------

void
CQChartsViewSettings::
updatePlotObjects()
{
  objectsWidgets_.propertyTree->propertyTree()->setPropertyModel(nullptr);

  delete objectsWidgets_.propertyModel;

  objectsWidgets_.propertyModel = nullptr;

  //---

  auto *plot = currentPlot(/*remap*/false);

  if (plot) {
    objectsWidgets_.propertyModel = new CQPropertyViewModel;

    auto objs = plot->plotObjects();

    if (int(objs.size()) > maxPlotObjs_) {
      CQChartsPlot::PlotObjs majorPlotObjs;

      for (auto &obj : objs) {
        if (obj->detailHint() == CQChartsPlotObj::DetailHint::MAJOR)
          majorPlotObjs.push_back(obj);
      }

      if (int(majorPlotObjs.size()) > maxPlotObjs_)
        return;

      objs = majorPlotObjs;
    }

    for (auto &obj : objs)
      obj->addProperties(objectsWidgets_.propertyModel, obj->id());
  }

  objectsWidgets_.propertyTree->propertyTree()->setPropertyModel(objectsWidgets_.propertyModel);
}

//------

void
CQChartsViewSettings::
updateSelection()
{
  window_->selectPropertyObjects();
}

//------

void
CQChartsViewSettings::
palettesComboSlot(int)
{
  auto name = themeWidgets_.palettesCombo->currentText();

  auto *palette = CQColorsMgrInst->getNamedPalette(name);

  themeWidgets_.palettesPlot->setPalette(palette);

  themeWidgets_.palettesControl->updateState();
}

void
CQChartsViewSettings::
palettesResetSlot()
{
  auto name = themeWidgets_.palettesCombo->currentText();

  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  if (! palette) return;

  CQColorsMgrInst->resetPalette(name);

  updatePalettes();
}

void
CQChartsViewSettings::
paletteColorsChangedSlot()
{
  updateView();
}

void
CQChartsViewSettings::
updatePalettes()
{
  updatePaletteWidgets();

  auto name = themeWidgets_.palettesCombo->currentText();

  auto *palette = CQColorsMgrInst->getNamedPalette(name);

  themeWidgets_.palettesPlot->setPalette(palette);

  themeWidgets_.palettesControl->updateState();

  updateView();
}

void
CQChartsViewSettings::
updatePaletteWidgets()
{
  themeWidgets_.palettesCombo->setEnabled(true);
}

void
CQChartsViewSettings::
updateView()
{
  auto *view = window_->view();

  view->updatePlots();
}

void
CQChartsViewSettings::
updateInterface()
{
  auto *view    = window_->view();
  auto *palette = view->interfacePalette();

  themeWidgets_.interfacePlot->setPalette(palette);

  themeWidgets_.interfaceControl->updateState();
}

//------

void
CQChartsViewSettings::
updateLayers()
{
  layersWidgets_.viewLayerTable->initLayers();
  layersWidgets_.plotLayerTable->initLayers();

  //---

  auto *view = window_->view();

  if (view)
    layersWidgets_.viewLayerTable->updateLayers(view);

  auto *plot = currentPlot();

  if (plot)
    layersWidgets_.plotLayerTable->updateLayers(plot);
}

void
CQChartsViewSettings::
viewLayersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
viewLayersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  auto *view = window_->view();

  if (row == 0)
    view->invalidateObjects();
  else
    view->invalidateOverlay();
}

void
CQChartsViewSettings::
plotLayersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
plotLayersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  auto *plot = currentPlot();
  if (! plot) return;

  CQChartsLayer::Type type;
  bool                active;

  if (! layersWidgets_.plotLayerTable->getLayerState(plot, row, type, active))
    return;

  auto *layer = plot->getLayer(type);
  if (! layer) return;

  plot->setLayerActive(type, active);

  const auto *buffer = plot->getBuffer(layer->buffer());

  if (buffer->type() != CQChartsBuffer::Type::MIDDLE)
    plot->invalidateLayer(buffer->type());
  else
    plot->drawObjs();
}

//------

CQChartsViewSettingsViewPropertiesWidget::
CQChartsViewSettingsViewPropertiesWidget(CQChartsViewSettings *settings, CQChartsView *view) :
 view_(view)
{
  setObjectName("viewPropertiesWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  propertyTree_ = new CQChartsPropertyViewTree(settings, view->propertyModel());

  propertyTree_->setObjectName("propertyTree");

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsViewSettingsFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsViewSettingsViewPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}

//------

CQChartsViewSettingsPlotPropertiesWidget::
CQChartsViewSettingsPlotPropertiesWidget(CQChartsViewSettings *settings, CQChartsPlot *plot) :
 plot_(plot)
{
  setObjectName("plotPropertiesWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  propertyTree_ = new CQChartsPropertyViewTree(settings, plot_->propertyModel());

  propertyTree_->setObjectName("propertyTree");

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsViewSettingsFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsViewSettingsPlotPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}

//------

CQChartsViewSettingsObjectPropertiesWidget::
CQChartsViewSettingsObjectPropertiesWidget(CQChartsViewSettings *settings)
{
  setObjectName("objectPropertiesWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  propertyTree_ = new CQChartsPropertyViewTree(settings, nullptr);

  propertyTree_->setObjectName("propertyTree");

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsViewSettingsFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsViewSettingsObjectPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}

//------

CQChartsViewSettingsFilterEdit::
CQChartsViewSettingsFilterEdit(CQChartsPropertyViewTree *tree) :
 tree_(tree)
{
  setObjectName("filterEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(escapePressed()),
          this, SLOT(hideFilterSlot()));

  connect(filterEdit_, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  layout->addWidget(filterEdit_);

  setFocusProxy(filterEdit_);
}

void
CQChartsViewSettingsFilterEdit::
replaceFilterSlot(const QString &text)
{
  tree_->setFilter(text);
}

void
CQChartsViewSettingsFilterEdit::
addFilterSlot(const QString &text)
{
//tree_->addFilter(text);
  tree_->setFilter(text);
}

void
CQChartsViewSettingsFilterEdit::
replaceSearchSlot(const QString &text)
{
  tree_->search(text);
}

void
CQChartsViewSettingsFilterEdit::
addSearchSlot(const QString &text)
{
//tree_->addSearch(text);
  tree_->search(text);
}

void
CQChartsViewSettingsFilterEdit::
hideFilterSlot()
{
  tree_->setFilterDisplayed(false);
}
