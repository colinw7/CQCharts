#ifndef CCOLOR_RANGE_H
#define CCOLOR_RANGE_H

#include <vector>

struct CRGB {
  double r { 0.0 };
  double g { 0.0 };
  double b { 0.0 };
};

class CColorRange {
 public:
  CColorRange();

 ~CColorRange() { }

  const CRGB &getColor(unsigned int i) const {
    return colors_[i];
  }

 private:
  void init();

 private:
  using Colors = std::vector<CRGB>;

  Colors colors_;
};

#endif
