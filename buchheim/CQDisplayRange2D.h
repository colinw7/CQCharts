#ifndef CQDisplayRange2D_H
#define CQDisplayRange2D_H

#include <CMathRound.h>
#include <cmath>

// Class to represent a 2D mapping from window to pixel coordinates
class CQDisplayRange2D {
 public:
  enum class HAlign {
    NONE,
    LEFT,
    CENTER,
    RIGHT,
    JUSTIFY,
    INSIDE,
    OUTSIDE
  };

  enum class VAlign {
    NONE,
    TOP,
    CENTER,
    BOTTOM,
    BASELINE
  };

  using Point  = QPointF;
  using BBox   = QRectF;
//using Matrix = QMatrix;

 public:
  struct Range {
   double xmin, ymin, xmax, ymax;

   Range(double xmin1=0.0, double ymin1=0.0, double xmax1=0.0, double ymax1=0.0) :
    xmin(xmin1), ymin(ymin1), xmax(xmax1), ymax(ymax1) {
   }

   void set(double xmin1, double ymin1, double xmax1, double ymax1) {
     xmin = xmin1; ymin = ymin1; xmax = xmax1; ymax = ymax1;
   }

   void get(double *xmin1, double *ymin1, double *xmax1, double *ymax1) const {
     *xmin1 = xmin; *ymin1 = ymin; *xmax1 = xmax; *ymax1 = ymax;
   }

   double dx() const { return xmax - xmin; }
   double dy() const { return ymax - ymin; }

   double xmid() const { return (xmin + xmax)/2 ; }
   double ymid() const { return (ymin + ymax)/2; }

   void incX(double dx) { xmin += dx; xmax += dx; }
   void incY(double dy) { ymin += dy; ymax += dy; }
  };

 public:
  CQDisplayRange2D(double pixel_xmin  =   0, double pixel_ymin  =   0,
                   double pixel_xmax  = 100, double pixel_ymax  = 100,
                   double window_xmin = 0.0, double window_ymin = 0.0,
                   double window_xmax = 1.0, double window_ymax = 1.0) :
   pixel_ (pixel_xmin , pixel_ymin , pixel_xmax , pixel_ymax ),
   window_(window_xmin, window_ymin, window_xmax, window_ymax), window1_(window_) {
    reset();
  }

  void setPixelRange(double pixel_xmin, double pixel_ymin, double pixel_xmax, double pixel_ymax) {
    pixel_.set(pixel_xmin, pixel_ymin, pixel_xmax, pixel_ymax);

    reset();
  }

  void setWindowRange(double window_xmin, double window_ymin,
                      double window_xmax, double window_ymax) {
    window_.set(window_xmin, window_ymin, window_xmax, window_ymax);

    reset();
  }

  void getPixelRange(double *pixel_xmin, double *pixel_ymin,
                     double *pixel_xmax, double *pixel_ymax) const {
    pixel_.get(pixel_xmin, pixel_ymin, pixel_xmax, pixel_ymax);
  }

  void getPixelRange(int *pixel_xmin, int *pixel_ymin, int *pixel_xmax, int *pixel_ymax) const {
    double pixel_xmin1, pixel_ymin1, pixel_xmax1, pixel_ymax1;

    pixel_.get(&pixel_xmin1, &pixel_ymin1, &pixel_xmax1, &pixel_ymax1);

    *pixel_xmin = pixel_xmin1;
    *pixel_ymin = pixel_ymin1;
    *pixel_xmax = pixel_xmax1;
    *pixel_ymax = pixel_ymax1;
  }

  double getPixelWidth () const { return (pixel_.dx() >= 0 ? pixel_.dx() + 1 : pixel_.dx() - 1); }
  double getPixelHeight() const { return (pixel_.dy() >= 0 ? pixel_.dy() + 1 : pixel_.dy() - 1); }

  void getWindowRange(double *window_xmin, double *window_ymin,
                      double *window_xmax, double *window_ymax) const {
    window_.get(window_xmin, window_ymin, window_xmax, window_ymax);
  }

  void getWindowRange(BBox &bbox) const {
    double window_xmin, window_ymin, window_xmax, window_ymax;

    getWindowRange(&window_xmin, &window_ymin, &window_xmax, &window_ymax);

    bbox = BBox(Point(window_xmin, window_ymin), Point(window_xmax, window_ymax));
  }

  double getWindowWidth () const { return window_.dx(); }
  double getWindowHeight() const { return window_.dy(); }

  Point getWindowCenter() const { return Point(window_.xmid(), window_.ymid()); }

  void getTransformedWindowRange(double *window_xmin, double *window_ymin,
                                 double *window_xmax, double *window_ymax) const {
    window1_.get(window_xmin, window_ymin, window_xmax, window_ymax);
  }

  // get/set equal scale flag
  bool getEqualScale() const { return equal_scale_; }
  void setEqualScale(bool flag) { equal_scale_ = flag; recalc(); }

  bool getScaleMin() const { return scale_min_; }
  void setScaleMin(bool flag) { scale_min_ = flag; recalc(); }

  HAlign getHAlign() const { return halign_; }
  void setHAlign(HAlign halign) { halign_ = halign; recalc(); }

  VAlign getVAlign() const { return valign_; }
  void setVAlign(VAlign valign) { valign_ = valign; recalc(); }

  void setAlign(HAlign halign, VAlign valign) {
    halign_ = halign; valign_ = valign;
    recalc();
  }

  bool getFlipX() const { return flip_x_; }
  bool getFlipY() const { return flip_y_; }

  void zoomIn(double factor=2.0) {
    assert(factor > 0.0);

    zoomOut(1.0/factor);
  }

  void zoomOut(double factor=2.0) {
    double window_hwidth  = 0.5*window_width1_ *factor;
    double window_hheight = 0.5*window_height1_*factor;

    zoomTo(window_xc1_ - window_hwidth, window_yc1_ - window_hheight,
           window_xc1_ + window_hwidth, window_yc1_ + window_hheight);
  }

  void zoomTo(double window_xmin, double window_ymin, double window_xmax, double window_ymax) {
    window1_.set(window_xmin, window_ymin, window_xmax, window_ymax);

    recalc();
  }

  void scroll(double offset_x, double offset_y) {
    scrollX(offset_x);
    scrollY(offset_y);
  }

  void scrollX(double offset_x) { window1_.incX(offset_x); recalc(); }
  void scrollY(double offset_y) { window1_.incY(offset_y); recalc(); }

  void reset() { window1_ = window_; recalc(); }

  void recalc() {
    window_xc1_ = window1_.xmid();
    window_yc1_ = window1_.ymid();

    window_width1_  = window1_.dx();
    window_height1_ = window1_.dy();

    factor_x_ = (window_width1_  != 0 ?  getPixelWidth ()/window_width1_  : 1.0);
    factor_y_ = (window_height1_ != 0 ? -getPixelHeight()/window_height1_ : 1.0);

    if (getEqualScale()) {
      factor_x1_ = factor_x_;
      factor_y1_ = factor_y_;

      if (getScaleMin()) {
        if (fabs(factor_x1_) > fabs(factor_y1_)) {
          if (factor_x1_ < 0)
            factor_x1_ = -fabs(factor_y1_);
          else
            factor_x1_ =  fabs(factor_y1_);
        }
        else {
          if (factor_y1_ < 0)
            factor_y1_ = -fabs(factor_x1_);
          else
            factor_y1_ =  fabs(factor_x1_);
        }
      }
      else {
        if (fabs(factor_x1_) < fabs(factor_y1_)) {
          if (factor_x1_ < 0)
            factor_x1_ = -fabs(factor_y1_);
          else
            factor_x1_ =  fabs(factor_y1_);
        }
        else {
          if (factor_y1_ < 0)
            factor_y1_ = -fabs(factor_x1_);
          else
            factor_y1_ =  fabs(factor_x1_);
        }
      }

      pdx_ = 0;
      pdy_ = 0;

      double px = (window1_.xmax - window1_.xmin)*factor_x1_ + pixel_.xmin;
      double py = (window1_.ymax - window1_.ymin)*factor_y1_ + pixel_.ymax;

      //windowToPixel(window1_.xmax, window1_.ymin, &px, &py);

      double extra_width  = pixel_.xmax - px;
      double extra_height = py;

      if      (halign_ == HAlign::LEFT)
        pdx_ = 0;
      else if (halign_ == HAlign::CENTER)
        pdx_ = extra_width/2.0;
      else if (halign_ == HAlign::RIGHT)
        pdx_ = extra_width;

      if      (valign_ == VAlign::TOP)
        pdy_ = 0;
      else if (valign_ == VAlign::CENTER)
        pdy_ = -extra_height/2.0;
      else if (valign_ == VAlign::BOTTOM)
        pdy_ = -extra_height;
    }
    else {
      pdx_ = 0;
      pdy_ = 0;
    }

    flip_x_ = ((pixel_ .xmax - pixel_ .xmin)*(window_.xmax - window_.xmin) < 0);
    flip_y_ = ((pixel_ .ymax - pixel_ .ymin)*(window_.ymax - window_.ymin) < 0);

    //------

#if 0
    Matrix matrix1, matrix2, matrix3;

    if (getEqualScale()) {
      matrix1.setTranslation(pixel_.xmin + pdx_, pixel_.ymin + pdy_);
      matrix2.setScale      (factor_x1_, factor_y1_);
      matrix3.setTranslation(-window1_.xmin, -window1_.ymax);
    }
    else {
      matrix1.setTranslation(pixel_.xmin, pixel_.ymin);
      matrix2.setScale      (factor_x_, factor_y_);
      matrix3.setTranslation(-window1_.xmin, -window1_.ymax);
    }

    matrix_ = matrix1*matrix2*matrix3;

    // TODO: check invertable
    imatrix_ = matrix_.inverse();
#endif
  }

  void windowToPixel(const Point &window, Point &pixel) const {
    double x, y;
    windowToPixel(window.x(), window.y(), &x, &y);
    pixel = Point(x, y);
  }

  void windowToPixel(double window_x, double window_y, double *pixel_x, double *pixel_y) const {
    if (getEqualScale()) {
      *pixel_x = (window_x - window1_.xmin)*factor_x1_ + pixel_.xmin + pdx_;
      *pixel_y = (window_y - window1_.ymin)*factor_y1_ + pixel_.ymax + pdy_;
    }
    else {
      *pixel_x = (window_x - window1_.xmin)*factor_x_  + pixel_.xmin;
      *pixel_y = (window_y - window1_.ymin)*factor_y_  + pixel_.ymax;
    }
  }

  void windowToPixel(double window_x, double window_y, int *pixel_x, int *pixel_y) const {
    double pixel_x1, pixel_y1;

    windowToPixel(window_x, window_y, &pixel_x1, &pixel_y1);

    *pixel_x = CMathRound::Round(pixel_x1);
    *pixel_y = CMathRound::Round(pixel_y1);
  }

  void pixelToWindow(const Point &pixel, Point &window) const {
    double x, y;
    pixelToWindow(pixel.x(), pixel.y(), &x, &y);
    window = Point(x, y);
  }

  void pixelToWindow(double pixel_x, double pixel_y, double *window_x, double *window_y) const {
    if (getEqualScale()) {
      if (factor_x1_ != 0.0)
        *window_x = (pixel_x - pixel_.xmin - pdx_)/factor_x1_ + window1_.xmin;
      else
        *window_x = window1_.xmin;

      if (factor_y1_ != 0.0)
        *window_y = (pixel_y - pixel_.ymax - pdy_)/factor_y1_ + window1_.ymin;
      else
       *window_y = window1_.ymin;
    }
    else {
      if (factor_x_ != 0.0)
        *window_x = (pixel_x - pixel_.xmin)/factor_x_  + window1_.xmin;
      else
        *window_x = window1_.xmin;

      if (factor_y_ != 0.0)
        *window_y = (pixel_y - pixel_.ymax)/factor_y_  + window1_.ymin;
      else
       *window_y = window1_.ymin;
    }
  }

  void pixelToWindow(int pixel_x, int pixel_y, double *window_x, double *window_y) const {
    pixelToWindow(double(pixel_x), double(pixel_y), window_x, window_y);
  }

  void pixelLengthToWindowLength(double pixel, double *window) const {
    pixelWidthToWindowWidth(pixel, window);
  }

  void pixelWidthToWindowWidth(double pixel, double *window) const {
    if (pixel < 0) {
      pixelWidthToWindowWidth(-pixel, window);
      *window = -(*window);
      return;
    }

    double window_x1, window_y1;
    double window_x2, window_y2;

    pixelToWindow(0    , 0    , &window_x1, &window_y1);
    pixelToWindow(pixel, pixel, &window_x2, &window_y2);

    *window = fabs(window_x2 - window_x1);
  }

  void pixelHeightToWindowHeight(double pixel, double *window) const {
    if (pixel < 0) {
      pixelHeightToWindowHeight(-pixel, window);
      *window = -(*window);
      return;
    }

    double window_x1, window_y1;
    double window_x2, window_y2;

    pixelToWindow(0    , 0    , &window_x1, &window_y1);
    pixelToWindow(pixel, pixel, &window_x2, &window_y2);

    *window = fabs(window_y2 - window_y1);
  }

  bool checkPixel(double x, double y) {
    return (x >= pixel_.xmin && x <= pixel_.xmin && y >= pixel_.ymin && y <= pixel_.ymin);
  }

  //const Matrix &getMatrix () const { return matrix_ ; }
  //const Matrix &getIMatrix() const { return imatrix_; }

 private:
  Range pixel_;
  Range window_;

  Range window1_;

  double window_xc1_     { 0.0 };
  double window_yc1_     { 0.0 };
  double window_width1_  { 1.0 };
  double window_height1_ { 1.0 };

  double factor_x_  { 1.0 };
  double factor_y_  { 1.0 };
  double factor_x1_ { 1.0 };
  double factor_y1_ { 1.0 };

  double pdx_ { 0.0 };
  double pdy_ { 0.0 };

  bool equal_scale_ { false };
  bool scale_min_   { true };

  HAlign halign_ { HAlign::CENTER };
  VAlign valign_ { VAlign::CENTER };

  bool flip_x_ { false };
  bool flip_y_ { false };

  //Matrix matrix_;
  //Matrix imatrix_;
};

#endif
