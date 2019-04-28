#include <CQChartsGradientPalette.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>

#include <CQTclUtil.h>

#include <COSNaN.h>

namespace Util {
  double clamp(double val, double low, double high) {
    if (val < low ) return low;
    if (val > high) return high;
    return val;
  }

  double norm(double x, double low, double high) {
    return (x - low)/(high - low);
  }

  double lerp(double value1, double value2, double amt) {
    return value1 + (value2 - value1)*amt;
  }

  double map(double value, double low1, double high1, double low2, double high2) {
    return lerp(low2, high2, norm(value, low1, high1));
  }

  double Deg2Rad(double d) {
    return M_PI*d/180.0;
  }

  void addWords(const QString &str, std::vector<QString> &words) {
    QStringList strs = str.split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.length(); ++i)
      words.push_back(strs[i]);
  }
}

//------

CQChartsGradientPalette::
CQChartsGradientPalette()
{
  init();
}

void
CQChartsGradientPalette::
init()
{
  initFunctions();
}

CQChartsGradientPalette::
CQChartsGradientPalette(const CQChartsGradientPalette &palette)
{
  *this = palette;
}

CQChartsGradientPalette::
~CQChartsGradientPalette()
{
  delete qtcl_;
}

CQChartsGradientPalette &
CQChartsGradientPalette::
operator=(const CQChartsGradientPalette &palette)
{
  struct ColorFn {
    std::string fn;
  };

  colorType_  = palette.colorType_;
  colorModel_ = palette.colorModel_;

  rModel_        = palette.rModel_;
  gModel_        = palette.gModel_;
  bModel_        = palette.bModel_;
  gray_          = palette.gray_;
  redNegative_   = palette.redNegative_;
  greenNegative_ = palette.greenNegative_;
  blueNegative_  = palette.blueNegative_;
  redMin_        = palette.redMin_;
  redMax_        = palette.redMax_;
  greenMin_      = palette.greenMin_;
  greenMax_      = palette.greenMax_;
  blueMin_       = palette.blueMin_;
  blueMax_       = palette.blueMax_;

  rf_ = palette.rf_;
  gf_ = palette.gf_;
  bf_ = palette.bf_;

  cubeHelix_    = palette.cubeHelix_;
  cubeNegative_ = palette.cubeNegative_;

  colors_     = palette.colors_;
#if 0
  maxColors_  = palette.maxColors_;
#endif
  colorsMin_  = palette.colorsMin_;
  colorsMax_  = palette.colorsMax_;
  distinct_   = palette.distinct_;

#if 0
  gamma_= palette.gamma_;
#endif

  delete qtcl_;

  qtcl_ = nullptr;

  init();

  return *this;
}

//---

CQTcl *
CQChartsGradientPalette::
qtcl() const
{
  if (! qtcl_) {
    CQChartsGradientPalette *th = const_cast<CQChartsGradientPalette *>(this);

    th->qtcl_ = new CQTcl;

    th->qtcl_->createVar("pi", M_PI);
  }

  return qtcl_;
}

//---

void
CQChartsGradientPalette::
initFunctions()
{
  setRedFunction  ("$gray");
  setGreenFunction("$gray");
  setBlueFunction ("$gray");
}

//---

void
CQChartsGradientPalette::
setRedFunction(const std::string &fn)
{
  rf_.fn = fn;
}

void
CQChartsGradientPalette::
setGreenFunction(const std::string &fn)
{
  gf_.fn = fn;
}

void
CQChartsGradientPalette::
setBlueFunction(const std::string &fn)
{
  bf_.fn = fn;
}

void
CQChartsGradientPalette::
setFunctions(const std::string &rf, const std::string &gf, const std::string &bf)
{
  colorType_ = ColorType::FUNCTIONS;

  setRedFunction  (rf);
  setGreenFunction(gf);
  setBlueFunction (bf);
}

void
CQChartsGradientPalette::
setCubeHelix(double start, double cycles, double saturation)
{
  colorType_ = ColorType::CUBEHELIX;

  setCbStart     (start);
  setCbCycles    (cycles);
  setCbSaturation(saturation);
}

void
CQChartsGradientPalette::
setRgbModel(int r, int g, int b)
{
  colorType_ = ColorType::MODEL;

  rModel_ = r;
  gModel_ = g;
  bModel_ = b;
}

void
CQChartsGradientPalette::
addDefinedColor(double v, const QColor &c)
{
  colors_[v] = c;

  colorsMin_ = colors(). begin()->first;
  colorsMax_ = colors().rbegin()->first;
}

void
CQChartsGradientPalette::
resetDefinedColors()
{
  colors_.clear();

  colorsMin_ = 0.0;
  colorsMax_ = 0.0;
}

double
CQChartsGradientPalette::
mapDefinedColorX(double x) const
{
  double d = colorsMax_ - colorsMin_;

  return (d > 0.0 ? (x - colorsMin_)/d : x);
}

double
CQChartsGradientPalette::
unmapDefinedColorX(double x) const
{
  double d = colorsMax_ - colorsMin_;

  return x*d + colorsMin_;
}

QColor
CQChartsGradientPalette::
getColor(double x, bool scale) const
{
  if      (colorType() == ColorType::DEFINED) {
    if (colors().empty()) {
      QColor c1(0,0,0);
      QColor c2(255,255,255);

      if      (colorModel() == ColorModel::RGB)
        return interpRGB(c1, c2, x);
      else if (colorModel() == ColorModel::HSV)
        return interpHSV(c1, c2, x);
      else
        return interpRGB(c1, c2, x);
    }

    if (scale)
      x = mapDefinedColorX(x);

    auto p = colors().begin();

    double x1 = mapDefinedColorX((*p).first);
    QColor c1 = (*p).second;

    if (x <= x1) return c1;

    for (++p; p != colors().end(); ++p) {
      double x2 = mapDefinedColorX((*p).first);
      QColor c2 = (*p).second;

      if (x <= x2) {
        double m = (x - x1)/(x2 - x1);

        if      (colorModel() == ColorModel::RGB)
          return interpRGB(c1, c2, m);
        else if (colorModel() == ColorModel::HSV)
          return interpHSV(c1, c2, m);
        else
          return interpRGB(c1, c2, m);
      }

      x1 = x2;
      c1 = c2;
    }

    return c1;
  }
  else if (colorType() == ColorType::MODEL) {
    if (isGray()) {
      double g = Util::clamp(x, 0.0, 1.0);

      if (isRedNegative() || isGreenNegative() || isBlueNegative())
        g = 1.0 - g;

      return QColor(255*g, 255*g, 255*g);
    }

    //---

    double x1 = Util::clamp(x, 0.0, 1.0);

    double r = Util::clamp(interpModel(redModel  (), x1), 0.0, 1.0);
    double g = Util::clamp(interpModel(greenModel(), x1), 0.0, 1.0);
    double b = Util::clamp(interpModel(blueModel (), x1), 0.0, 1.0);

    if (isRedNegative  ()) r = 1.0 - r;
    if (isGreenNegative()) g = 1.0 - g;
    if (isBlueNegative ()) b = 1.0 - b;

    r = Util::map(r, 0.0, 1.0, redMin  (), redMax  ());
    g = Util::map(g, 0.0, 1.0, greenMin(), greenMax());
    b = Util::map(b, 0.0, 1.0, blueMin (), blueMax ());

    QColor c;

    if      (colorModel() == ColorModel::RGB)
      c = QColor::fromRgbF(r, g, b);
    else if (colorModel() == ColorModel::HSV)
      c = QColor::fromHsvF(r, g, b);
    else
      c = QColor::fromRgbF(r, g, b);

    return c;
  }
  else if (colorType() == ColorType::FUNCTIONS) {
    double r = 0.0, g = 0.0, b = 0.0;

    CQTcl *qtcl = this->qtcl();

    qtcl->createVar("gray", x);

    QVariant res;
    bool     ok;

    if (qtcl->evalExpr(rf_.fn.c_str(), res))
      r = CQChartsVariant::toReal(res, ok);

    if (qtcl->evalExpr(gf_.fn.c_str(), res))
      g = CQChartsVariant::toReal(res, ok);

    if (qtcl->evalExpr(bf_.fn.c_str(), res))
      b = CQChartsVariant::toReal(res, ok);

    //---

    QColor c;

    r = Util::clamp(r, 0, 1);
    g = Util::clamp(g, 0, 1);
    b = Util::clamp(b, 0, 1);

    if      (colorModel() == ColorModel::RGB)
      c = QColor::fromRgbF(r, g, b);
    else if (colorModel() == ColorModel::HSV)
      c = QColor::fromHsvF(r, g, b);
    else
      c = QColor::fromRgbF(r, g, b);

    return c;
  }
  else if (colorType() == ColorType::CUBEHELIX) {
    return QColor(cubeHelix_.interp(x, isCubeNegative()));
  }
  else {
    return QColor(0, 0, 0);
  }
}

double
CQChartsGradientPalette::
interpModel(int ind, double x) const
{
  if (ind < 0)
    return interpModel(-ind, 1 - x);

  switch (ind) {
    case  0: return 0;
    case  1: return 0.5;
    case  2: return 1;
    case  3: return x;
    case  4: return pow(x, 2);
    case  5: return pow(x, 3);
    case  6: return pow(x, 4);
    case  7: return sqrt(x);
    case  8: return sqrt(sqrt(x));
    case  9: return sin(Util::Deg2Rad(90*x));
    case 10: return cos(Util::Deg2Rad(90*x));
    case 11: return fabs(x - 0.5);
    case 12: return pow(2*x - 1, 2);
    case 13: return sin(Util::Deg2Rad(180*x));
    case 14: return fabs(cos(Util::Deg2Rad(180*x)));
    case 15: return sin(Util::Deg2Rad(360*x));
    case 16: return cos(Util::Deg2Rad(360*x));
    case 17: return fabs(sin(Util::Deg2Rad(360*x)));
    case 18: return fabs(cos(Util::Deg2Rad(360*x)));
    case 19: return fabs(sin(Util::Deg2Rad(720*x)));
    case 20: return fabs(cos(Util::Deg2Rad(720*x)));
    case 21: return 3*x;
    case 22: return 3*x - 1;
    case 23: return 3*x - 2;
    case 24: return fabs(3*x - 1);
    case 25: return fabs(3*x - 2);
    case 26: return (3*x - 1)/2;
    case 27: return (3*x - 2)/2;
    case 28: return fabs((3*x - 1)/2);
    case 29: return fabs((3*x - 2)/2);
    case 30: return x/0.32 - 0.78125;
    case 31: return 2*x - 0.84;
    case 32: {
      if      (x < 0.00) return 0;
      else if (x < 0.25) return 4*x;
      else if (x < 0.42) return 1;
      else if (x < 0.92) return -2*x + 1.84;
      else if (x < 1.00) return x/0.08 - 11.5;
      else               return 1;
    }
    case 33: return fabs(2*x - 0.5);
    case 34: return 2*x;
    case 35: return 2*x - 0.5;
    case 36: return 2*x - 1;
    default: return x;
  }
}

std::string
CQChartsGradientPalette::
modelName(int ind)
{
  if (ind < 0)
    return "Inverted " + modelName(-ind);

  switch (ind) {
    case  0: return "0";
    case  1: return "0.5";
    case  2: return "1";
    case  3: return "x";
    case  4: return "x\u00B2";
    case  5: return "x\u00B3";
    case  6: return "x\u2074";
    case  7: return "\u221Ax";
    case  8: return "\u221A\u221Ax";
    case  9: return "sin(90*x)";
    case 10: return "cos(90*x)";
    case 11: return "\u007Cx - 0.5\u007C";
    case 12: return "(2*x - 1)\u00B2";
    case 13: return "sin(180*x)";
    case 14: return "\u007Ccos(180*x)\u007C";
    case 15: return "sin(360*x)";
    case 16: return "cos(360*x)";
    case 17: return "\u007Csin(360*x)\u007C";
    case 18: return "\u007Ccos(360*x)\u007C";
    case 19: return "\u007Csin(720*x)\u007C";
    case 20: return "\u007Ccos(720*x)\u007C";
    case 21: return "3*x";
    case 22: return "3*x - 1";
    case 23: return "3*x - 2";
    case 24: return "\u007C3*x - 1\u007C";
    case 25: return "\u007C3*x - 2\u007C";
    case 26: return "(3*x - 1)/2";
    case 27: return "(3*x - 2)/2";
    case 28: return "\u007C(3*x - 1)/2\u007C";
    case 29: return "\u007C(3*x - 2)/2\u007C";
    case 30: return "x/0.32 - 0.78125";
    case 31: return "2*x - 0.84";
    case 32: return "0,4*x,1,-2*x + 1.84,x/0.08 - 11.5,1";
    case 33: return "\u007C2*x - 0.5\u007C";
    case 34: return "2*x";
    case 35: return "2*x - 0.5";
    case 36: return "2*x - 1";
    default: return "x";
  }
}

bool
CQChartsGradientPalette::
readFile(const std::string &filename)
{
  QStringList lines;

  if (! CQChartsUtil::fileToLines(filename.c_str(), lines)) {
    std::cerr << "can't read file \"" << filename << "\"" << std::endl;
    return false;
  }

  if (! readFileLines(lines))
    return false;

  return true;
}

bool
CQChartsGradientPalette::
readFileLines(const QStringList &lines)
{
  setColorType(CQChartsGradientPalette::ColorType::DEFINED);

  resetDefinedColors();

  int i = 0;

  for (const auto &line : lines) {
    if (line.length() == 0) continue;

    std::vector<QString> words;

    Util::addWords(line, words);

    double x = i;

    int j = 0;

    if (words.size() >= 4) {
      bool ok;

      x = CQChartsUtil::toReal(words[0], ok);

      ++j;
    }

    double r, g, b;

    if (words.size() >= 3) {
      bool ok;

      r = CQChartsUtil::toReal(words[j + 0], ok);
      g = CQChartsUtil::toReal(words[j + 1], ok);
      b = CQChartsUtil::toReal(words[j + 2], ok);
    }
    else
      continue;

    addDefinedColor(x, QColor(255*r, 255*g, 255*b));

    ++i;
  }

  return true;
}

void
CQChartsGradientPalette::
unset()
{
  colorType_  = ColorType::MODEL;
  colorModel_ = ColorModel::RGB;

  // Model
  rModel_        = 7;
  gModel_        = 5;
  bModel_        = 15;
  gray_          = false;
  redNegative_   = false;
  greenNegative_ = false;
  blueNegative_  = false;

  // Defined
  resetDefinedColors();

  // Functions
  initFunctions();

  // CubeHelix
  cubeHelix_.reset();

  // Misc
#if 0
  maxColors_ = -1;
#endif

#if 0
  gamma_ = 1.5;
#endif
}

void
CQChartsGradientPalette::
setLinearGradient(QLinearGradient &lg, double a) const
{
  int    n = 64;
  double d = 1.0/n;

  double x = 0.0;

  for (int i = 0; i < n; ++i) {
    QColor c = getColor(x);

    c.setAlphaF(a);

    lg.setColorAt(x, c);

    x += d;
  }
}

#if 0
void
CQChartsGradientPalette::
show(std::ostream &os) const
{
  os << "palette is " << (isGray() ? "GRAY" : "COLOR") << std::endl;

  if (! isGray()) {
    if      (colorType() == ColorType::MODEL)
      os << "rgb color mapping by rgbformulae are " <<
            redModel() << "," << greenModel() << "," << blueModel() << std::endl;
    else if (colorType() == ColorType::DEFINED)
      os << "color mapping by defined gradient" << std::endl;
    else if (colorType() == ColorType::FUNCTIONS)
      os << "color mapping is done by user defined functions " <<
            "A-formula: " << redFunction() <<
            "B-formula: " << greenFunction() <<
            "C-formula: " << blueFunction() << std::endl;
    else if (colorType() == ColorType::CUBEHELIX)
      os << "Cubehelix color palette: start " << cubeHelix_.start() <<
            " cycles " << cubeHelix_.cycles() <<
            " saturation " << cubeHelix_.saturation() << std::endl;
  }

  if      (colorType() == ColorType::MODEL)
    os << "figure is " << ((isRedNegative() || isGreenNegative() || isBlueNegative()) ?
                            "NEGATIVE" : "POSITIVE") << std::endl;
  else if (colorType() == ColorType::CUBEHELIX)
    os << "figure is " << (isCubeNegative() ? "NEGATIVE" : "POSITIVE") << std::endl;

#if 0
  if (maxColors() <= 0)
    os << "allocating ALL remaining";
  else
    os << "allocating MAX " << maxColors();
#endif

  os << " color positions for discrete palette terminals" << std::endl;

  os << "Color-Model: ";
  if      (colorModel() == ColorModel::RGB) os << "RGB";
  else if (colorModel() == ColorModel::HSV) os << "HSV";
  else if (colorModel() == ColorModel::CMY) os << "CMY";
  else if (colorModel() == ColorModel::YIQ) os << "YIQ";
  else if (colorModel() == ColorModel::XYZ) os << "XYZ";
  os << std::endl;

#if 0
  os << "gamma is " << gamma_ << std::endl;
#endif
}

void
CQChartsGradientPalette::
showGradient(std::ostream &os) const
{
  if (colorType() != ColorType::DEFINED) {
    int i = 0;

    for (const auto &cc : colors()) {
      const QColor &c = cc.second;

      os << i << ". gray=" << cc.first << ", ";

      if      (c.spec() == QColor::Rgb) {
        os << "(r,g,b)=(" << c.redF() << "," << c.greenF() << "," << c.blueF() <<
              "), " << c.name().toStdString() << " = " <<
              c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
      else if (c.spec() == QColor::Hsv) {
        // TODO
        os << "(h,s,v)=(...)";
      }

      ++i;
    }
  }
  else
    os << "color mapping *not* done by defined gradient." << std::endl;
}

void
CQChartsGradientPalette::
showRGBFormulae(std::ostream &os) const
{
  os << "set palette rgbformulae " <<
        redModel() << "," << greenModel() << "," << blueModel() << std::endl;
}

void
CQChartsGradientPalette::
showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int)
{
  if (n <= 1) n = 128;

  os << "Color palette with " << n << " discrete colors." << std::endl;

  double x = 0.0;
  double d = 1.0/(n - 1);

  for (int i = 0; i < n; ++i, x += d) {
    QColor c = getColor(x);

    os << "  ";

    if      (c.spec() == QColor::Rgb) {
      if      (is_float) {
        os << c.redF() << " " << c.greenF() << " " << c.blueF() << std::endl;
      }
      else if (is_int) {
        os << c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
      else {
        os << i << ". gray=" << x << ", (r,g,b)=(" <<
              c.redF() << "," << c.greenF() << "," << c.blueF() <<
              "), " << c.name().toStdString() << " = " <<
              c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
    }
    else if (c.spec() == QColor::Hsv) {
      // TODO
    }
  }
}
#endif
