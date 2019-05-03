#ifndef CQChartsGradientPalette_H
#define CQChartsGradientPalette_H

#include <QColor>
#include <QStringList>
#include <COSNaN.h>
#include <string>
#include <map>
#include <memory>
#include <cmath>

class CQTcl;

class QLinearGradient;

//---

//! \brief color helix gradient
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

//! \brief gradient palette
class CQChartsGradientPalette {
 public:
  enum class ColorType {
    NONE,
    MODEL,
    DEFINED,
    FUNCTIONS,
    CUBEHELIX
  };

  enum class ColorModel {
    NONE,
    RGB,
    HSV,
    CMY,
    YIQ,
    XYZ,
  };

  using ColorMap = std::map<double,QColor>;

 public:
  static ColorType stringToColorType(const QString &str) {
    if      (str == "model"    ) return ColorType::MODEL;
    else if (str == "defined"  ) return ColorType::DEFINED;
    else if (str == "functions") return ColorType::FUNCTIONS;
    else if (str == "cubehelix") return ColorType::CUBEHELIX;

    return ColorType::NONE;
  }

  static QString colorTypeToString(ColorType type) {
    switch (type) {
      case ColorType::MODEL    : return "model";
      case ColorType::DEFINED  : return "defined";
      case ColorType::FUNCTIONS: return "functions";
      case ColorType::CUBEHELIX: return "cubehelix";
      default                  : return "";
    }
  }

  static ColorModel stringToColorModel(const QString &str) {
    if      (str == "rgb") return ColorModel::RGB;
    else if (str == "hsv") return ColorModel::HSV;
    else if (str == "cmy") return ColorModel::CMY;
    else if (str == "yiq") return ColorModel::YIQ;
    else if (str == "xyz") return ColorModel::XYZ;

    return ColorModel::NONE;
  }

  static QString colorModelToString(ColorModel model) {
    switch (model) {
      case ColorModel::RGB: return "rgb";
      case ColorModel::HSV: return "hsv";
      case ColorModel::CMY: return "cmy";
      case ColorModel::YIQ: return "yiq";
      case ColorModel::XYZ: return "xyz";
      default             : return "";
    }
  }

 public:
  CQChartsGradientPalette();

  CQChartsGradientPalette(const CQChartsGradientPalette &palette);

  virtual ~CQChartsGradientPalette();

  CQChartsGradientPalette &operator=(const CQChartsGradientPalette &palette);

  //---

  CQChartsGradientPalette *dup() const { return new CQChartsGradientPalette(*this); }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QString &desc() const { return desc_; }
  void setDesc(const QString &s) { desc_ = s; }

  //---

  //! get/set color calculation type
  ColorType colorType() const { return colorType_; }
  void setColorType(ColorType t) { colorType_ = t; }

  //! get/set color model
  ColorModel colorModel() const { return colorModel_; }
  void setColorModel(ColorModel m) { colorModel_ = m; }

  //---

#if 0
  //! get/set gamma correction : NOT USED yet
  double gamma() const { return gamma_; }
  void setGamma(double g) { gamma_ = g; }
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

  QColor icolor(int i) const {
    int i1 = 0;

    for (const auto &rc : colors_) {
      if (i1 == i)
        return rc.second;

      ++i1;
    }

    return QColor();
  }

  ColorMap &colors() { return colors_; }

  void setColors(const ColorMap &colors) { colors_ = colors; }

  void addDefinedColor(double v, const QColor &c);

  void resetDefinedColors();

  // map/unmap defined x (in range 0.0->1.0) to/from min/max
  double mapDefinedColorX(double x) const;
  double unmapDefinedColorX(double x) const;

#if 0
  // max defined colors : NOT USED yet
  int maxColors() const { return maxColors_; }
  void setMaxColors(int n) { maxColors_ = n; }
#endif

  bool isDistinct() const { return distinct_; }
  void setDistinct(bool b) { distinct_ = b; }

  //---

  //! interpolate color at x (if scaled then input x has been adjusted to min/max range)
  QColor getColor(double x, bool scale=false) const;

 private:
  //! interpolate color for model ind and x value
  double interpModel(int ind, double x) const;

  //---

 public:
  // color models
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

  //! read file containing defined colors
  bool readFile(const std::string &filename);

 private:
  bool readFileLines(const QStringList &lines);

  //---

 public:
  //! unset palette
  void unset();

  //---

  //! set linear gradient
  void setLinearGradient(QLinearGradient &lg, double a=1.0) const;

  //---

  //void show(std::ostream &os) const;

  //void showGradient(std::ostream &os) const;
  //void showRGBFormulae(std::ostream &os) const;
  //void showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int);

  //---

  // util
 private:
  static double interpValue(double v1, double v2, double f) {
    return v1*(1 - f) + v2*f;
  }

 public:
  //! interpolate between two RGB colors
  static QColor interpRGB(const QColor &c1, const QColor &c2, double f) {
    return QColor::fromRgbF(interpValue(c1.redF  (), c2.redF  (), f),
                            interpValue(c1.greenF(), c2.greenF(), f),
                            interpValue(c1.blueF (), c2.blueF (), f));
  }

  //! interpolate between two HSV colors
  static QColor interpHSV(const QColor &c1, const QColor &c2, double f) {
    return QColor::fromHsvF(interpValue(c1.hueF       (), c2.hueF       (), f),
                            interpValue(c1.saturationF(), c2.saturationF(), f),
                            interpValue(c1.valueF     (), c2.valueF     (), f));
  }

 protected:
  void init();

  void initFunctions();

  CQTcl *qtcl() const;

 protected:
  struct ColorFn {
    std::string fn;
  };

  QString    name_;                               //!< name
  QString    desc_;                               //!< description

  // Color Calculation Type
  ColorType  colorType_     { ColorType::MODEL }; //!< color type

  // Color Model
  ColorModel colorModel_    { ColorModel::RGB };  //!< color model

  // Model
  int        rModel_        { 7 };                //!< red model number
  int        gModel_        { 5 };                //!< green model number
  int        bModel_        { 15 };               //!< blue model number
  bool       gray_          { false };            //!< is gray
  bool       redNegative_   { false };            //!< is red negated
  bool       greenNegative_ { false };            //!< is green negated
  bool       blueNegative_  { false };            //!< is blue negated
  double     redMin_        { 0.0 };              //!< red minimum
  double     redMax_        { 1.0 };              //!< red maximum
  double     greenMin_      { 0.0 };              //!< green minimum
  double     greenMax_      { 1.0 };              //!< green maximum
  double     blueMin_       { 0.0 };              //!< blue minimum
  double     blueMax_       { 1.0 };              //!< blue maximum

  // Functions
  ColorFn    rf_;                                 //!< red color tcl function
  ColorFn    gf_;                                 //!< green color tcl function
  ColorFn    bf_;                                 //!< blue color tcl function

  // CubeHelix
  CCubeHelix cubeHelix_;                          //!< cube helix data
  bool       cubeNegative_  { false };            //!< is cube helix negated

  // Defined
  ColorMap   colors_;                             //!< list of defined colors
#if 0
  int        maxColors_     { -1 };               //!< maximum number of colors
#endif
  double     colorsMin_     { 0.0 };              //!< colors min value (for scaling)
  double     colorsMax_     { 0.0 };              //!< colors max value (for scaling)
  bool       distinct_      { false };            //!< use distinct colors

  // Misc
#if 0
  double     gamma_         { 1.5 };              //!< gamma value
#endif

  // Tcl
  CQTcl*     qtcl_          { nullptr };          //!< qtcl pointer
};

using CQChartsGradientPaletteP = std::unique_ptr<CQChartsGradientPalette>;

#endif
