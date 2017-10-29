#ifndef CGradientPalette_H
#define CGradientPalette_H

#ifdef CGRADIENT_EXPR
#include <CExpr.h>
#endif

#include <QColor>
#include <COSNaN.h>
#include <string>
#include <map>
#include <memory>

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

class CGradientPalette {
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

  typedef std::map<double,QColor> ColorMap;

 public:
#ifdef CGRADIENT_EXPR
  CGradientPalette(CExpr *expr);
#else
  CGradientPalette();
#endif

#ifdef CGRADIENT_EXPR
  CExpr *expr() const { return expr_; }
  void setExpr(CExpr *expr);
#endif

  ColorType colorType() const { return colorType_; }
  void setColorType(ColorType t) { colorType_ = t; }

  ColorModel colorModel() const { return colorModel_; }
  void setColorModel(ColorModel m) { colorModel_ = m; }

  double gamma() const { return gamma_; }
  void setGamma(double g) { gamma_ = g; }

  bool isGray() const { return gray_; }
  void setGray(bool b) { gray_ = b; }

  bool isCubeNegative() const { return cubeNegative_; }
  void setCubeNegative(bool b) { cubeNegative_ = b; }

  bool isRedNegative() const { return redNegative_; }
  void setRedNegative(bool b) { redNegative_ = b; }

  bool isGreenNegative() const { return greenNegative_; }
  void setGreenNegative(bool b) { greenNegative_ = b; }

  bool isBlueNegative() const { return blueNegative_; }
  void setBlueNegative(bool b) { blueNegative_ = b; }

  int maxColors() const { return maxColors_; }
  void setMaxColors(int n) { maxColors_ = n; }

  bool isPSAllCF() const { return psAllcF_; }
  void setPSAllCF(bool b) { psAllcF_ = b; }

  void setRgbModel(int r, int g, int b);

  int redModel() const { return rModel_; }
  void setRedModel(int r) { rModel_ = r; }

  int greenModel() const { return gModel_; }
  void setGreenModel(int r) { gModel_ = r; }

  int blueModel() const { return bModel_; }
  void setBlueModel(int r) { bModel_ = r; }

  //---

  int numColors() const { return colors_.size(); }

  const ColorMap &colors() const { return colors_; }

  void addDefinedColor(double v, const QColor &c);

  void resetDefinedColors();

  QColor getColor(double x) const;

  //---

  double interp(int ind, double x) const;

  //---

  static int numModels() { return 37; }

  static std::string modelName(int model);

  //---

  const std::string &redFunction  () const { return rf_.fn; }
  void setRedFunction(const std::string &fn);

  const std::string &greenFunction() const { return gf_.fn; }
  void setGreenFunction(const std::string &fn);

  const std::string &blueFunction () const { return bf_.fn; }
  void setBlueFunction(const std::string &fn);

  void setFunctions(const std::string &rf, const std::string &gf, const std::string &bf);

  //---

  void setCubeHelix(double start, double cycles, double saturation);

  double cbStart() const { return cubeHelix_.start(); }
  void setCbStart(double r) { cubeHelix_.setStart(r); }

  double cbCycles() const { return cubeHelix_.cycles(); }
  void setCbCycles(double r) { cubeHelix_.setCycles(r); }

  double cbSaturation() const { return cubeHelix_.saturation(); }
  void setCbSaturation(double r) { cubeHelix_.setSaturation(r); }

  //---

  bool readFile(const std::string &filename);

  bool readFileLines(const std::vector<std::string> &lines);

  void unset();

  void show(std::ostream &os) const;

  void showGradient(std::ostream &os) const;
  void showRGBFormulae(std::ostream &os) const;
  void showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int);

 private:
  struct ColorFn {
    std::string     fn;
#ifdef CGRADIENT_EXPR
    CExprTokenStack stack;
#endif
  };

  // State
  ColorType  colorType_  { ColorType::MODEL };

  // Model
  int        rModel_ { 7 };
  int        gModel_ { 5 };
  int        bModel_ { 15 };

  // Functions
  ColorFn    rf_;
  ColorFn    gf_;
  ColorFn    bf_;

  // CubeHelix
  CCubeHelix cubeHelix_;

  // Defined
  ColorMap   colors_;

  // Misc
#ifdef CGRADIENT_EXPR
  CExpr*     expr_          { nullptr };
#endif
  ColorModel colorModel_    { ColorModel::RGB };
  bool       redNegative_   { false };
  bool       greenNegative_ { false };
  bool       blueNegative_  { false };
  bool       cubeNegative_  { false };
  bool       gray_          { false };
  double     gamma_         { 1.5 };
  int        maxColors_     { -1 };
  bool       psAllcF_       { false };
};

typedef std::unique_ptr<CGradientPalette> CGradientPaletteP;

#endif
