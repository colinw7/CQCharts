#ifndef CQChartsGradientPalette_H
#define CQChartsGradientPalette_H

#ifdef CQCharts_USE_CEXPR
#include <CExpr.h>
#endif

#include <QColor>
#include <COSNaN.h>
#include <string>
#include <map>
#include <memory>

class CQTcl;

//---

class CCubeHelix {
 public:
  struct HSL {
    HSL(double h, double s, double l) :
     h(h), s(s), l(l) {
    }

    double h { 0.0 };
    double s { 1.0 };
    double l { 1.0 };
  };

 public:
  CCubeHelix(const HSL &a=HSL( 300/360.0, 0.5, 0.0),
             const HSL &b=HSL(-240/360.0, 0.5, 1.0)) :
   a_(a), b_(b) {
    init();
  }

  void init() {
    double radians = M_PI/180;

    ah_ = 360*(a_.h + start_)*radians;
    bh_ = 360*(b_.h + start_)*radians - ah_;
    as_ = a_.s;
    bs_ = b_.s - as_;
    al_ = a_.l;
    bl_ = b_.l - al_;

    if (COSNaN::is_nan(bs_)) bs_ = 0, as_ = COSNaN::is_nan(as_) ? b_.s : as_;
    if (COSNaN::is_nan(bh_)) bh_ = 0, ah_ = COSNaN::is_nan(ah_) ? b_.h*360 : ah_;
  }

  double start() const { return start_; }
  void setStart(double r) { start_ = r; init(); }

  double cycles() const { return cycles_; }
  void setCycles(double r) { cycles_ = r; }

  double saturation() const { return saturation_; }

  void setSaturation(double r) {
    saturation_ = r;

    a_.s = r;
    b_.s = r;

    init();
  }

  void reset() {
    start_      = 0;
    cycles_     = 1;
    saturation_ = 1;
  }

  QColor interp(double t, bool negate=false) const {
    double h = ah_ + bh_*t;
    double l = pow(al_ + bl_*t, std::max(cycles_, 0.01));
    double a = (as_ + bs_*t)*l*(1 - l);

    double cosh = cos(h);
    double sinh = sin(h);

    double r = l + a*(-0.14861*cosh + 1.78277*sinh);
    double g = l + a*(-0.29227*cosh - 0.90649*sinh);
    double b = l + a*(+1.97294*cosh);

    if (negate) {
      r = 1.0 - r;
      g = 1.0 - g;
      b = 1.0 - b;
    }

    return QColor(255*r, 255*g, 255*b);
  }

 private:
  double start_      { 1.0/3.0 };
  double cycles_     { 1 };
  double saturation_ { 0.5 };

  HSL    a_, b_;
  double ah_, bh_;
  double as_, bs_;
  double al_, bl_;
};

//---

class CQChartsGradientPalette {
 public:
  enum class ColorType {
    MODEL,
    DEFINED,
    FUNCTIONS,
    CUBEHELIX
  };

  enum class ColorModel {
    RGB,
    HSV,
    CMY,
    YIQ,
    XYZ,
  };

  enum class ExprType {
    NONE,
    CEXPR,
    TCL
  };

  using ColorMap = std::map<double,QColor>;

 public:
  CQChartsGradientPalette();

  CQChartsGradientPalette(const CQChartsGradientPalette &palette);

  virtual ~CQChartsGradientPalette();

  //---

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  //---

  CQChartsGradientPalette *dup() const { return new CQChartsGradientPalette(*this); }

  //---

  // color calculation type
  ColorType colorType() const { return colorType_; }
  void setColorType(ColorType t) { colorType_ = t; }

  // color model
  ColorModel colorModel() const { return colorModel_; }
  void setColorModel(ColorModel m) { colorModel_ = m; }

  //---

  // expression type
  const ExprType &exprType() const { return exprType_; }
  void setExprType(const ExprType &type);

  //---

  // gamma correction : NOT USED yet
  double gamma() const { return gamma_; }
  void setGamma(double g) { gamma_ = g; }

  //---

#if 0
  bool isPSAllCF() const { return psAllcF_; }
  void setPSAllCF(bool b) { psAllcF_ = b; }
#endif

  //---

  // model
  void setRgbModel(int r, int g, int b);

  int redModel() const { return rModel_; }
  void setRedModel(int r) { rModel_ = r; }

  int greenModel() const { return gModel_; }
  void setGreenModel(int r) { gModel_ = r; }

  int blueModel() const { return bModel_; }
  void setBlueModel(int r) { bModel_ = r; }

  bool isGray() const { return gray_; }
  void setGray(bool b) { gray_ = b; }

  bool isRedNegative() const { return redNegative_; }
  void setRedNegative(bool b) { redNegative_ = b; }

  bool isGreenNegative() const { return greenNegative_; }
  void setGreenNegative(bool b) { greenNegative_ = b; }

  bool isBlueNegative() const { return blueNegative_; }
  void setBlueNegative(bool b) { blueNegative_ = b; }

  void setRedMin(double r) { redMin_ = std::min(std::max(r, 0.0), 1.0); }
  double redMin() const { return redMin_; }
  void setRedMax(double r) { redMax_ = std::min(std::max(r, 0.0), 1.0); }
  double redMax() const { return redMax_; }

  void setGreenMin(double r) { greenMin_ = std::min(std::max(r, 0.0), 1.0); }
  double greenMin() const { return greenMin_; }
  void setGreenMax(double r) { greenMax_ = std::min(std::max(r, 0.0), 1.0); }
  double greenMax() const { return greenMax_; }

  void setBlueMin(double r) { blueMin_ = std::min(std::max(r, 0.0), 1.0); }
  double blueMin() const { return blueMin_; }
  void setBlueMax(double r) { blueMax_ = std::min(std::max(r, 0.0), 1.0); }
  double blueMax() const { return blueMax_; }

  //---

  // defined colors
  int numColors() const { return colors_.size(); }

  const ColorMap &colors() const { return colors_; }

  void addDefinedColor(double v, const QColor &c);

  void resetDefinedColors();

  double mapDefinedColorX(double x) const;
  double unmapDefinedColorX(double x) const;

  // max defined colors : NOT USED yet
  int maxColors() const { return maxColors_; }
  void setMaxColors(int n) { maxColors_ = n; }

  //---

  QColor getColor(double x, bool scale=false) const;

  double interpModel(int ind, double x) const;

  //---

  static int numModels() { return 37; }

  static std::string modelName(int model);

  //---

  // functions
  const std::string &redFunction() const { return rf_.fn; }
  void setRedFunction(const std::string &fn);

  const std::string &greenFunction() const { return gf_.fn; }
  void setGreenFunction(const std::string &fn);

  const std::string &blueFunction() const { return bf_.fn; }
  void setBlueFunction(const std::string &fn);

  void setFunctions(const std::string &rf, const std::string &gf, const std::string &bf);

  //---

  // cube helix
  void setCubeHelix(double start, double cycles, double saturation);

  double cbStart() const { return cubeHelix_.start(); }
  void setCbStart(double r) { cubeHelix_.setStart(r); }

  double cbCycles() const { return cubeHelix_.cycles(); }
  void setCbCycles(double r) { cubeHelix_.setCycles(r); }

  double cbSaturation() const { return cubeHelix_.saturation(); }
  void setCbSaturation(double r) { cubeHelix_.setSaturation(r); }

  bool isCubeNegative() const { return cubeNegative_; }
  void setCubeNegative(bool b) { cubeNegative_ = b; }

  //---

  bool readFile(const std::string &filename);

  bool readFileLines(const std::vector<std::string> &lines);

  void unset();

  //void show(std::ostream &os) const;

  //void showGradient(std::ostream &os) const;
  //void showRGBFormulae(std::ostream &os) const;
  //void showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int);

  //---

  // util
  static double interpValue(double v1, double v2, double f) {
    return v1*(1 - f) + v2*f;
  }

  static QColor interpRGB(const QColor &c1, const QColor &c2, double f) {
    return QColor::fromRgbF(interpValue(c1.redF  (), c2.redF  (), f),
                            interpValue(c1.greenF(), c2.greenF(), f),
                            interpValue(c1.blueF (), c2.blueF (), f));
  }

  static QColor interpHSV(const QColor &c1, const QColor &c2, double f) {
    return QColor::fromHsvF(interpValue(c1.hueF       (), c2.hueF       (), f),
                            interpValue(c1.saturationF(), c2.saturationF(), f),
                            interpValue(c1.valueF     (), c2.valueF     (), f));
  }

 private:
  void init();

  void initFunctions();

 private:
  struct ColorFn {
    std::string     fn;
#ifdef CQCharts_USE_CEXPR
    CExprTokenStack stack;
#endif
  };

  QString    name_;

  // Color Calculation Type
  ColorType  colorType_     { ColorType::MODEL };

  // Color Model
  ColorModel colorModel_    { ColorModel::RGB };

#if defined(CQCharts_USE_TCL)
  ExprType   exprType_      { ExprType::TCL };
#elif defined(CQCharts_USE_CEXPR)
  ExprType   exprType_      { ExprType::CEXPR };
#else
  ExprType   exprType_      { ExprType::NONE };
#endif

  // Model
  int        rModel_        { 7 };
  int        gModel_        { 5 };
  int        bModel_        { 15 };
  bool       gray_          { false };
  bool       redNegative_   { false };
  bool       greenNegative_ { false };
  bool       blueNegative_  { false };
  double     redMin_        { 0.0 };
  double     redMax_        { 1.0 };
  double     greenMin_      { 0.0 };
  double     greenMax_      { 1.0 };
  double     blueMin_       { 0.0 };
  double     blueMax_       { 1.0 };

  // Functions
  ColorFn    rf_;
  ColorFn    gf_;
  ColorFn    bf_;

  // CubeHelix
  CCubeHelix cubeHelix_;
  bool       cubeNegative_  { false };

  // Defined
  ColorMap   colors_;
  int        maxColors_     { -1 };
  double     colorsMin_     { 0.0 };
  double     colorsMax_     { 0.0 };

  // Misc
#ifdef CQCharts_USE_CEXPR
  CExpr*     expr_          { nullptr };
#endif
#ifdef CQCharts_USE_TCL
  CQTcl*     qtcl_          { nullptr };
#endif
  double     gamma_         { 1.5 };
#if 0
  bool       psAllcF_       { false };
#endif
};

using CQChartsGradientPaletteP = std::unique_ptr<CQChartsGradientPalette>;

#endif
