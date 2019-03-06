#include <CQChartsViewSettings.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsGradientPaletteCanvas.h>
#include <CQChartsGradientPaletteControl.h>
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
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>

#include <CQPropertyViewItem.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>
#include <CQGroupBox.h>

#include <QTabWidget>
#include <QTextBrowser>
#include <QHeaderView>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

class CQChartsViewSettingsModelTable : public QTableWidget {
 public:
  CQChartsViewSettingsModelTable() {
    setObjectName("modelTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updateModels(CQCharts *charts) {
    CQCharts::ModelDatas modelDatas;

    charts->getModelDatas(modelDatas);

    clear();

    setColumnCount(2);
    setRowCount(modelDatas.size());

    setHorizontalHeaderItem(0, new QTableWidgetItem("Name" ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Index"));

    int i = 0;

    for (const auto &modelData : modelDatas) {
      QTableWidgetItem *nameItem = new QTableWidgetItem(modelData->id());
      QTableWidgetItem *indItem  = new QTableWidgetItem(QString("%1").arg(modelData->ind()));

      setItem(i, 0, nameItem);
      setItem(i, 1, indItem );

      nameItem->setData(Qt::UserRole, modelData->ind());

      ++i;
    }
  }

  long selectedModel() const {
    QList<QTableWidgetItem *> items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      QTableWidgetItem *item = items[i];
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

class CQChartsViewSettingsPlotTable : public QTableWidget {
 public:
  CQChartsViewSettingsPlotTable() {
    setObjectName("plotTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updatePlots(CQChartsView *view) {
    // add plots to plot table (id, type and state)
    clear();

    int np = view->numPlots();

    setColumnCount(3);
    setRowCount(np);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Id"   ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Type" ));
    setHorizontalHeaderItem(2, new QTableWidgetItem("State"));

    for (int i = 0; i < np; ++i) {
      CQChartsPlot *plot = view->plot(i);

      //--

      // set id item store plot index in user data
      QTableWidgetItem *idItem = new QTableWidgetItem(plot->id());

      setItem(i, 0, idItem);

      int ind = view->plotInd(plot);

      idItem->setData(Qt::UserRole, ind);

      //--

      // set type item
      QTableWidgetItem *typeItem = new QTableWidgetItem(plot->type()->name());

      setItem(i, 1, typeItem);

      //--

      // set state item
      QStringList states;

      if (plot->isOverlay()) states += "overlay";
      if (plot->isX1X2   ()) states += "x1x2";
      if (plot->isY1Y2   ()) states += "y1y2";

      QString stateStr = states.join("|");

      if (stateStr == "")
        stateStr = "normal";

      QTableWidgetItem *stateItem = new QTableWidgetItem(stateStr);

      setItem(i, 2, stateItem);
    }
  }

  void setCurrentInd(int ind) {
    int nr = rowCount();

    for (int i = 0; i < nr; ++i) {
      QTableWidgetItem *item = this->item(i, 0);

      bool ok;

      long ind1 = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

      item->setSelected(ind1 == ind);
    }
  }

  void getSelectedPlots(CQChartsView *view, std::vector<CQChartsPlot *> &plots) {
    QList<QTableWidgetItem *> items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      QTableWidgetItem *item = items[i];
      if (item->column() != 0) continue;

      QString id = item->text();

      CQChartsPlot *plot = view->getPlot(id);

      plots.push_back(plot);
    }
  }
};

//---

class CQChartsViewSettingsViewAnnotationsTable : public QTableWidget {
 public:
  CQChartsViewSettingsViewAnnotationsTable() {
    setObjectName("viewTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updateAnnotations(CQChartsView *view) {
    clear();

    const CQChartsView::Annotations &viewAnnotations = view->annotations();

    int nv = viewAnnotations.size();

    setColumnCount(2);
    setRowCount(nv);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Id"  ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));

    for (int i = 0; i < nv; ++i) {
      CQChartsAnnotation *annotation = viewAnnotations[i];

      QTableWidgetItem *idItem = new QTableWidgetItem(annotation->id());

      setItem(i, 0, idItem);

      int ind = annotation->ind();

      idItem->setData(Qt::UserRole, ind);

      QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

      setItem(i, 1, typeItem);
    }
  }

  void getSelectedAnnotations(CQChartsView *view, std::vector<CQChartsAnnotation *> &annotations) {
    QList<QTableWidgetItem *> items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      QTableWidgetItem *item = items[i];
      if (item->column() != 0) continue;

      QString id = item->text();

      CQChartsAnnotation *annotation = view->getAnnotationByName(id);

      if (annotation)
        annotations.push_back(annotation);
    }
  }
};

//---

class CQChartsViewSettingsPlotAnnotationsTable : public QTableWidget {
 public:
  CQChartsViewSettingsPlotAnnotationsTable() {
    setObjectName("plotTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  void updateAnnotations(CQChartsPlot *plot) {
    clear();

    if (! plot)
      return;

    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    int np = plotAnnotations.size();

    setColumnCount(2);
    setRowCount(np);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Id"  ));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));

    for (int i = 0; i < np; ++i) {
      CQChartsAnnotation *annotation = plotAnnotations[i];

      QTableWidgetItem *idItem = new QTableWidgetItem(annotation->id());

      setItem(i, 0, idItem);

      int ind = annotation->ind();

      idItem->setData(Qt::UserRole, ind);

      QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

      setItem(i, 1, typeItem);
    }
  }

  void getSelectedAnnotations(CQChartsPlot *plot, std::vector<CQChartsAnnotation *> &annotations) {
    QList<QTableWidgetItem *> items = selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      QTableWidgetItem *item = items[i];
      if (item->column() != 0) continue;

      QString id = item->text();

      CQChartsAnnotation *annotation = plot->getAnnotationByName(id);

      if (annotation)
        annotations.push_back(annotation);
    }
  }
};

//---

class CQChartsViewSettingsLayerTable : public QTableWidget {
 public:
  CQChartsViewSettingsLayerTable() {
    setObjectName("layerTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  QImage *selectedImage(CQChartsPlot *plot) const {
    if (! plot) return nullptr;

    QList<QTableWidgetItem *> items = selectedItems();
    if (items.length() <= 0) return nullptr;

    QTableWidgetItem *item = items[0];

    bool ok;

    long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
    if (! ok) return nullptr;

    CQChartsLayer *layer = plot->getLayer((CQChartsLayer::Type) l);
    if (! layer) return nullptr;

    CQChartsBuffer *buffer = plot->getBuffer(layer->buffer());
    if (! buffer) return nullptr;

    QImage *image = buffer->image();

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

    for (int l = l1; l <= l2; ++l) {
      int i = l - l1;

      CQChartsLayer::Type type = (CQChartsLayer::Type) l;

      QString name = CQChartsLayer::typeName(type);

      QTableWidgetItem *idItem = new QTableWidgetItem(name);

      setItem(i, 0, idItem);

      idItem->setData(Qt::UserRole, l);

      QTableWidgetItem *stateItem = new QTableWidgetItem("");

    //stateItem->setFlags(stateItem->flags() | Qt::ItemIsEnabled);
      stateItem->setFlags(stateItem->flags() | Qt::ItemIsUserCheckable);
    //stateItem->setFlags(stateItem->flags() & ! Qt::ItemIsEditable);

      setItem(i, 1, stateItem);

      QTableWidgetItem *rectItem = new QTableWidgetItem("");

      setItem(i, 2, rectItem);
    }
  }

  void updateLayers(CQChartsPlot *plot) {
    int l1 = (int) CQChartsLayer::firstLayer();
    int l2 = (int) CQChartsLayer::lastLayer ();

    for (int l = l1; l <= l2; ++l) {
      int i = l - l1;

      CQChartsLayer::Type type = (CQChartsLayer::Type) l;

      CQChartsLayer *layer = plot->getLayer(type);

      const CQChartsBuffer *buffer = (layer ? plot->getBuffer(layer->buffer()) : nullptr);

  //  QTableWidgetItem *idItem    = item(i, 0);
      QTableWidgetItem *stateItem = item(i, 1);
      QTableWidgetItem *rectItem  = item(i, 2);

      QStringList states;

      if (layer  && layer ->isActive()) states += "active";
      if (buffer && buffer->isValid ()) states += "valid";

      stateItem->setText(states.join("|"));

      stateItem->setCheckState((layer && layer->isActive()) ? Qt::Checked : Qt::Unchecked);

      QRectF rect = (buffer ? buffer->rect() : QRectF());

      QString rectStr = QString("X:%1, Y:%2, W:%3, H:%4").
                          arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());

      rectItem->setText(rectStr);
    }
  }

  bool getLayerState(CQChartsPlot *plot, int row, CQChartsLayer::Type &type, bool &active) {
    QTableWidgetItem *nameItem = item(row, 0);

    QString name = nameItem->text();

    type = CQChartsLayer::nameType(name);

    CQChartsLayer *layer = plot->getLayer(type);
    if (! layer) return false;

    QTableWidgetItem *stateItem = item(row, 1);

    active = (stateItem->checkState() == Qt::Checked);

    return true;
  }
};

//------

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(updateModels()));
  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(invalidateModelDetails()));
  connect(charts, SIGNAL(modelNameChanged(const QString &)), this, SLOT(updateModels()));

  connect(view, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(view, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  connect(view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

  connect(window, SIGNAL(themePalettesChanged()), this, SLOT(updatePalettes()));
  connect(window, SIGNAL(interfacePaletteChanged()), this, SLOT(updateInterface()));

  //--

  setObjectName("settings");

  setAutoFillBackground(true);

  addWidgets();
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
}

void
CQChartsViewSettings::
addWidgets()
{
  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  tab_ = CQUtil::makeWidget<QTabWidget>("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //----

  // Properties Tab
  QFrame *propertiesFrame = CQUtil::makeWidget<QFrame>("propertiesFrame");

  tab_->addTab(propertiesFrame, "Properties");

  initPropertiesFrame(propertiesFrame);

  //--

  // Models Tab
  QFrame *modelsFrame = CQUtil::makeWidget<QFrame>("modelsFrame");

  tab_->addTab(modelsFrame, "Models");

  initModelsFrame(modelsFrame);

  //--

  // Plots Tab
  QFrame *plotsFrame = CQUtil::makeWidget<QFrame>("plotsFrame");

  tab_->addTab(plotsFrame, "Plots");

  initPlotsFrame(plotsFrame);

  //--

  // Annotations Tab
  QFrame *annotationsFrame = CQUtil::makeWidget<QFrame>("annotationsFrame");

  tab_->addTab(annotationsFrame, "Annotations");

  initAnnotationsFrame(annotationsFrame);

  //--

  // Theme Tab
  QFrame *themeFrame = CQUtil::makeWidget<QFrame>("themeFrame");

  tab_->addTab(themeFrame, "Theme");

  initThemeFrame(themeFrame);

  //--

  // Layers Tab
  QFrame *layersFrame = CQUtil::makeWidget<QFrame>("layersFrame");

  tab_->addTab(layersFrame, "Layers");

  initLayersFrame(layersFrame);

  //----

  updateModels();

  updateAnnotations();
}

//------

void
CQChartsViewSettings::
initPropertiesFrame(QFrame *propertiesFrame)
{
  auto createPushButton = [&](const QString &label, const QString &objName,
                              const QString &tipStr, const char *slotName) {
    QPushButton *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tipStr);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  //---

  CQChartsView *view = window_->view();

  QVBoxLayout *propertiesLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //--

  QSplitter *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  splitter->setOrientation(Qt::Vertical);

  propertiesLayout->addWidget(splitter);

  //----

  // Create View Properties Group
  CQGroupBox *viewGroup = CQUtil::makeLabelWidget<CQGroupBox>("View", "viewGroup");

  QVBoxLayout *viewGroupLayout = CQUtil::makeLayout<QVBoxLayout>(viewGroup, 2, 2);

  splitter->addWidget(viewGroup);

  //--

  propertiesWidgets_.viewPropertyTree =
    new CQChartsPropertyViewTree(this, view->propertyModel());

  connect(viewPropertyTree(), SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  //--

  propertiesWidgets_.viewFilterEdit =
    new CQChartsViewSettingsFilterEdit(viewPropertyTree());

  //--

  viewGroupLayout->addWidget(propertiesWidgets_.viewFilterEdit);
  viewGroupLayout->addWidget(viewPropertyTree());

  //--

  QFrame *viewEditFrame = CQUtil::makeWidget<QFrame>("viewEditFrame");

  QHBoxLayout *viewEditFrameLayout = CQUtil::makeLayout<QHBoxLayout>(viewEditFrame, 2, 2);

  QPushButton *viewKeyButton =
    createPushButton("Key"  , "key"  , "Edit View Key"    , SLOT(editViewKeySlot()));
  QPushButton *viewWriteButton =
    createPushButton("Write", "write", "Write View Script", SLOT(writeViewSlot()));

  viewEditFrameLayout->addWidget(viewKeyButton);
  viewEditFrameLayout->addWidget(viewWriteButton);
  viewEditFrameLayout->addStretch(1);

  viewGroupLayout->addWidget(viewEditFrame);

  //----

  // Create Plots Properties Group
  CQGroupBox *plotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Plots", "plotsGroup");

  QVBoxLayout *plotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotsGroup, 2, 2);

  splitter->addWidget(plotsGroup);

  //--

  propertiesWidgets_.plotsTab = CQUtil::makeWidget<QTabWidget>("tab");

  plotsGroupLayout->addWidget(propertiesWidgets_.plotsTab);

  //--

  QFrame *plotEditFrame = CQUtil::makeWidget<QFrame>("plotEditFrame");

  QHBoxLayout *plotEditFrameLayout = CQUtil::makeLayout<QHBoxLayout>(plotEditFrame, 2, 2);

  QPushButton *plotTitleButton =
    createPushButton("Title" , "title", "Edit Plot Title"  , SLOT(editPlotTitleSlot()));
  QPushButton *plotKeyButton   =
    createPushButton("Key"   , "key"  , "Edit Plot Key"    , SLOT(editPlotKeySlot()));
  QPushButton *plotXAxisButton =
    createPushButton("X Axis", "xaxis", "Edit Plot X Axis" , SLOT(editPlotXAxisSlot()));
  QPushButton *plotYAxisButton =
    createPushButton("Y Axis", "yaxis", "Edit Plot Y Axis" , SLOT(editPlotYAxisSlot()));
  QPushButton *plotWriteButton =
    createPushButton("Write" , "write", "Write Plot Script", SLOT(writePlotSlot()));

  plotEditFrameLayout->addWidget(plotTitleButton);
  plotEditFrameLayout->addWidget(plotKeyButton);
  plotEditFrameLayout->addWidget(plotXAxisButton);
  plotEditFrameLayout->addWidget(plotYAxisButton);
  plotEditFrameLayout->addWidget(plotWriteButton);
  plotEditFrameLayout->addStretch(1);

  plotsGroupLayout->addWidget(plotEditFrame);

  //--

  splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  QVBoxLayout *modelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsFrame, 2, 2);

  //----

  QSplitter *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  splitter->setOrientation(Qt::Vertical);

  modelsFrameLayout->addWidget(splitter);

  //----

  // Model Table
  CQGroupBox *modelsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Models", "modelsGroup");

  QVBoxLayout *modelsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(modelsGroup, 2, 2);

  splitter->addWidget(modelsGroup);

  //--

  modelsWidgets_.modelTable = new CQChartsViewSettingsModelTable;

  connect(modelsWidgets_.modelTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(modelsSelectionChangeSlot()));

  modelsGroupLayout->addWidget(modelsWidgets_.modelTable);

  //----

  // Model Details
  modelsWidgets_.detailsGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("Details", "detailsGroup");

  QVBoxLayout *detailsGroupLayout =
    CQUtil::makeLayout<QVBoxLayout>(modelsWidgets_.detailsGroup, 2, 2);

  splitter->addWidget(modelsWidgets_.detailsGroup);

  //--

  CQChartsView *view   = window_->view();
  CQCharts     *charts = view->charts();

  modelsWidgets_.detailsWidget = new CQChartsModelDetailsWidget(charts);

  detailsGroupLayout->addWidget(modelsWidgets_.detailsWidget);

  //----

  // Model Buttons
  QHBoxLayout *modelControlLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  modelsFrameLayout->addLayout(modelControlLayout);

  QPushButton *loadModelButton = CQUtil::makeLabelWidget<QPushButton>("Load", "load");

  connect(loadModelButton, SIGNAL(clicked()), this, SLOT(loadModelSlot()));

  modelControlLayout->addWidget(loadModelButton);
  modelControlLayout->addStretch(1);

  //--

  splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initPlotsFrame(QFrame *plotsFrame)
{
  QVBoxLayout *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  //----

  CQGroupBox *plotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Plots", "plotsGroup");

  QVBoxLayout *plotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotsGroup, 2, 2);

  plotsFrameLayout->addWidget(plotsGroup);

  //--

  plotsWidgets_.plotTable = new CQChartsViewSettingsPlotTable;

  connect(plotsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotsSelectionChangeSlot()));

  plotsGroupLayout->addWidget(plotsWidgets_.plotTable);

  //----

#if 0
  QFrame *editFrame = CQUtil::makeWidget<QFrame>("editFrame");

  QHBoxLayout *editLayout = CQUtil::makeLayout<QHBoxLayout>(editFrame, 2, 2);

  QPushButton *titleButton = CQUtil::makeLabelWidget<QPushButton>("Title" , "title");
  QPushButton *keyButton   = CQUtil::makeLabelWidget<QPushButton>("Key"   , "key");
  QPushButton *xAxisButton = CQUtil::makeLabelWidget<QPushButton>("X Axis", "xaxis");
  QPushButton *yAxisButton = CQUtil::makeLabelWidget<QPushButton>("Y Axis", "yaxis");

  connect(titleButton, SIGNAL(clicked()), this, SLOT(editPlotTitleSlot()));
  connect(keyButton, SIGNAL(clicked()), this, SLOT(editPlotKeySlot()));
  connect(xAxisButton, SIGNAL(clicked()), this, SLOT(editPlotXAxisSlot()));
  connect(yAxisButton, SIGNAL(clicked()), this, SLOT(editPlotYAxisSlot()));

  editLayout->addWidget(titleButton);
  editLayout->addWidget(keyButton);
  editLayout->addWidget(xAxisButton);
  editLayout->addWidget(yAxisButton);
  editLayout->addStretch(1);

  plotsFrameLayout->addWidget(editFrame);
#endif

  //----

  CQGroupBox *groupPlotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Group", "groupPlotsGroup");

  QVBoxLayout *groupPlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(groupPlotsGroup, 2, 2);

  plotsFrameLayout->addWidget(groupPlotsGroup);

  //----

  QHBoxLayout *groupPlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsCheckLayout);

  //--

  plotsWidgets_.overlayCheck = CQUtil::makeLabelWidget<QCheckBox>("Overlay", "overlay");
  plotsWidgets_.x1x2Check    = CQUtil::makeLabelWidget<QCheckBox>("X1/X2"  , "x1x2");
  plotsWidgets_.y1y2Check    = CQUtil::makeLabelWidget<QCheckBox>("Y1/Y2"  , "y1y2");

  groupPlotsCheckLayout->addWidget(plotsWidgets_.overlayCheck);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.x1x2Check);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.y1y2Check);
  groupPlotsCheckLayout->addStretch(1);

  //----

  QHBoxLayout *groupPlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsButtonsLayout);

  //--

  QPushButton *groupApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  connect(groupApplyButton, SIGNAL(clicked()), this, SLOT(groupPlotsSlot()));

  groupPlotsButtonsLayout->addWidget(groupApplyButton);

  //--

  groupPlotsButtonsLayout->addStretch(1);

  //----

  CQGroupBox *placePlotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Place", "placePlotsGroup");

  QVBoxLayout *placePlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(placePlotsGroup, 2, 2);

  plotsFrameLayout->addWidget(placePlotsGroup);

  //--

  QHBoxLayout *placePlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsCheckLayout);

  plotsWidgets_.placeVerticalRadio   =
    CQUtil::makeLabelWidget<QRadioButton>("Vertical", "vertical");
  plotsWidgets_.placeHorizontalRadio =
    CQUtil::makeLabelWidget<QRadioButton>("Horizontal", "horizontal");
  plotsWidgets_.placeGridRadio       =
    CQUtil::makeLabelWidget<QRadioButton>("Grid", "grid");

  plotsWidgets_.placeVerticalRadio->setChecked(true);

  placePlotsCheckLayout->addWidget(plotsWidgets_.placeVerticalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeHorizontalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeGridRadio);

  placePlotsCheckLayout->addStretch(1);

  //--

  QHBoxLayout *placePlotsGridLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsGridLayout);

  plotsWidgets_.placeRowsEdit    = CQUtil::makeWidget<CQIntegerSpin>("rowsEdit");
  plotsWidgets_.placeColumnsEdit = CQUtil::makeWidget<CQIntegerSpin>("columnsEdit");

  plotsWidgets_.placeRowsEdit   ->setValue(1);
  plotsWidgets_.placeColumnsEdit->setValue(1);

  plotsWidgets_.placeRowsEdit   ->setMinimum(1);
  plotsWidgets_.placeColumnsEdit->setMinimum(1);

  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Rows", "row"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeRowsEdit);
  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Columns", "columns"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeColumnsEdit);
  placePlotsGridLayout->addStretch(1);

  //--

  QHBoxLayout *placePlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  placePlotsGroupLayout->addLayout(placePlotsButtonsLayout);

  QPushButton *placeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  placePlotsButtonsLayout->addWidget(placeApplyButton);
  placePlotsButtonsLayout->addStretch(1);

  connect(placeApplyButton, SIGNAL(clicked()), this, SLOT(placePlotsSlot()));

  //----

  auto createPushButton = [&](const QString &label, const QString &objName, const char *slotName) {
    QPushButton *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  CQGroupBox *controlPlotsGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("Control", "controlPlotsGroup");

  plotsFrameLayout->addWidget(controlPlotsGroup);

  QHBoxLayout *controlPlotsGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlPlotsGroup, 2, 2);

  plotsWidgets_.raiseButton      = createPushButton("Raise" , "raise" , SLOT(raisePlotSlot()));
  plotsWidgets_.lowerButton      = createPushButton("Lower" , "lower" , SLOT(lowerPlotSlot()));
  QPushButton*  createPlotButton = createPushButton("Create", "create", SLOT(createPlotSlot()));
  plotsWidgets_.removeButton     = createPushButton("Remove", "remove", SLOT(removePlotsSlot()));
//QPushButton*  writePlotButton  = createPushButton("Write" , "write" , SLOT(writePlotSlot()));

  plotsWidgets_.raiseButton ->setEnabled(false);
  plotsWidgets_.lowerButton ->setEnabled(false);
  plotsWidgets_.removeButton->setEnabled(false);

  controlPlotsGroupLayout->addWidget(plotsWidgets_.raiseButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.lowerButton);
  controlPlotsGroupLayout->addWidget(createPlotButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.removeButton);
//controlPlotsGroupLayout->addWidget(writePlotButton);

  //---

  controlPlotsGroupLayout->addStretch(1);
}

void
CQChartsViewSettings::
initAnnotationsFrame(QFrame *annotationsFrame)
{
  QVBoxLayout *annotationsFrameLayout =
    CQUtil::makeLayout<QVBoxLayout>(annotationsFrame, 2, 2);

  //---

  QSplitter *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  splitter->setOrientation(Qt::Vertical);

  annotationsFrameLayout->addWidget(splitter);

  //----

  // view annotations
  CQGroupBox *viewGroup = CQUtil::makeLabelWidget<CQGroupBox>("View", "viewGroup");

  QVBoxLayout *viewGroupLayout = CQUtil::makeLayout<QVBoxLayout>(viewGroup, 2, 2);

  //--

  annotationsWidgets_.viewTable = new CQChartsViewSettingsViewAnnotationsTable;

  connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewAnnotationSelectionChangeSlot()));

  viewGroupLayout->addWidget(annotationsWidgets_.viewTable);

  //--

  splitter->addWidget(viewGroup);

  //----

  // plot annotations
  CQGroupBox *plotGroup = CQUtil::makeLabelWidget<CQGroupBox>("Plot", "plotGroup");

  QVBoxLayout *plotGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotGroup, 2, 2);

  //--

  annotationsWidgets_.plotTable = new CQChartsViewSettingsPlotAnnotationsTable;

  connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotAnnotationSelectionChangeSlot()));

  plotGroupLayout->addWidget(annotationsWidgets_.plotTable);

  //--

  splitter->addWidget(plotGroup);

  //----

  auto createPushButton = [&](const QString &label, const QString &objName, const char *slotName) {
    QPushButton *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  // create annotation buttons
  CQGroupBox *controlGroup = CQUtil::makeLabelWidget<CQGroupBox>("Control", "controlGroup");

  annotationsFrameLayout->addWidget(controlGroup);

  QHBoxLayout *controlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlGroup, 2, 2);

  QPushButton *createButton =
    createPushButton("Create", "create", SLOT(createAnnotationSlot()));
  annotationsWidgets_.editButton =
    createPushButton("Edit"  , "edit"  , SLOT(editAnnotationSlot()));
  annotationsWidgets_.removeButton =
    createPushButton("Remove", "remove", SLOT(removeAnnotationsSlot()));
  QPushButton *writeButton =
    createPushButton("Write" , "write" , SLOT(writeAnnotationSlot()));

  annotationsWidgets_.editButton  ->setEnabled(false);
  annotationsWidgets_.removeButton->setEnabled(false);

  controlGroupLayout->addWidget(createButton);
  controlGroupLayout->addWidget(annotationsWidgets_.editButton);
  controlGroupLayout->addWidget(annotationsWidgets_.removeButton);
  controlGroupLayout->addWidget(writeButton);
  controlGroupLayout->addStretch(1);

  //--

  splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initThemeFrame(QFrame *themeFrame)
{
  QVBoxLayout *themeFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themeFrame, 2, 2);

  //--

  QTabWidget *themeSubTab = CQUtil::makeWidget<QTabWidget>("themeSubTab");

  themeFrameLayout->addWidget(themeSubTab);

  // tab for theme palettes
  QFrame *palettesFrame = CQUtil::makeWidget<QFrame>("palettesFrame");

  QVBoxLayout *palettesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(palettesFrame, 2, 2);

  themeSubTab->addTab(palettesFrame, "Palettes");

  // tab for interface palette
  QFrame *interfaceFrame = CQUtil::makeWidget<QFrame>("interfaceFrame");

  QHBoxLayout *interfaceFrameLayout = CQUtil::makeLayout<QHBoxLayout>(interfaceFrame, 2, 2);

  themeSubTab->addTab(interfaceFrame, "Interface");

  //--

#if 0
  QFrame *themeColorsFrame = CQUtil::makeWidget<QFrame>("themeColorsFrame");

  QGridLayout *themeColorsLayout = CQUtil::makeLayout<QGridLayout>(themeColorsFrame, 2, 2);

  QLabel*    selColorLabel = CQUtil::makeLabelWidget("Selection", "selColorLabel");
  QLineEdit* selColorEdit  = CQUtil::makeWidget<CQLineEdit>("selColorEdit");

  themeColorsLayout->addWidget(selColorLabel, 0, 0);
  themeColorsLayout->addWidget(selColorEdit , 0, 1);

  paletteLayout->addWidget(themeColorsFrame);
#endif

  //--

  QFrame *palettesControlFrame = CQUtil::makeWidget<QFrame>("control");

  QHBoxLayout *palettesControlFrameLayout =
    CQUtil::makeLayout<QHBoxLayout>(palettesControlFrame, 2, 2);

  QLabel *spinLabel = CQUtil::makeLabelWidget<QLabel>("Index", "indexLabel");

  themeWidgets_.palettesSpin = CQUtil::makeWidget<QSpinBox>("indexSpin");

  CQChartsView *view = window_->view();

  int np = view->themeObj()->numPalettes();

  themeWidgets_.palettesSpin->setRange(0, np);

  connect(themeWidgets_.palettesSpin, SIGNAL(valueChanged(int)), this, SLOT(paletteIndexSlot(int)));

  palettesControlFrameLayout->addWidget(spinLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesSpin);

  QLabel *paletteNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "paletteNameLabel");

  themeWidgets_.palettesCombo = CQUtil::makeWidget<QComboBox>("palettesCombo");

  QStringList paletteNames;

  CQChartsThemeMgrInst->getPaletteNames(paletteNames);

  themeWidgets_.palettesCombo->addItems(paletteNames);

  palettesControlFrameLayout->addWidget(paletteNameLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesCombo);

  themeWidgets_.palettesLoadButton = CQUtil::makeLabelWidget<QPushButton>("Load", "load");

  connect(themeWidgets_.palettesLoadButton, SIGNAL(clicked()), this, SLOT(loadPaletteNameSlot()));

  palettesControlFrameLayout->addWidget(themeWidgets_.palettesLoadButton);

  palettesControlFrameLayout->addStretch(1);

  palettesFrameLayout->addWidget(palettesControlFrame);

  //----

  QSplitter *palettesSplitter = CQUtil::makeWidget<QSplitter>("splitter");

  palettesSplitter->setOrientation(Qt::Vertical);

  palettesFrameLayout->addWidget(palettesSplitter);

  themeWidgets_.palettesPlot    =
    new CQChartsGradientPaletteCanvas(this, view->themeObj()->palette());
  themeWidgets_.palettesControl =
    new CQChartsGradientPaletteControl(themeWidgets_.palettesPlot);

  palettesSplitter->addWidget(themeWidgets_.palettesPlot);
  palettesSplitter->addWidget(themeWidgets_.palettesControl);

  connect(themeWidgets_.palettesControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));

  //----

  QSplitter *interfaceSplitter = CQUtil::makeWidget<QSplitter>("splitter");

  interfaceSplitter->setOrientation(Qt::Vertical);

  interfaceFrameLayout->addWidget(interfaceSplitter);

  themeWidgets_.interfacePlot    =
    new CQChartsGradientPaletteCanvas(this, view->interfacePalette());
  themeWidgets_.interfaceControl =
    new CQChartsGradientPaletteControl(themeWidgets_.interfacePlot);

  interfaceSplitter->addWidget(themeWidgets_.interfacePlot);
  interfaceSplitter->addWidget(themeWidgets_.interfaceControl);

  connect(themeWidgets_.interfaceControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));
}

void
CQChartsViewSettings::
initLayersFrame(QFrame *layersFrame)
{
  QVBoxLayout *layersFrameLayout = CQUtil::makeLayout<QVBoxLayout>(layersFrame, 2, 2);

  layersWidgets_.layerTable = new CQChartsViewSettingsLayerTable;

  layersWidgets_.layerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layersFrameLayout->addWidget(layersWidgets_.layerTable);

  connect(layersWidgets_.layerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(layersSelectionChangeSlot()));
  connect(layersWidgets_.layerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(layersClickedSlot(int, int)));

  //---

  QFrame *controlFrame = CQUtil::makeWidget<QFrame>("control");

  QHBoxLayout *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layersFrameLayout->addWidget(controlFrame);

  //--

  QPushButton *imageButton = CQUtil::makeLabelWidget<QPushButton>("Image", "image");

  controlLayout->addWidget(imageButton);
  controlLayout->addStretch(1);

  connect(imageButton, SIGNAL(clicked()), this, SLOT(layerImageSlot()));
}

class CQChartsViewSettingsLayerImage : public QDialog {
 public:
  CQChartsViewSettingsLayerImage() {
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

void
CQChartsViewSettings::
layerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  CQChartsPlot *plot = window_->view()->currentPlot();
  if (! plot) return;

  QImage *image = layersWidgets_.layerTable->selectedImage(plot);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

//------

void
CQChartsViewSettings::
updateModels()
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  modelsWidgets_.modelTable->updateModels(charts);

  //---

  invalidateModelDetails();
}

void
CQChartsViewSettings::
invalidateModelDetails()
{
  CQCharts *charts = window_->view()->charts();

  CQChartsModelData    *modelData = charts->currentModelData();
  CQChartsModelDetails *details   = (modelData ? modelData->details() : nullptr);

  modelsWidgets_.detailsWidget->setDetails(details);
}

void
CQChartsViewSettings::
modelsSelectionChangeSlot()
{
  CQCharts *charts = window_->view()->charts();

  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind >= 0)
    charts->setCurrentModelInd(ind);
}

void
CQChartsViewSettings::
loadModelSlot()
{
  CQCharts *charts = window_->view()->charts();

  if (loadModelDlg_)
    delete loadModelDlg_;

  loadModelDlg_ = new CQChartsLoadModelDlg(charts);

  loadModelDlg_->show();
}

//------

void
CQChartsViewSettings::
writeViewSlot()
{
  CQChartsView *view = window_->view();

  if (view)
    view->write(std::cerr);
}

//------

void
CQChartsViewSettings::
updatePlots()
{
  CQChartsView *view = window_->view();

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
      CQChartsViewSettingsPlotTabWidget *plotWidget =
        qobject_cast<CQChartsViewSettingsPlotTabWidget *>(propertiesWidgets_.plotsTab->widget(i));
      assert(plotWidget);

      if (plotWidget->plot() == plot)
        return i;
    }

    return -1;
  };

  // add new plots to tabbed property view
  for (int i = 0; i < np; ++i) {
    CQChartsPlot *plot = view->plot(i);

    int ind = findPlotTab(plot);

    if (ind < 0) {
      CQChartsViewSettingsPlotTabWidget *plotWidget =
        new CQChartsViewSettingsPlotTabWidget(this, plot);

      connect(plotWidget, SIGNAL(propertyItemSelected(QObject *, const QString &)),
              this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

      propertiesWidgets_.plotsTab->addTab(plotWidget, plot->id());
    }
  }

  // remove deleted plots from tab
  using PlotWidgets = std::vector<CQChartsViewSettingsPlotTabWidget *>;

  PlotWidgets plotWidgets;

  for (int i = 0; i < propertiesWidgets_.plotsTab->count(); ++i) {
    CQChartsViewSettingsPlotTabWidget *plotWidget =
      qobject_cast<CQChartsViewSettingsPlotTabWidget *>(propertiesWidgets_.plotsTab->widget(i));
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
}

void
CQChartsViewSettings::
updateCurrentPlot()
{
  CQChartsView *view = window_->view();
  assert(view);

  if (plotId_.length()) {
    CQChartsPlot *plot = view->getPlot(plotId_);

    if (plot) {
      disconnect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
      disconnect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    }
  }

  //---

  int ind = view->currentPlotInd();

  plotsWidgets_.plotTable->setCurrentInd(ind);

  //---

  CQChartsPlot *plot = view->currentPlot();

  plotId_ = (plot ? plot->id() : "");

  if (plot) {
    connect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
    connect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
  }

  //---

  updateAnnotations();

  updateLayers();
}

CQChartsPlot *
CQChartsViewSettings::
getPropertiesPlot() const
{
  CQChartsViewSettingsPlotTabWidget *plotWidget =
    qobject_cast<CQChartsViewSettingsPlotTabWidget *>(propertiesWidgets_.plotsTab->currentWidget());

  if (! plotWidget)
    return nullptr;

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
  CQChartsView *view = window_->view();

  plotsWidgets_.plotTable->getSelectedPlots(view, plots);
}

void
CQChartsViewSettings::
plotsSelectionChangeSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  plotsWidgets_.raiseButton->setEnabled(plots.size() == 1);
  plotsWidgets_.lowerButton->setEnabled(plots.size() == 1);

  plotsWidgets_.removeButton->setEnabled(plots.size() > 0);

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &plot : plots)
    plot->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
editViewKeySlot()
{
  CQChartsView *view = window_->view();

  if (! view->key())
    return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(view->key());

  editKeyDlg_->show();
}

void
CQChartsViewSettings::
editPlotTitleSlot()
{
  //CQChartsPlot *plot = getSelectedPlot();
  CQChartsPlot *plot = getPropertiesPlot();

  if (! plot || ! plot->title())
    return;

  if (editTitleDlg_)
    delete editTitleDlg_;

  editTitleDlg_ = new CQChartsEditTitleDlg(plot->title());

  editTitleDlg_->show();
}

void
CQChartsViewSettings::
editPlotKeySlot()
{
  //CQChartsPlot *plot = getSelectedPlot();
  CQChartsPlot *plot = getPropertiesPlot();

  if (! plot || ! plot->key())
    return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(plot->key());

  editKeyDlg_->show();
}

void
CQChartsViewSettings::
editPlotXAxisSlot()
{
  //CQChartsPlot *plot = getSelectedPlot();
  CQChartsPlot *plot = getPropertiesPlot();

  if (! plot || ! plot->xAxis())
    return;

  if (editXAxisDlg_)
    delete editXAxisDlg_;

  editXAxisDlg_ = new CQChartsEditAxisDlg(plot->xAxis());

  editXAxisDlg_->show();
}

void
CQChartsViewSettings::
editPlotYAxisSlot()
{
  //CQChartsPlot *plot = getSelectedPlot();
  CQChartsPlot *plot = getPropertiesPlot();

  if (! plot || ! plot->yAxis())
    return;

  if (editYAxisDlg_)
    delete editYAxisDlg_;

  editYAxisDlg_ = new CQChartsEditAxisDlg(plot->yAxis());

  editYAxisDlg_->show();
}

void
CQChartsViewSettings::
groupPlotsSlot()
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool overlay = plotsWidgets_.overlayCheck->isChecked();
  bool x1x2    = plotsWidgets_.x1x2Check   ->isChecked();
  bool y1y2    = plotsWidgets_.y1y2Check   ->isChecked();

  if      (x1x2) {
    if (plots.size() != 2) {
      charts->errorMsg("Need 2 plots for x1x2");
      return;
    }

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2) {
      charts->errorMsg("Need 2 plots for y1y2");
      return;
    }

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
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
  CQChartsView *view = window_->view();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool vertical   = plotsWidgets_.placeVerticalRadio  ->isChecked();
  bool horizontal = plotsWidgets_.placeHorizontalRadio->isChecked();
  int  rows       = plotsWidgets_.placeRowsEdit       ->value();
  int  columns    = plotsWidgets_.placeColumnsEdit    ->value();

  view->placePlots(plots, vertical, horizontal, rows, columns);
}

void
CQChartsViewSettings::
raisePlotSlot()
{
  CQChartsView *view = window_->view();
  CQChartsPlot *plot = getSelectedPlot();

  if (plot)
    view->raisePlot(plot);
}

void
CQChartsViewSettings::
lowerPlotSlot()
{
  CQChartsView *view = window_->view();
  CQChartsPlot *plot = getSelectedPlot();

  if (plot)
    view->lowerPlot(plot);
}

void
CQChartsViewSettings::
removePlotsSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  CQChartsView *view = window_->view();

  for (auto &plot : plots)
    view->removePlot(plot);

  view->updatePlots();
}

void
CQChartsViewSettings::
createPlotSlot()
{
  CQCharts *charts = window_->view()->charts();

  CQChartsModelData *modelData = charts->currentModelData();

  if (! modelData)
    return;

  if (createPlotDlg_)
    delete createPlotDlg_;

  createPlotDlg_ = new CQChartsCreatePlotDlg(charts, modelData);

  createPlotDlg_->setViewName(window_->view()->id());

  createPlotDlg_->show();
}

void
CQChartsViewSettings::
writePlotSlot()
{
  CQChartsPlot *plot = getPropertiesPlot();

  //CQChartsView *view = window_->view();
  //CQChartsPlot *plot = (view ? view->currentPlot() : nullptr();

  if (plot)
    plot->write(std::cerr);
}

//------

void
CQChartsViewSettings::
updateAnnotations()
{
  CQChartsView *view = window_->view();

  annotationsWidgets_.viewTable->updateAnnotations(view);

  //---

  CQChartsPlot *plot = view->currentPlot();

  annotationsWidgets_.plotTable->updateAnnotations(plot);
}

void
CQChartsViewSettings::
viewAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  bool anyAnnotations = (viewAnnotations.size() > 0 || plotAnnotations.size() > 0);

  annotationsWidgets_.editButton  ->setEnabled(anyAnnotations);
  annotationsWidgets_.removeButton->setEnabled(anyAnnotations);

  if (viewAnnotations.size()) {
    disconnect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
               this, SLOT(plotAnnotationSelectionChangeSlot()));

    annotationsWidgets_.plotTable->selectionModel()->clear();

    connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
            this, SLOT(plotAnnotationSelectionChangeSlot()));
  }

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : viewAnnotations)
    annotation->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
plotAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  bool anyAnnotations = (viewAnnotations.size() > 0 || plotAnnotations.size() > 0);

  annotationsWidgets_.editButton  ->setEnabled(anyAnnotations);
  annotationsWidgets_.removeButton->setEnabled(anyAnnotations);

  if (plotAnnotations.size()) {
    disconnect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
               this, SLOT(viewAnnotationSelectionChangeSlot()));

    annotationsWidgets_.viewTable->selectionModel()->clear();

    connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
            this, SLOT(viewAnnotationSelectionChangeSlot()));
  }

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : plotAnnotations)
    annotation->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const
{
  CQChartsView *view = window_->view();

  annotationsWidgets_.viewTable->getSelectedAnnotations(view, viewAnnotations);

  //---

  CQChartsPlot *plot = view->currentPlot();

  if (plot)
    annotationsWidgets_.plotTable->getSelectedAnnotations(plot, plotAnnotations);
}

void
CQChartsViewSettings::
createAnnotationSlot()
{
  CQChartsView *view = window_->view();

  CQChartsPlot *plot = view->currentPlot();

  if (! plot)
    return;

  if (createAnnotationDlg_)
    delete createAnnotationDlg_;

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(plot);

  createAnnotationDlg_->show();
}

void
CQChartsViewSettings::
editAnnotationSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  CQChartsAnnotation *annotation = nullptr;

  if      (! viewAnnotations.empty())
    annotation = viewAnnotations[0];
  else if (! plotAnnotations.empty())
    annotation = plotAnnotations[0];

  if (! annotation)
    return;

  if (editAnnotationDlg_)
    delete editAnnotationDlg_;

  editAnnotationDlg_ = new CQChartsEditAnnotationDlg(annotation);

  editAnnotationDlg_->show();
}

void
CQChartsViewSettings::
removeAnnotationsSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  CQChartsView *view = window_->view();

  for (const auto &annotation : viewAnnotations)
    view->removeAnnotation(annotation);

  CQChartsPlot *plot = view->currentPlot();

  if (plot) {
    for (const auto &annotation : plotAnnotations)
      plot->removeAnnotation(annotation);
  }

  view->updatePlots();
}

void
CQChartsViewSettings::
writeAnnotationSlot()
{
  CQChartsView *view = window_->view();

  const CQChartsView::Annotations &viewAnnotations = view->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(std::cerr);

  //---

  CQChartsView::Plots plots;

  view->getPlots(plots);

  for (const auto &plot : plots) {
    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(std::cerr);
  }
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
paletteIndexSlot(int)
{
  updatePalettes();
}

void
CQChartsViewSettings::
loadPaletteNameSlot()
{
  CQChartsView *view = window_->view();

  QString name = themeWidgets_.palettesCombo->currentText();

  CQChartsGradientPalette *palette = CQChartsThemeMgrInst->getNamedPalette(name);

  if (! palette)
    return;

  int i = themeWidgets_.palettesSpin->value();

  view->themeObj()->setPalette(i, palette->dup());

  updatePalettes();
}

void
CQChartsViewSettings::
updatePalettes()
{
  CQChartsView *view = window_->view();

  themeWidgets_.palettesSpin      ->setEnabled(true);
  themeWidgets_.palettesCombo     ->setEnabled(true);
  themeWidgets_.palettesLoadButton->setEnabled(true);

  int i = themeWidgets_.palettesSpin->value();

  CQChartsGradientPalette *palette = view->themeObj()->palette(i);

  themeWidgets_.palettesPlot->setGradientPalette(palette);

  int ind = themeWidgets_.palettesCombo->findText(palette->name());

  themeWidgets_.palettesCombo->setCurrentIndex(ind);

  themeWidgets_.palettesControl->updateState();

  view->updatePlots();
}

void
CQChartsViewSettings::
updateInterface()
{
  CQChartsView *view = window_->view();

  CQChartsGradientPalette *palette = view->interfacePalette();

  themeWidgets_.interfacePlot->setGradientPalette(palette);

  themeWidgets_.interfaceControl->updateState();
}

//------

void
CQChartsViewSettings::
updateLayers()
{
  layersWidgets_.layerTable->initLayers();

  //---

  CQChartsView *view = window_->view();
  CQChartsPlot *plot = (view ? view->currentPlot() : nullptr);
  if (! plot) return;

  layersWidgets_.layerTable->updateLayers(plot);
}

void
CQChartsViewSettings::
layersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
layersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  CQChartsView *view = window_->view();
  CQChartsPlot *plot = (view ? view->currentPlot() : nullptr);
  if (! plot) return;

  CQChartsLayer::Type type;
  bool                active;

  if (! layersWidgets_.layerTable->getLayerState(plot, row, type, active))
    return;

  CQChartsLayer *layer = plot->getLayer(type);
  if (! layer) return;

  plot->setLayerActive(type, active);

  const CQChartsBuffer *buffer = plot->getBuffer(layer->buffer());

  if (buffer->type() != CQChartsBuffer::Type::MIDDLE)
    plot->invalidateLayer(buffer->type());
  else
    plot->queueDrawObjs();
}

//------

CQChartsViewSettingsPlotTabWidget::
CQChartsViewSettingsPlotTabWidget(CQChartsViewSettings *settings, CQChartsPlot *plot) :
 plot_(plot)
{
  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  propertyTree_ = new CQChartsPropertyViewTree(settings, plot_->propertyModel());

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  //--

  filterEdit_ = new CQChartsViewSettingsFilterEdit(propertyTree_);

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

//------

CQChartsViewSettingsFilterEdit::
CQChartsViewSettingsFilterEdit(CQChartsPropertyViewTree *tree) :
 tree_(tree)
{
  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));

  connect(filterEdit_, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  layout->addWidget(filterEdit_);
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
