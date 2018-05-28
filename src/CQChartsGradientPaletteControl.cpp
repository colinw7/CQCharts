#include <CQChartsGradientPaletteControl.h>
#include <CQChartsGradientPaletteCanvas.h>
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

CQChartsGradientPaletteControl::
CQChartsGradientPaletteControl(CQChartsGradientPaletteCanvas *palette) :
 QFrame(0), palette_(palette)
{
  setObjectName("paletteControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  QFontMetricsF fm(font());

  //---

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  CQChartsGradientPalette *pal = palette_->gradientPalette();

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

  // red, green, blue function combos
  QGroupBox *functionGroupBox = new QGroupBox("Function");
  functionGroupBox->setObjectName("function");

  functionGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

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

  // red, green, blue negative check boxes
  QGroupBox *negateGroupBox = new QGroupBox("Negate");
  negateGroupBox->setObjectName("negate");

  negateGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  QHBoxLayout *negateGroupLayout = new QHBoxLayout(negateGroupBox);
  negateGroupLayout->setMargin(0); negateGroupLayout->setSpacing(2);

  modelLayout->addWidget(negateGroupBox);

  modelRNegativeCheck_ = new QCheckBox("R");
  modelRNegativeCheck_->setObjectName("rnegative");

  modelGNegativeCheck_ = new QCheckBox("G");
  modelGNegativeCheck_->setObjectName("gnegative");

  modelBNegativeCheck_ = new QCheckBox("B");
  modelBNegativeCheck_->setObjectName("bnegative");

  connect(modelRNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelRNegativeChecked(int)));
  connect(modelGNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelGNegativeChecked(int)));
  connect(modelBNegativeCheck_, SIGNAL(stateChanged(int)), this, SLOT(modelBNegativeChecked(int)));

  negateGroupLayout->addWidget(modelRNegativeCheck_);
  negateGroupLayout->addWidget(modelGNegativeCheck_);
  negateGroupLayout->addWidget(modelBNegativeCheck_);
  negateGroupLayout->addStretch(1);

  //---

  // red, green, blue min/max edits
  QGroupBox *rangeGroupBox = new QGroupBox("Range");
  rangeGroupBox->setObjectName("range");

  rangeGroupBox->setContentsMargins(2, fm.height() + 2, 0, 0);

  QHBoxLayout *rangeGroupLayout = new QHBoxLayout(rangeGroupBox);
  rangeGroupLayout->setMargin(0); rangeGroupLayout->setSpacing(2);

  modelLayout->addWidget(rangeGroupBox);

  QGridLayout *rangeGridLayout = new QGridLayout;

  redMinMaxLabel_   = new QLabel("R"); redMinMaxLabel_  ->setObjectName("r");
  greenMinMaxLabel_ = new QLabel("G"); greenMinMaxLabel_->setObjectName("g");
  blueMinMaxLabel_  = new QLabel("B"); blueMinMaxLabel_ ->setObjectName("b");

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

  definedColors_ = new CQChartsGradientPaletteDefinedColors;

  definedColors_->updateColors(pal);

  definedFrameLayout->addWidget(definedColors_);

  connect(definedColors_, SIGNAL(colorsChanged()), this, SLOT(colorsChanged()));

  QFrame *definedButtonsFrame = new QFrame;
  definedButtonsFrame->setObjectName("definedButtonsFrame");

  QHBoxLayout *definedButtonsLayout = new QHBoxLayout(definedButtonsFrame);
  definedButtonsLayout->setMargin(2); definedButtonsLayout->setSpacing(2);

  addColorButton_ = new QPushButton("Add");
  addColorButton_->setObjectName("add");

  loadColorsButton_ = new QPushButton("Load");
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

  createFunctionEdit(functionsGridLayout, 0, "R", &redFunctionLabel_  , &redFunctionEdit_  );
  createFunctionEdit(functionsGridLayout, 1, "G", &greenFunctionLabel_, &greenFunctionEdit_);
  createFunctionEdit(functionsGridLayout, 2, "B", &blueFunctionLabel_ , &blueFunctionEdit_ );

  functionsGridLayout->setRowStretch(3, 1);

  setRedFunction  (pal->redFunction  ());
  setGreenFunction(pal->greenFunction());
  setBlueFunction (pal->blueFunction ());

  connect(redFunctionEdit_  , SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(greenFunctionEdit_, SIGNAL(editingFinished()), this, SLOT(functionChanged()));
  connect(blueFunctionEdit_ , SIGNAL(editingFinished()), this, SLOT(functionChanged()));

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

  //---

  updateColorType ();
  updateColorModel();
}

void
CQChartsGradientPaletteControl::
updateState()
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  setColorType (pal->colorType ());
  setColorModel(pal->colorModel());

  setRedModel  (pal->redModel  ());
  setGreenModel(pal->greenModel());
  setBlueModel (pal->blueModel ());

  modelRNegativeCheck_->setChecked(pal->isRedNegative  ());
  modelGNegativeCheck_->setChecked(pal->isGreenNegative());
  modelBNegativeCheck_->setChecked(pal->isBlueNegative ());

  redMin_  ->setValue(pal->redMin  ());
  redMax_  ->setValue(pal->redMax  ());
  greenMin_->setValue(pal->greenMin());
  greenMax_->setValue(pal->greenMax());
  blueMin_ ->setValue(pal->blueMin ());
  blueMax_ ->setValue(pal->blueMax ());

  //---

  setCubeStart     (pal->cbStart     ());
  setCubeCycles    (pal->cbCycles    ());
  setCubeSaturation(pal->cbSaturation());

  //---

  definedColors_->updateColors(pal);
}

void
CQChartsGradientPaletteControl::
colorTypeChanged(int)
{
  setColorType(colorType_->type());
}

CQChartsGradientPalette::ColorType
CQChartsGradientPaletteControl::
colorType() const
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  return pal->colorType();
}

void
CQChartsGradientPaletteControl::
setColorType(CQChartsGradientPalette::ColorType colorType)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setColorType(colorType);

  updateColorType();

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
updateColorType()
{
  CQChartsGradientPalette::ColorType colorType = this->colorType();

  if      (colorType == CQChartsGradientPalette::ColorType::MODEL) {
    colorType_->setCurrentIndex(0);
    stack_    ->setCurrentIndex(0);
  }
  else if (colorType == CQChartsGradientPalette::ColorType::DEFINED) {
    colorType_->setCurrentIndex(1);
    stack_    ->setCurrentIndex(1);
  }
  else if (colorType == CQChartsGradientPalette::ColorType::FUNCTIONS) {
    colorType_->setCurrentIndex(2);
    stack_    ->setCurrentIndex(2);
  }
  else if (colorType == CQChartsGradientPalette::ColorType::CUBEHELIX) {
    colorType_->setCurrentIndex(3);
    stack_    ->setCurrentIndex(3);
  }
}

void
CQChartsGradientPaletteControl::
colorModelChanged(int)
{
  setColorModel(colorModel_->model());
}

CQChartsGradientPalette::ColorModel
CQChartsGradientPaletteControl::
colorModel() const
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  return pal->colorModel();
}

void
CQChartsGradientPaletteControl::
setColorModel(CQChartsGradientPalette::ColorModel colorModel)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setColorModel(colorModel);

  updateColorModel();

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
updateColorModel()
{
  CQChartsGradientPalette::ColorModel colorModel = this->colorModel();

  if      (colorModel == CQChartsGradientPalette::ColorModel::RGB) {
    colorModel_->setCurrentIndex(0);

    redModelLabel_  ->setText("R");
    greenModelLabel_->setText("G");
    blueModelLabel_ ->setText("B");

    modelRNegativeCheck_->setText("R");
    modelGNegativeCheck_->setText("G");
    modelBNegativeCheck_->setText("B");

    redMinMaxLabel_  ->setText("R");
    greenMinMaxLabel_->setText("G");
    blueMinMaxLabel_ ->setText("B");

    redFunctionLabel_  ->setText("R");
    greenFunctionLabel_->setText("G");
    blueFunctionLabel_ ->setText("B");
  }
  else if (colorModel == CQChartsGradientPalette::ColorModel::HSV) {
    colorModel_->setCurrentIndex(1);

    redModelLabel_  ->setText("H");
    greenModelLabel_->setText("S");
    blueModelLabel_ ->setText("V");

    modelRNegativeCheck_->setText("H");
    modelGNegativeCheck_->setText("S");
    modelBNegativeCheck_->setText("V");

    redMinMaxLabel_  ->setText("H");
    greenMinMaxLabel_->setText("S");
    blueMinMaxLabel_ ->setText("V");

    redFunctionLabel_  ->setText("H");
    greenFunctionLabel_->setText("S");
    blueFunctionLabel_ ->setText("V");
  }
}

int
CQChartsGradientPaletteControl::
redModel() const
{
  return redModelCombo_->currentIndex();
}

void
CQChartsGradientPaletteControl::
setRedModel(int model)
{
  redModelCombo_->setCurrentIndex(model);
}

int
CQChartsGradientPaletteControl::
greenModel() const
{
  return greenModelCombo_->currentIndex();
}

void
CQChartsGradientPaletteControl::
setGreenModel(int model)
{
  greenModelCombo_->setCurrentIndex(model);
}

int
CQChartsGradientPaletteControl::
blueModel() const
{
  return blueModelCombo_->currentIndex();
}

void
CQChartsGradientPaletteControl::
setBlueModel(int model)
{
  blueModelCombo_->setCurrentIndex(model);
}

void
CQChartsGradientPaletteControl::
modelChanged(int model)
{
  CQChartsGradientPaletteModel *me = qobject_cast<CQChartsGradientPaletteModel *>(sender());
  assert(me);

  CQChartsGradientPalette *pal = palette_->gradientPalette();

  if      (me == redModelCombo_  ) pal->setRedModel  (model);
  else if (me == greenModelCombo_) pal->setGreenModel(model);
  else if (me == blueModelCombo_ ) pal->setBlueModel (model);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
modelRNegativeChecked(int state)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setRedNegative(state);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
modelGNegativeChecked(int state)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setGreenNegative(state);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
modelBNegativeChecked(int state)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setBlueNegative(state);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
modelRangeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CQChartsGradientPalette *pal = palette_->gradientPalette();

  if      (rs == redMin_  ) pal->setRedMin  (r);
  else if (rs == redMax_  ) pal->setRedMax  (r);
  else if (rs == greenMin_) pal->setGreenMin(r);
  else if (rs == greenMax_) pal->setGreenMax(r);
  else if (rs == blueMin_ ) pal->setBlueMin (r);
  else if (rs == blueMax_ ) pal->setBlueMax (r);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
cubeNegativeChecked(int state)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->setCubeNegative(state);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
colorsChanged()
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->resetDefinedColors();

  for (int i = 0; i < definedColors_->numRealColors(); ++i) {
    const CQChartsGradientPaletteDefinedColors::RealColor &realColor = definedColors_->realColor(i);

    pal->addDefinedColor(realColor.r, realColor.c);
  }

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
addColorSlot()
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  int row = 0;

  QList<QTableWidgetItem *> selectedItems = definedColors_->selectedItems();

  if (selectedItems.length())
    row = selectedItems[0]->row();

  const CQChartsGradientPalette::ColorMap colors = pal->colors();

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

    if      (pal->colorModel() == CQChartsGradientPalette::ColorModel::RGB)
      c = CQChartsGradientPalette::interpRGB(c1, c2, 0.5);
    else if (pal->colorModel() == CQChartsGradientPalette::ColorModel::HSV)
      c = CQChartsGradientPalette::interpHSV(c1, c2, 0.5);
    else
      c = CQChartsGradientPalette::interpRGB(c1, c2, 0.5);
  }

  pal->addDefinedColor(x, c);

  definedColors_->updateColors(pal);

  emit stateChanged();
}

void
CQChartsGradientPaletteControl::
loadColorsSlot()
{
  QString dir = QDir::current().dirName();

  QString fileName = QFileDialog::getOpenFileName(this, "Open File", dir, "Files (*.*)");

  if (! fileName.length())
    return;

  readFile(fileName);
}

void
CQChartsGradientPaletteControl::
readFile(const QString &fileName)
{
  CQChartsGradientPalette *pal = palette_->gradientPalette();

  pal->readFile(fileName.toStdString());

  definedColors_->updateColors(pal);
}

std::string
CQChartsGradientPaletteControl::
redFunction() const
{
  return redFunctionEdit_->text().toStdString();
}

void
CQChartsGradientPaletteControl::
setRedFunction(const std::string &fn)
{
  redFunctionEdit_->setText(fn.c_str());
}

std::string
CQChartsGradientPaletteControl::
greenFunction() const
{
  return greenFunctionEdit_->text().toStdString();
}

void
CQChartsGradientPaletteControl::
setGreenFunction(const std::string &fn)
{
  greenFunctionEdit_->setText(fn.c_str());
}

std::string
CQChartsGradientPaletteControl::
blueFunction() const
{
  return blueFunctionEdit_->text().toStdString();
}

void
CQChartsGradientPaletteControl::
setBlueFunction(const std::string &fn)
{
  blueFunctionEdit_->setText(fn.c_str());
}

void
CQChartsGradientPaletteControl::
functionChanged()
{
  QLineEdit *le = qobject_cast<QLineEdit *>(sender());
  assert(le);

  CQChartsGradientPalette *pal = palette_->gradientPalette();

  if      (le == redFunctionEdit_)
    pal->setRedFunction  (le->text().toStdString());
  else if (le == greenFunctionEdit_)
    pal->setGreenFunction(le->text().toStdString());
  else if (le == blueFunctionEdit_)
    pal->setBlueFunction (le->text().toStdString());

  emit stateChanged();
}

double
CQChartsGradientPaletteControl::
cubeStart() const
{
  return cubeStart_->value();
}

void
CQChartsGradientPaletteControl::
setCubeStart(double r)
{
  cubeStart_->setValue(r);
}

double
CQChartsGradientPaletteControl::
cubeCycles() const
{
  return cubeCycles_->value();
}

void
CQChartsGradientPaletteControl::
setCubeCycles(double r)
{
  cubeCycles_->setValue(r);
}

double
CQChartsGradientPaletteControl::
cubeSaturation() const
{
  return cubeSaturation_->value();
}

void
CQChartsGradientPaletteControl::
setCubeSaturation(double r)
{
  cubeSaturation_->setValue(r);
}

void
CQChartsGradientPaletteControl::
cubeValueChanged(double r)
{
  CQRealSpin *rs = qobject_cast<CQRealSpin *>(sender());
  assert(rs);

  CQChartsGradientPalette *pal = palette_->gradientPalette();

  if      (rs == cubeStart_)
    pal->setCbStart(r);
  else if (rs == cubeCycles_)
    pal->setCbCycles(r);
  else if (rs == cubeSaturation_)
    pal->setCbSaturation(r);

  emit stateChanged();
}

QFrame *
CQChartsGradientPaletteControl::
createColorTypeCombo(const QString &label, CQChartsGradientPaletteColorType **type)
{
  QFrame *frame = new QFrame;
  frame->setObjectName("frame");

  QHBoxLayout *layout = new QHBoxLayout(frame);
  layout->setMargin(2); layout->setSpacing(2);

  *type = new CQChartsGradientPaletteColorType;

  QLabel *colorLabel = new QLabel(label);
  colorLabel->setObjectName("label");

  layout->addWidget(colorLabel);
  layout->addWidget(*type);
  layout->addStretch(1);

  return frame;
}

QFrame *
CQChartsGradientPaletteControl::
createColorModelCombo(const QString &label, CQChartsGradientPaletteColorModel **model)
{
  QFrame *frame = new QFrame;
  frame->setObjectName("frame");

  QHBoxLayout *layout = new QHBoxLayout(frame);
  layout->setMargin(2); layout->setSpacing(2);

  *model = new CQChartsGradientPaletteColorModel;

  QLabel *colorLabel = new QLabel(label);
  colorLabel->setObjectName("label");

  layout->addWidget(colorLabel);
  layout->addWidget(*model);
  layout->addStretch(1);

  return frame;
}

void
CQChartsGradientPaletteControl::
createModelCombo(QGridLayout *grid, int row, const QString &label,
                 QLabel **modelLabel, CQChartsGradientPaletteModel **modelCombo)
{
  *modelLabel = new QLabel(label);
  (*modelLabel)->setObjectName("label");

  *modelCombo = new CQChartsGradientPaletteModel;
  (*modelCombo)->setObjectName("combo");

  grid->addWidget(*modelLabel, row, 0);
  grid->addWidget(*modelCombo, row, 1);

  grid->setColumnStretch(2, true);
}

void
CQChartsGradientPaletteControl::
createFunctionEdit(QGridLayout *grid, int row, const QString &label,
                   QLabel **functionLabel, QLineEdit **functionEdit)
{
  *functionLabel = new QLabel(label);
  (*functionLabel)->setObjectName("label");

  *functionEdit  = new QLineEdit;
  (*functionEdit )->setObjectName("edit");

  grid->addWidget(*functionLabel, row, 0);
  grid->addWidget(*functionEdit , row, 1);

  grid->setColumnStretch(2, true);
}

void
CQChartsGradientPaletteControl::
createRealEdit(QGridLayout *grid, int row, const QString &label, CQRealSpin **edit)
{
  createRealEdit(grid, row, 0, true, label, edit);
}

void
CQChartsGradientPaletteControl::
createRealEdit(QGridLayout *grid, int row, int col, bool stretch,
               const QString &label, CQRealSpin **edit)
{
  *edit = new CQRealSpin;
  (*edit)->setObjectName("spin");

  QLabel *editLabel = new QLabel(label);
  editLabel->setObjectName("label");

  grid->addWidget(editLabel, row, col);
  grid->addWidget(*edit    , row, col + 1);

  if (stretch)
    grid->setColumnStretch(col + 2, true);
}

//---

CQChartsGradientPaletteColorType::
CQChartsGradientPaletteColorType(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorType");

  addItem("Model"     , QVariant(static_cast<int>(CQChartsGradientPalette::ColorType::MODEL    )));
  addItem("Defined"   , QVariant(static_cast<int>(CQChartsGradientPalette::ColorType::DEFINED  )));
  addItem("Functions" , QVariant(static_cast<int>(CQChartsGradientPalette::ColorType::FUNCTIONS)));
  addItem("Cube Helix", QVariant(static_cast<int>(CQChartsGradientPalette::ColorType::CUBEHELIX)));
}

CQChartsGradientPalette::ColorType
CQChartsGradientPaletteColorType::
type() const
{
  return static_cast<CQChartsGradientPalette::ColorType>(itemData(currentIndex()).toInt());
}

void
CQChartsGradientPaletteColorType::
setType(const CQChartsGradientPalette::ColorType &type)
{
  QVariant var(static_cast<int>(type));

  for (int i = 0; i < count(); ++i)
    if (itemData(i).toInt() == var.toInt())
      setCurrentIndex(i);
}

//---

CQChartsGradientPaletteColorModel::
CQChartsGradientPaletteColorModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("colorModel");

  addItem("RGB", QVariant(static_cast<int>(CQChartsGradientPalette::ColorModel::RGB)));
  addItem("HSV", QVariant(static_cast<int>(CQChartsGradientPalette::ColorModel::HSV)));
}

CQChartsGradientPalette::ColorModel
CQChartsGradientPaletteColorModel::
model() const
{
  return static_cast<CQChartsGradientPalette::ColorModel>(itemData(currentIndex()).toInt());
}

void
CQChartsGradientPaletteColorModel::
setModel(const CQChartsGradientPalette::ColorModel &model)
{
  QVariant var(static_cast<int>(model));

  for (int i = 0; i < count(); ++i)
    if (itemData(i).toInt() == var.toInt())
      setCurrentIndex(i);
}

//---

CQChartsGradientPaletteModel::
CQChartsGradientPaletteModel(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("model");

  for (int i = 0; i < CQChartsGradientPalette::numModels(); ++i)
    addItem(CQChartsGradientPalette::modelName(i).c_str() + QString(" (%1)").arg(i));
}

//---

class CQChartsGradientPaletteDefinedColorsDelegate : public QItemDelegate {
 public:
  CQChartsGradientPaletteDefinedColorsDelegate(CQChartsGradientPaletteDefinedColors *colors_);

  // Override to create editor
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &ind) const;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &ind) const;

  // Override to set editor from content
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &ind) const;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &ind) const;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &ind) const;

 private:
  CQChartsGradientPaletteDefinedColors *colors_;
};

CQChartsGradientPaletteDefinedColors::
CQChartsGradientPaletteDefinedColors(QWidget *parent) :
 QTableWidget(parent)
{
  setObjectName("defineColors");

  verticalHeader()->hide();

  CQChartsGradientPaletteDefinedColorsDelegate *delegate =
    new CQChartsGradientPaletteDefinedColorsDelegate(this);

  setItemDelegate(delegate);
  setEditTriggers(QAbstractItemView::AllEditTriggers);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  QHeaderView *header = horizontalHeader();

  header->setStretchLastSection(true) ;
}

void
CQChartsGradientPaletteDefinedColors::
updateColors(CQChartsGradientPalette *palette)
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

const CQChartsGradientPaletteDefinedColors::RealColor &
CQChartsGradientPaletteDefinedColors::
realColor(int r) const
{
  return realColors_[r];
}

void
CQChartsGradientPaletteDefinedColors::
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

CQChartsGradientPaletteDefinedColorsDelegate::
CQChartsGradientPaletteDefinedColorsDelegate(CQChartsGradientPaletteDefinedColors *colors_) :
 QItemDelegate(colors_), colors_(colors_)
{
}

QWidget *
CQChartsGradientPaletteDefinedColorsDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const CQChartsGradientPaletteDefinedColors::RealColor &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = new CQRealSpin(parent);
    edit->setObjectName("spin");

    edit->setValue(realColor.r);

    return edit;
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = new CQColorChooser(parent);
    edit->setObjectName("color");

    edit->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

    edit->setColor(realColor.c);

    return edit;
  }
  else
    return QItemDelegate::createEditor(parent, option, ind);
}

// model->editor
void
CQChartsGradientPaletteDefinedColorsDelegate::
setEditorData(QWidget *w, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  const CQChartsGradientPaletteDefinedColors::RealColor &realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    edit->setValue(realColor.r);
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    const QColor &c = realColor.c;

    edit->setColor(c);
  }
  else
    QItemDelegate::setEditorData(w, ind);
}

// editor->model
void
CQChartsGradientPaletteDefinedColorsDelegate::
setModelData(QWidget *w, QAbstractItemModel *model, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  CQChartsGradientPaletteDefinedColors::RealColor realColor = colors_->realColor(ind.row());

  if       (ind.column() == 0) {
    CQRealSpin *edit = qobject_cast<CQRealSpin *>(w);

    double r = edit->value();

    model->setData(ind, QVariant(r));

    realColor.r = r;
  }
  else if (ind.column() == 1) {
    CQColorChooser *edit = qobject_cast<CQColorChooser *>(w);

    realColor.c = edit->color();

    model->setData(ind, QVariant(realColor.c));
  }
  else
    QItemDelegate::setModelData(w, model, ind);

  colors_->setRealColor(ind.row(), realColor);
}

QSize
CQChartsGradientPaletteDefinedColorsDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  return QItemDelegate::sizeHint(option, ind);
}

void
CQChartsGradientPaletteDefinedColorsDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &/* ind */) const
{
  editor->setGeometry(option.rect);
}

void
CQChartsGradientPaletteDefinedColorsDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &ind) const
{
  QTableWidgetItem *item = colors_->item(ind.row(), ind.column());
  assert(item);

  QItemDelegate::paint(painter, option, ind);
}
