#include <CQChartsPaintDevice.h>
#include <CMandelbrot.h>

#include <iostream>

class TestChartsImpl {
 public:
  void draw(CQChartsPaintDevice *device) {
    int max_iterations = 256;

    auto p1 = device->windowToPixel(
                CQChartsGeom::Point(mandelbrot_.getXMin(), mandelbrot_.getYMax()));
    auto p2 = device->windowToPixel(
                CQChartsGeom::Point(mandelbrot_.getXMax(), mandelbrot_.getYMin()));

    int nx = int(p2.x - p1.x);
    int ny = int(p2.y - p1.y);

    for (int iy = 0; iy < ny; ++iy) {
      for (int ix = 0; ix < nx; ++ix) {
        auto p = device->pixelToWindow(CQChartsGeom::Point(ix, iy));

        int ic = mandelbrot_.calc(p.x, p.y, max_iterations);

        auto g = double(ic)/max_iterations;

        QColor c(g, g, g);

        device->setPen(c);

        device->drawPoint(p);
      }
    }
  }

 private:
  CMandelbrot mandelbrot_;
};

static TestChartsImpl *impl;

//---

extern "C" {

void test_charts_init() {
  std::cerr << "test_charts_init\n";

  impl = new TestChartsImpl;
}

void test_charts_draw(void *data) {
  std::cerr << "test_charts_draw\n";

  auto *device = reinterpret_cast<CQChartsPaintDevice *>(data);

  impl->draw(device);
}

}
