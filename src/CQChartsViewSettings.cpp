#include <CQChartsViewSettings.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
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
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>

#include <CQColorsEditCanvas.h>
#include <CQColorsEditControl.h>
#include <CQColorsEditList.h>
#include <CQColors.h>
#include <CQColorsTheme.h>

#include <CQPropertyViewItem.h>
#include <CQTableWidget.h>
#include <CQTabWidget.h>
#include <CQTabSplit.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>
#include <CQGroupBox.h>
#include <CQToolTip.h>

#include <QTextBrowser>
#include <QHeaderView>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QFileDialog>
#include <QDir>

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

    QSize s = QLabel::sizeHint();

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

class CQChartsPlotTip : public CQToolTipIFace {
 public:
  CQChartsPlotTip() :
   CQToolTipIFace() {
  }

 ~CQChartsPlotTip() {
    delete widget_;
  }

  void setPlot(CQChartsPlot *plot) { plot_ = plot; }

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

    QString desc = plot_->type()->description();

    int pos = desc.indexOf("</h2>");

    if (pos > 0) {
      QString lhs = desc.mid(0, pos);
      QString rhs = desc.mid(pos);

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

//------

class CQChartsViewSettingsModelTable : public CQTableWidget {
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

      nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
      indItem ->setFlags(indItem ->flags() & ~Qt::ItemIsEditable);

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

class CQChartsViewSettingsPlotTable : public CQTableWidget {
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

      idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

      setItem(i, 0, idItem);

      int ind = view->plotInd(plot);

      idItem->setData(Qt::UserRole, ind);

      //--

      // set type item
      QTableWidgetItem *typeItem = new QTableWidgetItem(plot->type()->name());

      typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);

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

      stateItem->setFlags(stateItem->flags() & ~Qt::ItemIsEditable);

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

class CQChartsViewSettingsViewAnnotationsTable : public CQTableWidget {
 public:
  CQChartsViewSettingsViewAnnotationsTable() {
    setObjectName("viewTable");

    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

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

      idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

      setItem(i, 0, idItem);

      int ind = annotation->ind();

      idItem->setData(Qt::UserRole, ind);

      QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

      typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);

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

class CQChartsViewSettingsPlotAnnotationsTable : public CQTableWidget {
 public:
  CQChartsViewSettingsPlotAnnotationsTable() {
    setObjectName("plotTable");

    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

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

      idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

      setItem(i, 0, idItem);

      int ind = annotation->ind();

      idItem->setData(Qt::UserRole, ind);

      QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

      typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);

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

class CQChartsViewSettingsViewLayerTable : public CQTableWidget {
 public:
  CQChartsViewSettingsViewLayerTable() {
    setObjectName("viewLayerTable");

    horizontalHeader()->setStretchLastSection(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  QImage *selectedImage(CQChartsView *view) const {
    if (! view) return nullptr;

    QList<QTableWidgetItem *> items = selectedItems();
    if (items.length() <= 0) return nullptr;

    QTableWidgetItem *item = items[0];

    bool ok;

    long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
    if (! ok) return nullptr;

    CQChartsBuffer *buffer = nullptr;

    if      (l == 0)
      buffer = view->objectsBuffer();
    else if (l == 1)
      buffer = view->overlayBuffer();

    if (! buffer) return nullptr;

    QImage *image = buffer->image();

    return image;
  }

  void initLayers() {
    if (rowCount() != 0)
      return;

    clear();

    setColumnCount(3);
    setRowCount(2);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Buffer"));
    setHorizontalHeaderItem(1, new QTableWidgetItem("State" ));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Rect"  ));

    for (int l = 0; l < 2; ++l) {
      QTableWidgetItem *item = nullptr;

      if (l == 0)
        item = new QTableWidgetItem("Objects");
      else
        item = new QTableWidgetItem("Overlay");

      setItem(l, 0, item);

      item->setData(Qt::UserRole, l);

      QTableWidgetItem *stateItem = new QTableWidgetItem("");

      stateItem->setFlags(stateItem->flags() & ~Qt::ItemIsEditable);

      setItem(l, 1, stateItem);

      QTableWidgetItem *rectItem = new QTableWidgetItem("");

      rectItem->setFlags(rectItem->flags() & ~Qt::ItemIsEditable);

      setItem(l, 2, rectItem);
    }
  }

  void updateLayers(CQChartsView *view) {
    for (int l = 0; l < 2; ++l) {
      CQChartsBuffer *buffer = nullptr;

      if      (l == 0)
        buffer = view->objectsBuffer();
      else if (l == 1)
        buffer = view->overlayBuffer();

  //  QTableWidgetItem *idItem    = item(l, 0);
      QTableWidgetItem *stateItem = item(l, 1);
      QTableWidgetItem *rectItem  = item(l, 2);

      stateItem->setText("");

      QRectF rect = (buffer ? buffer->rect() : QRectF());

      QString rectStr = QString("X:%1, Y:%2, W:%3, H:%4").
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

      idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

      setItem(i, 0, idItem);

      idItem->setData(Qt::UserRole, l);

      QTableWidgetItem *stateItem = new QTableWidgetItem("");

    //stateItem->setFlags(stateItem->flags() | Qt::ItemIsEnabled);
      stateItem->setFlags(stateItem->flags() | Qt::ItemIsUserCheckable);
    //stateItem->setFlags(stateItem->flags() & ~Qt::ItemIsEditable);

      setItem(i, 1, stateItem);

      QTableWidgetItem *rectItem = new QTableWidgetItem("");

      rectItem->setFlags(rectItem->flags() & ~Qt::ItemIsEditable);

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
  connect(charts, SIGNAL(modelDataRemoved(int)), this, SLOT(updateModels()));
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
//delete propertiesWidgets_.plotTip;
}

void
CQChartsViewSettings::
addWidgets()
{
  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  tab_ = CQUtil::makeWidget<CQTabWidget>("tab");

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

  tab_->addTab(themeFrame, "Colors");

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

  CQTabSplit *propertiesSplit = CQUtil::makeWidget<CQTabSplit>("propertiesSplit");

  propertiesSplit->setOrientation(Qt::Vertical);
  propertiesSplit->setGrouped(true);

  propertiesLayout->addWidget(propertiesSplit);

  //----

  // Create View Properties Frame
  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("viewFrame");

  QVBoxLayout *viewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(viewFrame, 2, 2);

  propertiesSplit->addWidget(viewFrame, "View");

  //--

  propertiesWidgets_.viewPropertyTree = new CQChartsViewSettingsViewPropertiesWidget(this, view);

  viewFrameLayout->addWidget(propertiesWidgets_.viewPropertyTree);

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

  viewFrameLayout->addWidget(viewEditFrame);

  //----

  // Create Plots Properties Frame
  QFrame *plotsFrame = CQUtil::makeWidget<QFrame>("plotsFrame");

  QVBoxLayout *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  propertiesSplit->addWidget(plotsFrame, "Plots");

  //--

  propertiesWidgets_.plotsTab = CQUtil::makeWidget<CQTabWidget>("tab");

  plotsFrameLayout->addWidget(propertiesWidgets_.plotsTab);

  connect(propertiesWidgets_.plotsTab, SIGNAL(currentChanged(int)),
          this, SLOT(plotsTabChangedSlot()));
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

  int i1 = INT_MAX*0.4;
  int i2 = INT_MAX - i1;

  propertiesSplit->setSizes(QList<int>({i1, i2}));
}

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  QVBoxLayout *modelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsFrame, 2, 2);

  //----

  CQTabSplit *modelsSplit = CQUtil::makeWidget<CQTabSplit>("modelsSplit");

  modelsSplit->setOrientation(Qt::Vertical);

  modelsFrameLayout->addWidget(modelsSplit);

  //----

  // Models Frame
  QFrame *modelsModelsFrame = CQUtil::makeWidget<QFrame>("modelsModelsFrame");

  QVBoxLayout *modelsModelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsModelsFrame, 2, 2);

  modelsSplit->addWidget(modelsModelsFrame, "Models");

  //--

  modelsWidgets_.modelTable = new CQChartsViewSettingsModelTable;

  connect(modelsWidgets_.modelTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(modelsSelectionChangeSlot()));

  modelsModelsFrameLayout->addWidget(modelsWidgets_.modelTable);

  //----

  // Model Details
  modelsWidgets_.detailsFrame = CQUtil::makeWidget<QFrame>("detailsFrame");

  QVBoxLayout *detailsFrameLayout =
    CQUtil::makeLayout<QVBoxLayout>(modelsWidgets_.detailsFrame, 2, 2);

  modelsSplit->addWidget(modelsWidgets_.detailsFrame, "Details");

  //--

  CQChartsView *view   = window_->view();
  CQCharts     *charts = view->charts();

  modelsWidgets_.detailsWidget = new CQChartsModelDetailsWidget(charts);

  detailsFrameLayout->addWidget(modelsWidgets_.detailsWidget);

  //----

  // Model Buttons
  QHBoxLayout *modelControlLayout = CQUtil::makeLayout<QHBoxLayout>(nullptr, 2, 2);

  modelsModelsFrameLayout->addLayout(modelControlLayout);

  //--

  auto createPushButton = [&](const QString &label, const QString &objName, const QString &tip,
                              const char *slotName) {
    QPushButton *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  QPushButton *loadModelButton =
    createPushButton("Load", "load", "Load Model", SLOT(loadModelSlot()));
  modelsWidgets_.editButton =
    createPushButton("Edit", "edit", "Edit Model", SLOT(editModelSlot()));
  modelsWidgets_.plotButton =
    createPushButton("Plot", "plot", "Create Plot", SLOT(plotModelSlot()));

  modelControlLayout->addWidget(loadModelButton);
  modelControlLayout->addWidget(modelsWidgets_.editButton);
  modelControlLayout->addWidget(modelsWidgets_.plotButton);

  modelsWidgets_.editButton->setEnabled(false);
  modelsWidgets_.plotButton->setEnabled(false);

  //--

  modelControlLayout->addStretch(1);

  //--

  modelsSplit->setSizes(QList<int>({INT_MAX, INT_MAX}));
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
  plotsWidgets_.removeButton     = createPushButton("Remove", "remove", SLOT(removePlotsSlot()));
//QPushButton*  writePlotButton  = createPushButton("Write" , "write" , SLOT(writePlotSlot()));

  plotsWidgets_.raiseButton ->setEnabled(false);
  plotsWidgets_.lowerButton ->setEnabled(false);
  plotsWidgets_.removeButton->setEnabled(false);

  controlPlotsGroupLayout->addWidget(plotsWidgets_.raiseButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.lowerButton);
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

  CQTabSplit *annotationsSplit = CQUtil::makeWidget<CQTabSplit>("annotationsSplit");

  annotationsSplit->setOrientation(Qt::Vertical);

  annotationsFrameLayout->addWidget(annotationsSplit);

  //----

  // view annotations
  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("viewFrame");

  QVBoxLayout *viewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(viewFrame, 2, 2);

  //--

  annotationsWidgets_.viewTable = new CQChartsViewSettingsViewAnnotationsTable;

  connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewAnnotationSelectionChangeSlot()));

  viewFrameLayout->addWidget(annotationsWidgets_.viewTable);

  //--

  annotationsSplit->addWidget(viewFrame, "View");

  //----

  // plot annotations
  QFrame *plotFrame = CQUtil::makeWidget<QFrame>("plotFrame");

  QVBoxLayout *plotFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotFrame, 2, 2);

  //--

  annotationsWidgets_.plotTable = new CQChartsViewSettingsPlotAnnotationsTable;

  connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotAnnotationSelectionChangeSlot()));

  plotFrameLayout->addWidget(annotationsWidgets_.plotTable);

  //--

  annotationsSplit->addWidget(plotFrame, "Plot");

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

  annotationsSplit->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initThemeFrame(QFrame *themeFrame)
{
  QVBoxLayout *themeFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themeFrame, 2, 2);

  //--

  CQTabWidget *themeSubTab = CQUtil::makeWidget<CQTabWidget>("themeSubTab");

  themeFrameLayout->addWidget(themeSubTab);

  // tab for theme
  QFrame *themePalettesFrame = CQUtil::makeWidget<QFrame>("themePalettesFrame");

  QVBoxLayout *themePalettesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themePalettesFrame, 2, 2);

  themeSubTab->addTab(themePalettesFrame, "Theme");

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

  QLabel*     selColorLabel = CQUtil::makeLabelWidget("Selection", "selColorLabel");
  CQLineEdit* selColorEdit  = CQUtil::makeWidget<CQLineEdit>("selColorEdit");

  themeColorsLayout->addWidget(selColorLabel, 0, 0);
  themeColorsLayout->addWidget(selColorEdit , 0, 1);

  QLabel*     insColorLabel = CQUtil::makeLabelWidget("Inside", "insColorLabel");
  CQLineEdit* insColorEdit  = CQUtil::makeWidget<CQLineEdit>("insColorEdit");

  themeColorsLayout->addWidget(insColorLabel, 1, 0);
  themeColorsLayout->addWidget(insColorEdit , 1, 1);

  paletteLayout->addWidget(themeColorsFrame);
#endif

  //--

  CQChartsView *view = window_->view();

  CQColorsTheme *theme = view->theme();

  //--

  // create palettes list
  themeWidgets_.palettesList = new CQColorsEditList(this);

  themePalettesFrameLayout->addWidget(themeWidgets_.palettesList);

  //connect(themeWidgets_.palettesList, SIGNAL(palettesChanged()), this, SLOT(updateView()));

  //--

  // create palettes frame
  QFrame *palettesControlFrame = CQUtil::makeWidget<QFrame>("control");

  QHBoxLayout *palettesControlFrameLayout =
    CQUtil::makeLayout<QHBoxLayout>(palettesControlFrame, 2, 2);

  //--

  QLabel *paletteNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "paletteNameLabel");

  themeWidgets_.palettesCombo = CQUtil::makeWidget<QComboBox>("palettesCombo");

  QStringList paletteNames;

  CQColorsMgrInst->getPaletteNames(paletteNames);

  themeWidgets_.palettesCombo->addItems(paletteNames);

  palettesControlFrameLayout->addWidget(paletteNameLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesCombo);

  connect(themeWidgets_.palettesCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(palettesComboSlot(int)));

  //--

  QPushButton *resetButton = CQUtil::makeLabelWidget<QPushButton>("Reset", "resetButton");

  palettesControlFrameLayout->addWidget(resetButton);

  connect(resetButton, SIGNAL(clicked()), this, SLOT(palettesResetSlot()));

  //--

  palettesControlFrameLayout->addStretch(1);

  palettesFrameLayout->addWidget(palettesControlFrame);

  //----

  // create palettes splitter and add canvas and control
  QSplitter *palettesSplitter = CQUtil::makeWidget<QSplitter>("splitter");

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
  QSplitter *interfaceSplitter = CQUtil::makeWidget<QSplitter>("splitter");

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
  QVBoxLayout *layersFrameLayout = CQUtil::makeLayout<QVBoxLayout>(layersFrame, 2, 2);

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

  QFrame *controlFrame = CQUtil::makeWidget<QFrame>("control");

  QHBoxLayout *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layersFrameLayout->addWidget(controlFrame);

  //--

  QPushButton *viewImageButton = CQUtil::makeLabelWidget<QPushButton>("View Image", "viewImage");

  controlLayout->addWidget(viewImageButton);

  connect(viewImageButton, SIGNAL(clicked()), this, SLOT(viewLayerImageSlot()));

  QPushButton *plotImageButton = CQUtil::makeLabelWidget<QPushButton>("Plot Image", "plotImage");

  controlLayout->addWidget(plotImageButton);

  connect(plotImageButton, SIGNAL(clicked()), this, SLOT(plotLayerImageSlot()));

  controlLayout->addStretch(1);
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

  CQChartsView *view = window_->view();
  if (! view) return;

  QImage *image = layersWidgets_.viewLayerTable->selectedImage(view);
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

  CQChartsPlot *plot = window_->view()->currentPlot();
  if (! plot) return;

  QImage *image = layersWidgets_.plotLayerTable->selectedImage(plot);
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
    CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
      qobject_cast<CQChartsViewSettingsPlotPropertiesWidget *>(
        propertiesWidgets_.plotsTab->widget(i));
    assert(plotWidget);

    if (plotWidget->plot() == plot)
      return plotWidget->propertyTree();
  }

  return nullptr;
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

  modelsWidgets_.editButton->setEnabled(ind >= 0);
  modelsWidgets_.plotButton->setEnabled(ind >= 0);
}

void
CQChartsViewSettings::
loadModelSlot()
{
  CQCharts *charts = window_->view()->charts();

  delete loadModelDlg_;

  loadModelDlg_ = new CQChartsLoadModelDlg(charts);

  loadModelDlg_->show();
}

void
CQChartsViewSettings::
editModelSlot()
{
  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind < 0)
    return;

  CQCharts *charts = window_->view()->charts();

  CQChartsModelData *modelData = charts->getModelData(ind);

  if (! modelData)
    return;

  charts->editModelDlg(modelData);
}

void
CQChartsViewSettings::
plotModelSlot()
{
  long ind = modelsWidgets_.modelTable->selectedModel();

  if (ind < 0)
    return;

  CQCharts *charts = window_->view()->charts();

  CQChartsModelData *modelData = charts->getModelData(ind);

  if (! modelData)
    return;

  CQChartsCreatePlotDlg *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(window_->view()->id());
}

//------

void
CQChartsViewSettings::
writeViewSlot()
{
  CQChartsView *view = window_->view();
  if (! view) return;

  QString dir = QDir::current().dirName() + "/view.tcl";

  QString fileName = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");

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

  CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
    qobject_cast<CQChartsViewSettingsPlotPropertiesWidget *>(
      propertiesWidgets_.plotsTab->widget(ind));
  assert(plotWidget);

//propertiesWidgets_.plotTip->setPlot(plotWidget->plot());
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
      CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
        qobject_cast<CQChartsViewSettingsPlotPropertiesWidget *>(
          propertiesWidgets_.plotsTab->widget(i));
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
      CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
        new CQChartsViewSettingsPlotPropertiesWidget(this, plot);

      plotWidget->setObjectName(QString("plotTabWidget_%1").
        arg(propertiesWidgets_.plotsTab->count() + 1));

      connect(plotWidget, SIGNAL(propertyItemSelected(QObject *, const QString &)),
              this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

      ind = propertiesWidgets_.plotsTab->addTab(plotWidget, plot->id());

      //propertiesWidgets_.plotsTab->setTabToolTip(ind, plot->type()->description());
    }
  }

  // remove deleted plots from tab
  using PlotWidgets = std::vector<CQChartsViewSettingsPlotPropertiesWidget *>;

  PlotWidgets plotWidgets;

  for (int i = 0; i < propertiesWidgets_.plotsTab->count(); ++i) {
    CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
      qobject_cast<CQChartsViewSettingsPlotPropertiesWidget *>(
        propertiesWidgets_.plotsTab->widget(i));
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
  CQChartsViewSettingsPlotPropertiesWidget *plotWidget =
    qobject_cast<CQChartsViewSettingsPlotPropertiesWidget *>(
      propertiesWidgets_.plotsTab->currentWidget());

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

  editKeyDlg_ = new CQChartsEditKeyDlg(this, view->key());

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

  editTitleDlg_ = new CQChartsEditTitleDlg(this, plot->title());

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

  editKeyDlg_ = new CQChartsEditKeyDlg(this, plot->key());

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

  editXAxisDlg_ = new CQChartsEditAxisDlg(this, plot->xAxis());

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

  editYAxisDlg_ = new CQChartsEditAxisDlg(this, plot->yAxis());

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

  view->placePlots(plots, vertical, horizontal, rows, columns, /*reset*/true);
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

  updateView();
}

void
CQChartsViewSettings::
writePlotSlot()
{
  CQChartsPlot *plot = getPropertiesPlot();
  if (! plot) return;

  QString dir = QDir::current().dirName() + "/plot.tcl";

  QString fileName = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  //---

  CQChartsView *view = plot->view();

  view->write(fs);

  CQChartsModelData *modelData = plot->getModelData();

  if (modelData)
    modelData->write(fs);

  plot->write(fs);
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

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(this, plot);

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

  editAnnotationDlg_ = new CQChartsEditAnnotationDlg(this, annotation);

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

  updateView();
}

void
CQChartsViewSettings::
writeAnnotationSlot()
{
  CQChartsView *view = window_->view();
  if (! view) return;

  QString dir = QDir::current().dirName() + "/annotation.tcl";

  QString fileName = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  //---

  const CQChartsView::Annotations &viewAnnotations = view->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(fs);

  //---

  CQChartsView::Plots plots;

  view->getPlots(plots);

  for (const auto &plot : plots) {
    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(fs);
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
palettesComboSlot(int)
{
  QString name = themeWidgets_.palettesCombo->currentText();

  CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(name);

  themeWidgets_.palettesPlot->setPalette(palette);

  themeWidgets_.palettesControl->updateState();
}

void
CQChartsViewSettings::
palettesResetSlot()
{
  QString name = themeWidgets_.palettesCombo->currentText();

  CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(name);
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

  QString name = themeWidgets_.palettesCombo->currentText();

  CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(name);

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
  CQChartsView *view = window_->view();

  view->updatePlots();
}

void
CQChartsViewSettings::
updateInterface()
{
  CQChartsView *view = window_->view();

  CQColorsPalette *palette = view->interfacePalette();

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

  CQChartsView *view = window_->view();

  if (view)
    layersWidgets_.viewLayerTable->updateLayers(view);

  CQChartsPlot *plot = (view ? view->currentPlot() : nullptr);

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

  CQChartsView *view = window_->view();

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

  CQChartsView *view = window_->view();
  CQChartsPlot *plot = (view ? view->currentPlot() : nullptr);
  if (! plot) return;

  CQChartsLayer::Type type;
  bool                active;

  if (! layersWidgets_.plotLayerTable->getLayerState(plot, row, type, active))
    return;

  CQChartsLayer *layer = plot->getLayer(type);
  if (! layer) return;

  plot->setLayerActive(type, active);

  const CQChartsBuffer *buffer = plot->getBuffer(layer->buffer());

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
  setObjectName("viewWidget");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

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
  setObjectName("plotTabWidget");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

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

CQChartsViewSettingsFilterEdit::
CQChartsViewSettingsFilterEdit(CQChartsPropertyViewTree *tree) :
 tree_(tree)
{
  setObjectName("filterEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

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
