#include <CColorRange.h>

CColorRange::
CColorRange()
{
  init();
}

void
CColorRange::
init()
{
  colors_.resize(256);

  double s = 1.0/255.0;

  colors_[0] = { 0.0, 0.0, 0.0 };

  /* We want Blues, Greens then Reds in our Palette */

  for (unsigned int i = 1; i < 85; ++i) {
    double i1 = s*(        0 );
    double i2 = s*(3*      i );
    double i3 = s*(3*(86 - i));

    colors_[i +   0] = { i1, i2, i3 };
    colors_[i +  85] = { i2, i3, i1 };
    colors_[i + 170] = { i3, i1, i2 };
  }

  double i1 = s*  0;
  double i2 = s*255;
  double i3 = s*  3;

  colors_[ 85] = { i1, i2, i3 };
  colors_[170] = { i2, i3, i1 };
  colors_[255] = { i3, i1, i2 };
}
