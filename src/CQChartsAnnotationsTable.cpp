#include <CQChartsAnnotationsTable.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAnnotation.h>
#include <CQChartsVariant.h>
#include <CQChartsCreateAnnotationDlg.h>
#include <CQChartsEditAnnotationDlg.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQTabSplit.h>

#include <QHeaderView>
#include <QPushButton>
#include <QPainter>
#include <QFileDialog>
#include <QDir>

#include <fstream>

CQChartsAnnotationDelegate::
CQChartsAnnotationDelegate(CQChartsAnnotationsTable *table) :
 table_(table)
{
}

void
CQChartsAnnotationDelegate::
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

//---

CQChartsAnnotationsControl::
CQChartsAnnotationsControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("annotationsControl");

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //--

  split_ = CQUtil::makeWidget<CQTabSplit>("annotationsSplit");

  split_->setOrientation(Qt::Vertical);
  split_->setGrouped(true);

  layout->addWidget(split_);

  //----

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const char *slotName, const QString &tip) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    button->setToolTip(tip);

    return button;
  };

  //---

  // view annotations control
  viewTable_ = new CQChartsViewAnnotationsControl;

  split_->addWidget(viewTable_, "View");

  //---

  // plot annotations control
  plotTable_ = new CQChartsPlotAnnotationsControl;

  split_->addWidget(plotTable_, "Plot");

  //--

  split_->setSizes(QList<int>({INT_MAX, INT_MAX}));

  //--

  // create view/plot annotation buttons
  auto *controlGroup  = CQUtil::makeLabelWidget<CQGroupBox>("View/Plot Control", "controlGroup");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlGroup, 2, 2);

  layout->addWidget(controlGroup);

  auto *writeButton =
    createPushButton("Write", "write", SLOT(writeAnnotationSlot()),
                     "Write View and Plot Annotations");

  controlLayout->addWidget(writeButton);
  controlLayout->addStretch(1);
}

void
CQChartsAnnotationsControl::
setView(CQChartsView *view)
{
  if (view_)
    disconnect(view_, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  view_ = view;

  viewTable_->setView(view_);

  if (view_)
    connect(view_, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
}

void
CQChartsAnnotationsControl::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  plot_ = plot;

  plotTable_->setPlot(plot_);

  if (plot_)
    connect(plot_, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
}

void
CQChartsAnnotationsControl::
updateAnnotations()
{
  viewTable_->setView(view_);

  //---

  auto plotName = (plot_ ? QString("Plot %1").arg(plot_->id()) : "Plot");

  split_->setWidgetName(plotTable_, plotName);

  plotTable_->setPlot(plot_);
}

void
CQChartsAnnotationsControl::
writeAnnotationSlot()
{
  if (! view_) return;

  auto dir = QDir::current().dirName() + "/annotation.tcl";

  auto filename = QFileDialog::getSaveFileName(this, "Write Annotations", dir, "Files (*.tcl)");
  if (! filename.length()) return; // cancelled

  auto fs = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  const auto &viewAnnotations = view_->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(fs);

  //---

  CQChartsView::Plots plots;

  view_->getPlots(plots);

  for (const auto &plot : plots) {
    const auto &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(fs);
  }
}

//---

CQChartsViewAnnotationsControl::
CQChartsViewAnnotationsControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("viewAnnotationsControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  viewTable_ = new CQChartsViewAnnotationsTable;

  connect(viewTable_, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewAnnotationSelectionChangeSlot()));

  layout->addWidget(viewTable_);

  //---

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const char *slotName, const QString &tip) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    button->setToolTip(tip);

    return button;
  };

  //---

  // create view annotation buttons
  auto *viewControlGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("View Control", "viewControlGroup");

  layout->addWidget(viewControlGroup);

  auto *viewControlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(viewControlGroup, 2, 2);

  viewRaiseButton_ =
    createPushButton("Raise"    , "raise" , SLOT(raiseViewAnnotationSlot()),
                     "Raise View Annotation");
  viewLowerButton_ =
    createPushButton("Lower"    , "lower" , SLOT(lowerViewAnnotationSlot()),
                     "Lower View Annotation");
  viewCreateButton_ =
    createPushButton("Create...", "create", SLOT(createViewAnnotationSlot()),
                     "Create View Annotation");
  viewEditButton_ =
    createPushButton("Edit...", "edit", SLOT(editViewAnnotationSlot()),
                     "Edit Selected View Annotation");
  viewRemoveButton_ =
    createPushButton("Remove", "remove", SLOT(removeViewAnnotationsSlot()),
                     "Remove Selected View Annotation");

  viewEditButton_  ->setEnabled(false);
  viewRemoveButton_->setEnabled(false);

  viewControlGroupLayout->addWidget(viewRaiseButton_);
  viewControlGroupLayout->addWidget(viewLowerButton_);
  viewControlGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  viewControlGroupLayout->addWidget(viewCreateButton_);
  viewControlGroupLayout->addWidget(viewEditButton_);
  viewControlGroupLayout->addWidget(viewRemoveButton_);
  viewControlGroupLayout->addStretch(1);
}

void
CQChartsViewAnnotationsControl::
setView(CQChartsView *view)
{
  if (view != view_) {
    if (view_)
      disconnect(view_, SIGNAL(annotationsChanged()), viewTable_, SLOT(updateAnnotations()));

    view_ = view;

    viewTable_->setView(view_);

    if (view_)
      connect(view_, SIGNAL(annotationsChanged()), viewTable_, SLOT(updateAnnotations()));
  }
}

void
CQChartsViewAnnotationsControl::
viewAnnotationSelectionChangeSlot()
{
  CQChartsAnnotationsTable::Annotations viewAnnotations;

  viewTable_->getSelectedAnnotations(viewAnnotations);

  bool anyViewAnnotations = (viewAnnotations.size() > 0);

  viewRaiseButton_ ->setEnabled(anyViewAnnotations);
  viewLowerButton_ ->setEnabled(anyViewAnnotations);
  viewEditButton_  ->setEnabled(anyViewAnnotations);
  viewRemoveButton_->setEnabled(anyViewAnnotations);

#if 0
  // deselect all plot annotations (needed ?)
  if (anyViewAnnotations) {
    CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
      plotTable_, SIGNAL(itemSelectionChanged()),
      this, SLOT(plotAnnotationSelectionChangeSlot()));

    plotTable_->selectionModel()->clear();
  }
#endif

  //---

  {
  CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
    viewTable_, SIGNAL(itemSelectionChanged()),
    this, SLOT(viewAnnotationSelectionChangeSlot()));
  CQChartsWidgetUtil::AutoDisconnect annotationsDisconnect(
    view_, SIGNAL(annotationsChanged()), this, SLOT(updateViewAnnotations()));

  disconnect(view_, SIGNAL(annotationsChanged()), viewTable_, SLOT(updateAnnotations()));

  view_->startSelection();

  view_->deselectAll();

  for (auto &annotation : viewAnnotations) {
    if (annotation->isSelectable())
      annotation->setSelected(true);
  }

  view_->endSelection();

  connect(view_, SIGNAL(annotationsChanged()), viewTable_, SLOT(updateAnnotations()));
  }
}

void
CQChartsViewAnnotationsControl::
raiseViewAnnotationSlot()
{
  auto *annotation = getSelectedViewAnnotation();
  if (! annotation) return;

  annotation->view()->raiseAnnotation(annotation);

  updateViewAnnotations();
}

void
CQChartsViewAnnotationsControl::
lowerViewAnnotationSlot()
{
  auto *annotation = getSelectedViewAnnotation();
  if (! annotation) return;

  annotation->view()->lowerAnnotation(annotation);

  updateViewAnnotations();
}

void
CQChartsViewAnnotationsControl::
updateViewAnnotations()
{
  viewTable_->updateAnnotations();
}

void
CQChartsViewAnnotationsControl::
createViewAnnotationSlot()
{
  if (createAnnotationDlg_)
    delete createAnnotationDlg_;

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(this, view_);

  createAnnotationDlg_->show();
  createAnnotationDlg_->raise();
}

void
CQChartsViewAnnotationsControl::
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
CQChartsViewAnnotationsControl::
removeViewAnnotationsSlot()
{
  CQChartsAnnotationsTable::Annotations viewAnnotations;

  viewTable_->getSelectedAnnotations(viewAnnotations);

  if (! view_) return;

  for (const auto &annotation : viewAnnotations)
    view_->removeAnnotation(annotation);

  view_->updatePlots();
}

CQChartsAnnotation *
CQChartsViewAnnotationsControl::
getSelectedViewAnnotation() const
{
  CQChartsAnnotationsTable::Annotations viewAnnotations;

  viewTable_->getSelectedAnnotations(viewAnnotations);

  return (! viewAnnotations.empty() ? viewAnnotations[0] : nullptr);
}

//---

CQChartsPlotAnnotationsControl::
CQChartsPlotAnnotationsControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("plotAnnotationsControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  plotTable_ = new CQChartsPlotAnnotationsTable;

  connect(plotTable_, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotAnnotationSelectionChangeSlot()));

  layout->addWidget(plotTable_);

  //--

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const char *slotName, const QString &tip) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    button->setToolTip(tip);

    return button;
  };

  //---

  // create plot annotation buttons
  auto *plotControlGroup =
    CQUtil::makeLabelWidget<CQGroupBox>("Plot Control", "plotControlGroup");

  layout->addWidget(plotControlGroup);

  auto *plotControlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(plotControlGroup, 2, 2);

  plotRaiseButton_ =
    createPushButton("Raise"    , "raise" , SLOT(raisePlotAnnotationSlot()),
                     "Raise View Annotation");
  plotLowerButton_ =
    createPushButton("Lower"    , "lower" , SLOT(lowerPlotAnnotationSlot()),
                     "Lower View Annotation");
  plotCreateButton_ =
    createPushButton("Create...", "create", SLOT(createPlotAnnotationSlot()),
                     "Create Plot Annotation");
  plotEditButton_ =
    createPushButton("Edit...", "edit", SLOT(editPlotAnnotationSlot()),
                     "Edit Selected Plot Annotation");
  plotRemoveButton_ =
    createPushButton("Remove", "remove", SLOT(removePlotAnnotationsSlot()),
                     "Remove Selected Plot Annotation");

  plotEditButton_  ->setEnabled(false);
  plotRemoveButton_->setEnabled(false);

  plotControlGroupLayout->addWidget(plotRaiseButton_);
  plotControlGroupLayout->addWidget(plotLowerButton_);
  plotControlGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  plotControlGroupLayout->addWidget(plotCreateButton_);
  plotControlGroupLayout->addWidget(plotEditButton_);
  plotControlGroupLayout->addWidget(plotRemoveButton_);
  plotControlGroupLayout->addStretch(1);
}

void
CQChartsPlotAnnotationsControl::
setPlot(CQChartsPlot *plot)
{
  if (plot != plot_) {
    if (plot_)
      disconnect(plot_, SIGNAL(annotationsChanged()), plotTable_, SLOT(updateAnnotations()));

    plot_ = plot;

    plotTable_->setPlot(plot_);

    if (plot_)
      connect(plot_, SIGNAL(annotationsChanged()), plotTable_, SLOT(updateAnnotations()));
  }
}

void
CQChartsPlotAnnotationsControl::
plotAnnotationSelectionChangeSlot()
{
  CQChartsAnnotationsTable::Annotations plotAnnotations;

  plotTable_->getSelectedAnnotations(plotAnnotations);

  bool anyPlotAnnotations = (plotAnnotations.size() > 0);

  plotRaiseButton_ ->setEnabled(anyPlotAnnotations);
  plotLowerButton_ ->setEnabled(anyPlotAnnotations);
  plotEditButton_  ->setEnabled(anyPlotAnnotations);
  plotRemoveButton_->setEnabled(anyPlotAnnotations);

#if 0
  // deselect all view annotations (needed ?)
  if (anyPlotAnnotations) {
    CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
      viewTable_, SIGNAL(itemSelectionChanged()),
      this, SLOT(viewAnnotationSelectionChangeSlot()));

    viewTable_->selectionModel()->clear();
  }
#endif

  //---

  auto *view = plot_->view();

  {
  CQChartsWidgetUtil::AutoDisconnect tableDisconnect(
    plotTable_, SIGNAL(itemSelectionChanged()),
    this, SLOT(plotAnnotationSelectionChangeSlot()));
  CQChartsWidgetUtil::AutoDisconnect annotationsDisconnect(
    plot_, SIGNAL(annotationsChanged()), this, SLOT(updatePlotAnnotations()));

  disconnect(plot_, SIGNAL(annotationsChanged()), plotTable_, SLOT(updateAnnotations()));

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : plotAnnotations) {
    if (annotation->isSelectable())
      annotation->setSelected(true);
  }

  view->endSelection();

  connect(plot_, SIGNAL(annotationsChanged()), plotTable_, SLOT(updateAnnotations()));
  }
}

void
CQChartsPlotAnnotationsControl::
raisePlotAnnotationSlot()
{
  auto *annotation = getSelectedPlotAnnotation();
  if (! annotation) return;

  annotation->plot()->raiseAnnotation(annotation);

  updatePlotAnnotations();
}

void
CQChartsPlotAnnotationsControl::
lowerPlotAnnotationSlot()
{
  auto *annotation = getSelectedPlotAnnotation();
  if (! annotation) return;

  annotation->plot()->lowerAnnotation(annotation);

  updatePlotAnnotations();
}

void
CQChartsPlotAnnotationsControl::
updatePlotAnnotations()
{
  plotTable_->updateAnnotations();
}

void
CQChartsPlotAnnotationsControl::
createPlotAnnotationSlot()
{
  if (! plot_) return;

  if (createAnnotationDlg_)
    delete createAnnotationDlg_;

  createAnnotationDlg_ = new CQChartsCreateAnnotationDlg(this, plot_);

  createAnnotationDlg_->show();
  createAnnotationDlg_->raise();
}

void
CQChartsPlotAnnotationsControl::
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
CQChartsPlotAnnotationsControl::
removePlotAnnotationsSlot()
{
  CQChartsAnnotationsTable::Annotations plotAnnotations;

  plotTable_->getSelectedAnnotations(plotAnnotations);

  if (! plot_) return;

  for (const auto &annotation : plotAnnotations)
    plot_->removeAnnotation(annotation);

  auto *view = plot_->view();

  view->updatePlots();
}

CQChartsAnnotation *
CQChartsPlotAnnotationsControl::
getSelectedPlotAnnotation() const
{
  CQChartsAnnotationsTable::Annotations plotAnnotations;

  plotTable_->getSelectedAnnotations(plotAnnotations);

  return (! plotAnnotations.empty() ? plotAnnotations[0] : nullptr);
}

//---

CQChartsAnnotationsTable::
CQChartsAnnotationsTable(CQChartsView *view, CQChartsPlot *plot) :
 view_(view), plot_(plot)
{
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->setVisible(false);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);

  //---

  delegate_ = new CQChartsAnnotationDelegate(this);

  setItemDelegate(delegate_);
}

CQChartsAnnotationsTable::
~CQChartsAnnotationsTable()
{
  delete delegate_;
}

void
CQChartsAnnotationsTable::
addHeaderItems()
{
  setColumnCount(3);

  setHorizontalHeaderItem(0, new QTableWidgetItem("Id"   ));
  setHorizontalHeaderItem(1, new QTableWidgetItem("Type" ));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Style"));
}

QTableWidgetItem *
CQChartsAnnotationsTable::
createItem(const QString &name, int r, int c, CQChartsAnnotation *annotation)
{
  auto *item = new QTableWidgetItem(name);

  item->setToolTip(name);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  item->setData(Qt::UserRole, annotation->ind());

  setItem(r, c, item);

  return item;
}

void
CQChartsAnnotationsTable::
getSelectedAnnotations(Annotations &annotations)
{
  auto items = selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    auto *item = items[i];
    if (item->column() != 0) continue;

    auto *annotation = itemAnnotation(item);

    if (annotation)
      annotations.push_back(annotation);
  }
}

CQChartsAnnotation *
CQChartsAnnotationsTable::
itemAnnotation(QTableWidgetItem *item) const
{
  bool ok;

  long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

  CQChartsAnnotation *annotation = nullptr;

  if      (view_)
    annotation = view_->getAnnotationByInd(int(ind));
  else if (plot_)
    annotation = plot_->getAnnotationByInd(int(ind));

  return annotation;
}

//--

CQChartsViewAnnotationsTable::
CQChartsViewAnnotationsTable()
{
  setObjectName("viewAnnotationsTable");
}

void
CQChartsViewAnnotationsTable::
setView(CQChartsView *view)
{
  if (view != view_) {
    view_ = view;

    CQChartsAnnotationsTable::setView(view_);

    updateAnnotations();
  }
}

void
CQChartsViewAnnotationsTable::
updateAnnotations()
{
  clear();

  if (! view_)
    return;

  const auto &viewAnnotations = view_->annotations();

  int nv = int(viewAnnotations.size());

  setRowCount(nv);

  addHeaderItems();

  for (int i = 0; i < nv; ++i) {
    auto *annotation = viewAnnotations[size_t(i)];

    (void) createItem(annotation->id()      , i, 0, annotation);
    (void) createItem(annotation->typeName(), i, 1, annotation);
    (void) createItem(""                    , i, 2, annotation);
  }
}

//--

CQChartsPlotAnnotationsTable::
CQChartsPlotAnnotationsTable()
{
  setObjectName("plotAnnotationsTable");
}

void
CQChartsPlotAnnotationsTable::
setPlot(CQChartsPlot *plot)
{
  if (plot != plot_) {
    plot_ = plot;

    CQChartsAnnotationsTable::setPlot(plot_);

    updateAnnotations();
  }
}

void
CQChartsPlotAnnotationsTable::
updateAnnotations()
{
  clear();

  if (! plot_)
    return;

  const auto &plotAnnotations = plot_->annotations();

  int np = int(plotAnnotations.size());

  setRowCount(np);

  addHeaderItems();

  for (int i = 0; i < np; ++i) {
    auto *annotation = plotAnnotations[size_t(i)];

    (void) createItem(annotation->id()      , i, 0, annotation);
    (void) createItem(annotation->typeName(), i, 1, annotation);
    (void) createItem(""                    , i, 2, annotation);
  }
}
