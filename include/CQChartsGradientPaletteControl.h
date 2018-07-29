#ifndef CQChartsGradientPaletteControl_H
#define CQChartsGradientPaletteControl_H

#include <CQChartsGradientPalette.h>
#include <QFrame>
#include <QComboBox>
#include <QTableWidget>

class CQChartsGradientPaletteCanvas;
class CQChartsGradientPaletteColorType;
class CQChartsGradientPaletteColorModel;
class CQChartsGradientPaletteModel;
class CQChartsGradientPaletteDefinedColors;
class CQRealSpin;

class QStackedWidget;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QLabel;
class QGridLayout;

// Controls for CQChartsGradientPalette customization
class CQChartsGradientPaletteControl : public QFrame {
  Q_OBJECT

  Q_PROPERTY(int redModel   READ redModel   WRITE setRedModel  )
  Q_PROPERTY(int greenModel READ greenModel WRITE setGreenModel)
  Q_PROPERTY(int blueModel  READ blueModel  WRITE setBlueModel )

 public:
  CQChartsGradientPaletteControl(CQChartsGradientPaletteCanvas *palette);

  //! get/set color type
  CQChartsGradientPalette::ColorType colorType() const;
  void setColorType(CQChartsGradientPalette::ColorType colorType);

  //! get/set color model
  CQChartsGradientPalette::ColorModel colorModel() const;
  void setColorModel(CQChartsGradientPalette::ColorModel colorModel);

  int redModel() const;
  void setRedModel(int mode);

  int greenModel() const;
  void setGreenModel(int mode);

  int blueModel() const;
  void setBlueModel(int mode);

  std::string redFunction() const;
  void setRedFunction(const std::string &fn);

  std::string greenFunction() const;
  void setGreenFunction(const std::string &fn);

  std::string blueFunction() const;
  void setBlueFunction(const std::string &fn);

  double cubeStart() const;
  void setCubeStart(double r);

  double cubeCycles() const;
  void setCubeCycles(double r);

  double cubeSaturation() const;
  void setCubeSaturation(double r);

  void readFile(const QString &fileName);

 signals:
  void stateChanged();

 public slots:
  void updateState();

 private slots:
  void colorTypeChanged(int);
  void colorModelChanged(int);

  void modelChanged(int);

  void modelRNegativeChecked(int state);
  void modelGNegativeChecked(int state);
  void modelBNegativeChecked(int state);

  void modelRangeValueChanged(double v);

  void cubeNegativeChecked(int state);

  void colorsChanged();

  void addColorSlot();
  void removeColorSlot();
  void loadColorsSlot();

  void functionChanged();
  void cubeValueChanged(double r);

 private:
  void updateColorType ();
  void updateColorModel();

  QFrame *createColorTypeCombo(const QString &label, CQChartsGradientPaletteColorType **type);

  QFrame *createColorModelCombo(const QString &label, CQChartsGradientPaletteColorModel **model);

  void createModelCombo(QGridLayout *grid, int row, const QString &label,
                        QLabel **modelLabel, CQChartsGradientPaletteModel **modelCombo);

  void createFunctionEdit(QGridLayout *grid, int row, const QString &label,
                          QLabel **functionLabel, QLineEdit **functionEdit);

  void createRealEdit(QGridLayout *grid, int row, const QString &label, CQRealSpin **edit);

  void createRealEdit(QGridLayout *grid, int row, int col, bool stretch,
                      const QString &label, CQRealSpin **edit);

 private:
  CQChartsGradientPaletteCanvas*        palette_             { nullptr };
  CQChartsGradientPaletteColorType*     colorType_           { nullptr };
  CQChartsGradientPaletteColorModel*    colorModel_          { nullptr };
  QStackedWidget*                       stack_               { nullptr };
  QLabel*                               redModelLabel_       { nullptr };
  CQChartsGradientPaletteModel*         redModelCombo_       { nullptr };
  QLabel*                               greenModelLabel_     { nullptr };
  CQChartsGradientPaletteModel*         greenModelCombo_     { nullptr };
  QLabel*                               blueModelLabel_      { nullptr };
  CQChartsGradientPaletteModel*         blueModelCombo_      { nullptr };
  QCheckBox*                            modelRNegativeCheck_ { nullptr };
  QCheckBox*                            modelGNegativeCheck_ { nullptr };
  QCheckBox*                            modelBNegativeCheck_ { nullptr };
  QLabel*                               redMinMaxLabel_      { nullptr };
  QLabel*                               greenMinMaxLabel_    { nullptr };
  QLabel*                               blueMinMaxLabel_     { nullptr };
  CQRealSpin*                           redMin_              { nullptr };
  CQRealSpin*                           redMax_              { nullptr };
  CQRealSpin*                           greenMin_            { nullptr };
  CQRealSpin*                           greenMax_            { nullptr };
  CQRealSpin*                           blueMin_             { nullptr };
  CQRealSpin*                           blueMax_             { nullptr };
  QLabel*                               redFunctionLabel_    { nullptr };
  QLabel*                               greenFunctionLabel_  { nullptr };
  QLabel*                               blueFunctionLabel_   { nullptr };
  QLineEdit*                            redFunctionEdit_     { nullptr };
  QLineEdit*                            greenFunctionEdit_   { nullptr };
  QLineEdit*                            blueFunctionEdit_    { nullptr };
  CQChartsGradientPaletteDefinedColors* definedColors_       { nullptr };
  QPushButton*                          addColorButton_      { nullptr };
  QPushButton*                          removeColorButton_   { nullptr };
  QPushButton*                          loadColorsButton_    { nullptr };
  CQRealSpin*                           cubeStart_           { nullptr };
  CQRealSpin*                           cubeCycles_          { nullptr };
  CQRealSpin*                           cubeSaturation_      { nullptr };
  QCheckBox*                            cubeNegativeCheck_   { nullptr };
};

//---

class CQChartsGradientPaletteColorType : public QComboBox {
  Q_OBJECT

 public:
  CQChartsGradientPaletteColorType(QWidget *parent=0);

  CQChartsGradientPalette::ColorType type() const;

  void setType(const CQChartsGradientPalette::ColorType &type);
};

//---

class CQChartsGradientPaletteColorModel : public QComboBox {
  Q_OBJECT

 public:
  CQChartsGradientPaletteColorModel(QWidget *parent=0);

  CQChartsGradientPalette::ColorModel model() const;

  void setModel(const CQChartsGradientPalette::ColorModel &model);
};

//---

class CQChartsGradientPaletteModel : public QComboBox {
  Q_OBJECT

 public:
  CQChartsGradientPaletteModel(QWidget *parent=0);
};

//---

class CQChartsGradientPaletteDefinedColors : public QTableWidget {
  Q_OBJECT

 public:
  struct RealColor {
    RealColor(double r1, const QColor &c1) :
     r(r1), c(c1) {
    }

    double r { 0.0 };
    QColor c { 0, 0, 0 };
  };

  using RealColors = std::vector<RealColor>;

 public:
  CQChartsGradientPaletteDefinedColors(QWidget *parent=0);

  void updateColors(CQChartsGradientPalette *palette);

  int numRealColors() const { return realColors_.size(); }

  const RealColor &realColor(int i) const;

  void setRealColor(int i, const RealColor &c);

 signals:
  void colorsChanged();

 private:
  RealColors realColors_;
};

#endif
