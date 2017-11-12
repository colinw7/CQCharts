#include <CQGradientControlIFace.h>
#include <CQGradientControlPlot.h>
#include <CQRealSpin.h>
#include <CQColorChooser.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QHeaderView>
#include <QCheckBox>
#include <QItemDelegate>
#include <cassert>

CQGradientControlIFace::
CQGradientControlIFace(CQGradientControlPlot *palette) :
 QFrame(0), palette_(palette)
{
  setObjectName("paletteControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  QFontMetrics fm(font());

  //---

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  CGradientPalette *pal = palette_->gradientPalette();

  //---

  QFrame *colorTypeFrame = createColorTypeCombo("Type", &colorType_);

  colorType_->setType(pal->colorType());

  connect(colorType_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorTypeChanged(int)));

  layout->addWidget(colorTypeFrame);

  //---

  QFrame *colorModelFrame = createColorModelCombo("Color", &colorModel_);

  colorModel_->setModel(pal->colorModel());

  connect(colorModel_, SIGNAL(currentIndexChanged(int)), this, SLOT(colorModelChanged(int)));

  layout->addWidget(colorModelFrame);

  //---

  stack_ = new QStackedWidget;

  stack_->setObjectName("stack");

  //---

  QFrame *modelFrame = new QFrame;

  modelFrame->setObjectName("modelFrame");

  QVBoxLayout *modelLayout = new QVBoxLayout(modelFrame);
  modelLayout->setMargin(2); modelLayout->setSpacing(2);

  //---

  QGroupBox *functionGroupBox = new QGroupBox("Function");

  functionGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  functionGroupBox->setObjectName("function");

  QGridLayout *functionGroupLayout = new QGridLayout(functionGroupBox);
  functionGroupLayout->setMargin(0); functionGroupLayout->setSpacing(2);

  modelLayout->addWidget(functionGroupBox);

  createModelCombo(functionGroupLayout, 0, "R", &redModelLabel_  , &redModelCombo_  );
  createModelCombo(functionGroupLayout, 1, "G", &greenModelLabel_, &greenModelCombo_);
  createModelCombo(functionGroupLayout, 2, "B", &blueModelLabel_ , &blueModelCombo_ );

  setRedModel  (pal->redModel  ());
  setGreenModel(pal->greenModel());
  setBlueModel (pal->blueModel ());

  connect(redModelCombo_  , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(greenModelCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
  connect(blueModelCombo_ , SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));

  //---

  QGroupBox *negateGroupBox = new QGroupBox("Negate");

  negateGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  negateGroupBox->setObjectName("negate");

  QHBoxLayout *negateGroupLayout = new QHBoxLayout(negateGroupBox);
  negateGroupLayout->setMargin(0); negateGroupLayout->setSpacing(2);

  modelLayout->addWidget(negateGroupBox);

  modelRNegativeCheck_ = new QCheckBox("R");
  modelGNegativeCheck_ = new QCheckBox("G");
  modelBNegativeCheck_ = new QCheckBox("B");

  modelRNegativeCheck_->setObjectName("rnegative");
  modelGNegativeCheck_->setObjectName("gnegative");
  modelBNegativeCheck_->setObjectName("bnegative");

  connect(modelRNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelRNegativeChecked(int)));
  connect(modelGNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelGNegativeChecked(int)));
  connect(modelBNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelBNegativeChecked(int)));

  negateGroupLayout->addWidget(modelRNegativeCheck_);
  negateGroupLayout->addWidget(modelGNegativeCheck_);
  negateGroupLayout->addWidget(modelBNegativeCheck_);
  negateGroupLayout->addStretch(1);

  //---

  QGroupBox *rangeGroupBox = new QGroupBox("Range");

  rangeGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  rangeGroupBox->setObjectName("range");

  QHBoxLayout *rangeGroupLayout = new QHBoxLayout(rangeGroupBox);
  rangeGroupLayout->setMargin(0); rangeGroupLayout->setSpacing(2);

  modelLayout->addWidget(rangeGroupBox);

  QGridLayout *rangeGridLayout = new QGridLayout;

  redMinMaxLabel_   = new QLabel("R");
  greenMinMaxLabel_ = new QLabel("G");
  blueMinMaxLabel_  = new QLabel("B");

  rangeGridLayout->addWidget(redMinMaxLabel_  , 0, 0);
  rangeGridLayout->addWidget(greenMinMaxLabel_, 1, 0);
  rangeGridLayout->addWidget(blueMinMaxLabel_ , 2, 0);

  createRealEdit(rangeGridLayout, 0, 1, false, "Min", &redMin_  );
  createRealEdit(rangeGridLayout, 0, 3, true , "Max", &redMax_  );
  createRealEdit(rangeGridLayout, 1, 1, false, "Min", &greenMin_);
  createRealEdit(rangeGridLayout, 1, 3, true , "Max", &greenMax_);
  createRealEdit(rangeGridLayout, 2, 1, false, "Min", &blueMin_ );
  createRealEdit(rangeGridLayout, 2, 3, true , "Max", &blueMax_ );

  redMin_  ->setValue(pal->redMin  ());
  redMax_  ->setValue(pal->redMax  ());
  greenMin_->setValue(pal->greenMin());
  greenMax_->setValue(pal->greenMax());
  blueMin_ ->setValue(pal->blueMin ());
  blueMax_ ->setValue(pal->blueMax ());

  rangeGroupLayout->addLayout(rangeGridLayout);
  rangeGroupLayout->addStretch(1);

  connect(redMin_  , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(redMax_  , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(greenMin_, SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(greenMax_, SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(blueMin_ , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));
  connect(blueMax_ , SIGNAL(valueChanged(double)), this, SLOT(modelRangeValueChanged(double)));

  //---

  modelLayout->addStretch(1);

  stack_->addWidget(modelFrame);

  //---

  QFrame *definedFrame = new QFrame;

  definedFrame->setObjectName("definedFrame");

  QVBoxLayout *definedFrameLayout = new QVBoxLayout(definedFrame);
  definedFrameLayout->setMargin(2); definedFrameLayout->setSpacing(2);

  definedColors_ = new CQGradientControlDefinedColors;

  definedColors_->updateColors(pal);

  definedFrameLayout->addWidget(definedColors_);

  connect(definedColors_, SIGNAL(colorsChanged()), this, SLOT(colorsChanged()));

  QFrame *definedButtonsFrame = new QFrame;

  definedButtonsFrame->setObjectName("definedButtonsFrame");

  QHBoxLayout *definedButtonsLayout = new QHBoxLayout(definedButtonsFrame);
  definedButtonsLayout->setMargin(2); definedButtonsLayout->setSpacing(2);

  addColorButton_   = new QPushButton("Add");
  loadColorsButton_ = new QPushButton("Load");

  addColorButton_  ->setObjectName("add");
  loadColorsButton_->setObjectName("load");

  definedButtonsLayout->addWidget(addColorButton_);
  definedButtonsLayout->addWidget(loadColorsButton_);
  definedButtonsLayout->addStretch(1);

  connect(addColorButton_  , SIGNAL(clicked()), this, SLOT(addColorSlot()));
  connect(loadColorsButton_, SIGNAL(clicked()), this, SLOT(loadColorsSlot()));

  definedFrameLayout->addWidget(definedButtonsFrame);

  stack_->addWidget(definedFrame);

  //---

  QFrame *functionsFrame = new QFrame;

  functionsFrame->setObjectName("functionsFrame");

  QGridLayout *functionsGridLayout = new QGridLayout(functionsFrame);
  functionsGridLayout->setMargin(2); functionsGridLayout->setSpacing(2);

  createFunctionEdit(functionsGridLayout, 0, "R", &redFunction_  );
  createFunctionEdit(functionsGridLayout, 1, "G", &greenFunction_);
  createFunctionEdit(functionsGridLayout, 2, "B", &blueFunction_ );

  functionsGridLayout->setRowStretch(3, 1);

  setRedFunction  (pal->redFunction  ());
  setGreenFunction(pal->greenFunction());
  setBlueFunction (pal->blueFunction ());

  connect(redFunction_  , SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(greenFunction_, SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(blueFunction_ , SIGNAL(editingFinished()), this, SLOT(functionChanged()));

  stack_->addWidget(functionsFrame);

  //---

  QFrame *cubeFrame = new QFrame;

  cubeFrame->setObjectName("cubeFrame");

  QGridLayout *cubeGridLayout = new QGridLayout(cubeFrame);
  cubeGridLayout->setMargin(2); cubeGridLayout->setSpacing(2);

  createRealEdit(cubeGridLayout, 0, "Start"     , &cubeStart_     );
  createRealEdit(cubeGridLayout, 1, "Cycles"    , &cubeCycles_    );
  createRealEdit(cubeGridLayout, 2, "Saturation", &cubeSaturation_);

  cubeNegativeCheck_ = new QCheckBox("Negative");

  cubeNegativeCheck_->setObjectName("negative");

  connect(cubeNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(cubeNegativeChecked(int)));

  cubeGridLayout->addWidget(cubeNegativeCheck_, 3, 0, 1, 2);

  cubeGridLayout->setRowStretch(4, 1);

  setCubeStart     (pal->cbStart     ());
  setCubeCycles    (pal->cbCycles    ());
  setCubeSaturation(pal->cbSaturation());

  connect(cubeStart_     , SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));
  connect(cubeCycles_    , SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));
  connect(cubeSaturation_, SIGNAL(valueChanged(double)), this, SLOT(cubeValueChanged(double)));

  stack_->addWidget(cubeFrame);

  //---

  layout->addWidget(stack_);
  layout->addStretch(1);

  //---

  connect(this, SIGNAL(stateChanged()), palette, SLOT(update()));
}

void
CQGradientControlIFace::
colorTypeChanged(int)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setColorType(colorType_->type());

  if      (pal->colorType() == CGradientPalette::ColorType::MODEL)
    stack_->setCurrentIndex(0);
  else if (pal->colorType() == CGradientPalette::ColorType::DEFINED)
    stack_->setCurrentIndex(1);
  else if (pal->colorType() == CGradientPalette::ColorType::FUNCTIONS)
    stack_->setCurrentIndex(2);
  else if (pal->colorType() == CGradientPalette::ColorType::CUBEHELIX)
    stack_->setCurrentIndex(3);

  emit stateChanged();
}

void
CQGradientControlIFace::
colorModelChanged(int)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setColorModel(colorModel_->model());

  if (pal->colorModel() == CGradientPalette::ColorModel::HSV) {
    redModelLabel_  ->setText("H");
    greenModelLabel_->setText("S");
    blueModelLabel_ ->setText("V");

    modelRNegativeCheck_->setText("H");
    modelGNegativeCheck_->setText("S");
    modelBNegativeCheck_->setText("V");

    redMinMaxLabel_  ->setText("H");
    greenMinMaxLabel_->setText("S");
    blueMinMaxLabel_ ->setText("V");
  }
  else {
    redModelLabel_  ->setText("R");
    greenModelLabel_->setText("G");
    blueModelLabel_ ->setText("B");

    modelRNegativeCheck_->setText("R");
    modelGNegativeCheck_->setText("G");
    modelBNegativeCheck_->setText("B");

    redMinMaxLabel_  ->setText("R");
    greenMinMaxLabel_->setText("G");
    blueMinMaxLabel_ ->setText("B");
  }

  emit stateChanged();
}

int
CQGradientControlIFace::
redModel() const
{
  return redModelCombo_->currentIndex();
}

void
CQGradientControlIFace::
setRedModel(int model)
{
  redModelCombo_->setCurrentIndex(model);
}

int
CQGradientControlIFace::
greenModel() const
{
  return greenModelCombo_->currentIndex();
}

void
CQGradientControlIFace::
setGreenModel(int model)
{
  greenModelCombo_->setCurrentIndex(model);
}

int
CQGradientControlIFace::
blueModel() const
{
  return blueModelCombo_->currentIndex();
}

void
CQGradientControlIFace::
setBlueModel(int model)
{
  blueModelCombo_->setCurrentIndex(model);
}

void
CQGradientControlIFace::
modelChanged(int model)
{
  CQGradientControlModel *me = qobject_cast<CQGradientControlModel *>(sender());
  assert(me);

  CGradientPalette *pal = palette_->gradientPalette();

  if      (me == redModelCombo_  ) pal->setRedModel  (model);
  else if (me == greenModelCombo_) pal->setGreenModel(model);
  else if (me == blueModelCombo_ ) pal->setBlueModel (model);

  emit stateChanged();
}

void
CQGradientControlIFace::
modelRNegativeChecked(int state)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setRedNegative(state);

  emit stateChanged();
}

void
CQGradientControlIFace::
modelGNegativeChecked(int state)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setGreenNegative(state);

  emit stateChanged();
}

void
CQGradientControlIFace::
modelBNegativeChecked(int state)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setBlueNegative(state);

  emit stateChanged();
}

void
CQGradientControlIFace::
modelRangeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CGradientPalette *pal = palette_->gradientPalette();

  if      (rs == redMin_  ) pal->setRedMin  (r);
  else if (rs == redMax_  ) pal->setRedMax  (r);
  else if (rs == greenMin_) pal->setGreenMin(r);
  else if (rs == greenMax_) pal->setGreenMax(r);
  else if (rs == blueMin_ ) pal->setBlueMin (r);
  else if (rs == blueMax_ ) pal->setBlueMax (r);

  emit stateChanged();
}

void
CQGradientControlIFace::
cubeNegativeChecked(int state)
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->setCubeNegative(state);

  emit stateChanged();
}

void
CQGradientControlIFace::
colorsChanged()
{
  CGradientPalette *pal = palette_->gradientPalette();

  pal->resetDefinedColors();

  for (int i = 0; i < definedColors_->numRealColors(); ++i) {
    const CQGradientControlDefinedColors::RealColor &realColor = definedColors_->realColor(i);

    pal->addDefinedColor(realColor.r, realColor.c);
  }

  emit stateChanged();
}

void
CQGradientControlIFace::
addColorSlot()
{
  CGradientPalette *pal = palette_->gradientPalette();

  int row = 0;

  QList<QTableWidgetItem *> selectedItems = definedColors_->selectedItems();

  if (selectedItems.length())
    row = selectedItems[0]->row();

  const CGradientPalette::ColorMap colors = pal->colors();

  double x = 0.5;
  QColor c = QColor(127, 127, 127);

  int row1 = -1;

  if      (row + 1 < int(colors.size()))
    row1 = row;
  else if (row < int(colors.size()) && row > 0)
    row1 = row - 1;

  if (row1 >= 0) {
    auto p = colors.begin();

    std::advance(p, row1);

    double        x1 = (*p).first;
    const QColor &c1 = (*p).second;

    ++p;
    double        x2 = (*p).first;
    const QColor &c2 = (*p).second;

    x = (x1 + x2)/2;

    if      (pal->colorModel() == CGradientPalette::ColorModel::RGB)
      c = CGradientPalette::interpRGB(c1, c2, 0.5);
    else if (pal->colorModel() == CGradientPalette::ColorModel::HSV)
      c = CGradientPalette::interpHSV(c1, c2, 0.5);
    else
      c = CGradientPalette::interpRGB(c1, c2, 0.5);
  }

  pal->addDefinedColor(x, c);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQGradientControlIFace::
loadColorsSlot()
{
  CGradientPalette *pal = palette_->gradientPalette();

  QString dir = QDir::current().dirName();

  QString fileName = QFileDialog::getOpenFileName(this, "Open File", dir, "Files (*.*)");

  if (! fileName.length())
    return;

  pal->readFile(fileName.toStdString());

  definedColors_->updateColors(pal);
}

std::string
CQGradientControlIFace::
redFunction() const
{
  return redFunction_->text().toStdString();
}

void
CQGradientControlIFace::
setRedFunction(const std::string &fn)
{
  redFunction_->setText(fn.c_str());
}

std::string
CQGradientControlIFace::
greenFunction() const
{
  return greenFunction_->text().toStdString();
}

void
CQGradientControlIFace::
setGreenFunction(const std::string &fn)
{
  greenFunction_->setText(fn.c_str());
}

std::string
CQGradientControlIFace::
blueFunction() const
{
  return blueFunction_->text().toStdString();
}

void
CQGradientControlIFace::
setBlueFunction(const std::string &fn)
{
  blueFunction_->setText(fn.c_str());
}

void
CQGradientControlIFace::
functionChanged()
{
  QLineEdit *le = qobject_cast<QLineEdit *>(sender());
  assert(le);

  CGradientPalette *pal = palette_->gradientPalette();

  if      (le == redFunction_)
    pal->setRedFunction  (le->text().toStdString());
  else if (le == greenFunction_)
    pal->setGreenFunction(le->text().toStdString());
  else if (le == blueFunction_)
    pal->setBlueFunction (le->text().toStdString());

  emit stateChanged();
}

double
CQGradientControlIFace::
cubeStart() const
{
  return cubeStart_->value();
}

void
CQGradientControlIFace::
setCubeStart(double r)
{
  cubeStart_->setValue(r);
}

double
CQGradientControlIFace::
cubeCycles() const
{
  return cubeCycles_->value();
}

void
CQGradientControlIFace::
setCubeCycles(double r)
{
  cubeCycles_->setValue(r);
}

double
CQGradientControlIFace::
cubeSaturation() const
{
  return cubeSaturation_->value();
}

void
CQGradientControlIFace::
setCubeSaturation(double r)
{
  cubeSaturation_->setValue(r);
}

void
CQGradientControlIFace::
cubeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CGradientPalette *pal = palette_->gradientPalette();

  if      (rs == cubeStart_)
    pal->setCbStart(r);
  else if (rs == cubeCycles_)
    pal->setCbCycles(r);
  else if (rs == cubeSaturation_)
    pal->setCbSaturation(r);

  emit stateChanged();
}

QFrame *
CQGradientControlIFace::
createColorTypeCombo(const QString &label, CQGradientControlColorType **type)
{
  QFrame *frame = new QFrame;

  frame->setObjectName("frame");

  QHBoxLayout *layout = new QHBoxLayout(frame);
  layout->setMargin(2); layout->setSpacing(2);

  *type = new CQGradientControlColorType;

  layout->addWidget(new QLabel(label));
  layout->addWidget(*type);
  layout->addStretch(1);

  return frame;
}

QFrame *
CQGradientControlIFace::
createColorModelCombo(const QString &label, CQGradientControlColorModel **model)
{
  QFrame *frame = new QFrame;

  frame->setObjectName("frame");

  QHBoxLayout *layout = new QHBoxLayout(frame);
  layout->setMargin(2); layout->setSpacing(2);

  *model = new CQGradientControlColorModel;

  layout->addWidget(new QLabel(label));
  layout->addWidget(*model);
  layout->addStretch(1);

  return frame;
}

void
CQGradientControlIFace::
createModelCombo(QGridLayout *grid, int row, const QString &label,
                 QLabel **modelLabel, CQGradientControlModel **modelCombo)
{
  *modelLabel = new QLabel(label);
  *modelCombo = new CQGradientControlModel;

  grid->addWidget(*modelLabel, row, 0);
  grid->addWidget(*modelCombo, row, 1);

  grid->setColumnStretch(2, true);
}

void
CQGradientControlIFace::
createFunctionEdit(QGridLayout *grid, int row, const QString &label, QLineEdit **edit)
{
  *edit = new QLineEdit;

  grid->addWidget(new QLabel(label), row, 0);
  grid->addWidget(*edit, row, 1);

  grid->setColumnStretch(2, true);
}

void
CQGradientControlIFace::
createRealEdit(QGridLayout *grid, int row, const QString &label, CQRealSpin **edit)
{
  createRealEdit(grid, row, 0, true, label, edit);
}

void
CQGradientControlIFace::
createRealEdit(QGridLayout *grid, int row, int col, bool stretch,
               const QString &label, CQRealSpin **edit)
{
  *edit = new CQRealSpin;

  grid->addWidget(new QLabel(label), row, col);
  grid->addWidget(*edit, row, col + 1);

  if (stretch)
    grid->setColumnStretch(col + 2, true);
}

//---

CQGradientControlColorType::
CQGradientControlColorType(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorType");

  addItem("Model"     , QVariant(static_cast<int>(CGradientPalette::ColorType::MODEL    )));
  addItem("Defined"   , QVariant(static_cast<int>(CGradientPalette::ColorType::DEFINED  )));
  addItem("Functions" , QVariant(static_cast<int>(CGradientPalette::ColorType::FUNCTIONS)));
  addItem("Cube Helix", QVariant(static_cast<int>(CGradientPalette::ColorType::CUBEHELIX)));
}

CGradientPalette::ColorType
CQGradientControlColorType::
type() const
{
  return static_cast<CGradientPalette::ColorType>(itemData(currentIndex()).toInt());
}

void
CQGradientControlColorType::
setType(const CGradientPalette::ColorType &type)
{
  QVariant var(static_cast<int>(type));

  for (int i = 0; i < count(); ++i)
    if (itemData(i).toInt() == var.toInt())
      setCurrentIndex(i);
}

//---

CQGradientControlColorModel::
CQGradientControlColorModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorModel");

  addItem("RGB", QVariant(static_cast<int>(CGradientPalette::ColorModel::RGB)));
  addItem("HSV", QVariant(static_cast<int>(CGradientPalette::ColorModel::HSV)));
}

CGradientPalette::ColorModel
CQGradientControlColorModel::
model() const
{
  return static_cast<CGradientPalette::ColorModel>(itemData(currentIndex()).toInt());
}

void
CQGradientControlColorModel::
setModel(const CGradientPalette::ColorModel &model)
{
  QVariant var(static_cast<int>(model));

  for (int i = 0; i < count(); ++i)
    if (itemData(i).toInt() == var.toInt())
      setCurrentIndex(i);
}

//---

CQGradientControlModel::
CQGradientControlModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("model");

  for (int i = 0; i < CGradientPalette::numModels(); ++i)
    addItem(CGradientPalette::modelName(i).c_str());
}

//---

class CQGradientControlDefinedColorsDelegate : public QItemDelegate {
 public:
  CQGradientControlDefinedColorsDelegate(CQGradientControlDefinedColors *colors_);

  // Override to create editor
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &index) const;

  // Override to set editor from content
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

 private:
  CQGradientControlDefinedColors *colors_;
};

CQGradientControlDefinedColors::
CQGradientControlDefinedColors(QWidget *parent) :
 QTableWidget(parent)
{
  setObjectName("defineColors");

  verticalHeader()->hide();

  CQGradientControlDefinedColorsDelegate *delegate =
    new CQGradientControlDefinedColorsDelegate(this);

  setItemDelegate(delegate);
  setEditTriggers(QAbstractItemView::AllEditTriggers);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  QHeaderView *header = horizontalHeader();

  header->setStretchLastSection(true) ;
}

void
CQGradientControlDefinedColors::
updateColors(CGradientPalette *palette)
{
  setColumnCount(2);

  setHorizontalHeaderLabels(QStringList() << "X" << "Color");

  setRowCount(palette->numColors());

  realColors_.clear();

  for (const auto &c : palette->colors())
    realColors_.emplace_back(c.first, c.second.rgba());

  for (int r = 0; r < numRealColors(); ++r) {
    const RealColor &realColor = this->realColor(r);

    QTableWidgetItem *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
    QTableWidgetItem *item2 = new QTableWidgetItem(realColor.c.name());

    setItem(r, 0, item1);
    setItem(r, 1, item2);
  }
}

const CQGradientControlDefinedColors::RealColor &
CQGradientControlDefinedColors::
realColor(int r) const
{
  return realColors_[r];
}

void
CQGradientControlDefinedColors::
setRealColor(int r, const RealColor &realColor)
{
  realColors_[r] = realColor;

  //QTableWidgetItem *item1 = new QTableWidgetItem(QString("%1").arg(realColor.r));
  //QTableWidgetItem *item2 = new QTableWidgetItem(realColor.c.name());

  //setItem(r, 0, item1);
  //setItem(r, 1, item2);

  emit colorsChanged();
}

//---

CQGradientControlDefinedColorsDelegate::
CQGradientControlDefinedColorsDelegate(CQGradientControlDefinedColors *colors_) :
 QItemDelegate(colors_), colors_(colors_)
{
}

QWidget *
CQGradientControlDefinedColorsDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QTableWidgetItem *item = colors_->item(index.row(), index.column());
  assert(item);

  const CQGradientControlDefinedColors::RealColor &realColor = colors_->realColor(index.row());

  if       (index.column() == 0) {
    CQRealSpin *edit = new CQRealSpin(parent);

    edit->setValue(realColor.r);

    return edit;
  }
  else if (index.column() == 1) {
    CQColorChooser *edit = new CQColorChooser(parent);

    edit->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

    edit->setColor(realColor.c);

    return edit;
  }
  else
    return QItemDelegate::createEditor(parent, option, index);
}

// model->editor
void
CQGradientControlDefinedColorsDelegate::
setEditorData(QWidget *w, const QModelIndex &index) const
{
  QTableWidgetItem *item = colors_->item(index.row(), index.column());
  assert(item);

  const CQGradientControlDefinedColors::RealColor &realColor = colors_->realColor(index.row());

  if       (index.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    edit->setValue(realColor.r);
  }
  else if (index.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    const QColor &c = realColor.c;

    edit->setColor(c);
  }
  else
    QItemDelegate::setEditorData(w, index);
}

// editor->model
void
CQGradientControlDefinedColorsDelegate::
setModelData(QWidget *w, QAbstractItemModel *model, const QModelIndex &index) const
{
  QTableWidgetItem *item = colors_->item(index.row(), index.column());
  assert(item);

  CQGradientControlDefinedColors::RealColor realColor = colors_->realColor(index.row());

  if       (index.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    double r = edit->value();

    model->setData(index, QVariant(r));

    realColor.r = r;
  }
  else if (index.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    realColor.c = edit->color();

    model->setData(index, QVariant(realColor.c));
  }
  else
    QItemDelegate::setModelData(w, model, index);

  colors_->setRealColor(index.row(), realColor);
}

QSize
CQGradientControlDefinedColorsDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QTableWidgetItem *item = colors_->item(index.row(), index.column());
  assert(item);

  return QItemDelegate::sizeHint(option, index);
}

void
CQGradientControlDefinedColorsDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

void
CQGradientControlDefinedColorsDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QTableWidgetItem *item = colors_->item(index.row(), index.column());
  assert(item);

  QItemDelegate::paint(painter, option, index);
}
