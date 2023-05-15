#include <CQChartsModuleData.h>
#include <CMandelbrot.h>
#include <CDisplayRange2D.h>
#include <CColorRange.h>
#include <QPainter>
#include <tcl.h>

#include <iostream>

enum MandelbrotChartsId {
  MANDELBROT_MAIN     = 0,
  MANDELBROT_OVERVIEW = 1,
  MANDELBROT_PROBE_X  = 2,
  MANDELBROT_PROBE_Y  = 3
};

class MandelbrotChartsImpl {
 public:
  MandelbrotChartsImpl() { }

  int init(const char *args) {
    QString str(args);

    if      (str == "overview")
      return MANDELBROT_OVERVIEW;
    else if (str == "probe_x")
      return MANDELBROT_PROBE_X;
    else if (str == "probe_y")
      return MANDELBROT_PROBE_Y;
    else
      return MANDELBROT_MAIN;
  }

  void getData(CQChartsModuleGetData *data) {
    static char *buffer = nullptr;
    static uint  bufferSize = 0;

    QString res = "{properties {";

    if (data->id == MANDELBROT_MAIN)
      res += QString("{iterations int %1}").arg(iterations_);

    res += "}}";

    if (res.length() > bufferSize) {
      bufferSize = res.length();

      delete buffer;

      buffer = new char [bufferSize + 1];
    }

    memcpy(buffer, res.toLatin1().constData(), res.length() + 1);

    data->buffer = buffer;
  }

  void setData(CQChartsModuleSetData *data) {
    if (data->id == MANDELBROT_MAIN) {
      std::cerr << "setData: " << data->buffer << "\n";

      QStringList words;

      splitList(data->buffer, words);

      for (auto &word : words) {
        //std::cerr << "Word: " << word.toStdString() << "\n";

        QStringList words1;
        splitList(word, words1);

        if (words1.size() > 1 && words1[0] == "properties") {
          QStringList words2;
          splitList(words1[1], words2);

          for (auto &word2 : words2) {
            //std::cerr << "Word2: " << word2.toStdString() << "\n";

            QStringList words3;
            splitList(word2, words3);

            if (words3.size() > 2 && words3[0] == "iterations") {
              iterations_ = words3[2].toInt();

              std::cerr << "Set iterations " << iterations_ << "\n";

              mainChanged_ = true;
            }
          }
        }
      }
    }
  }

  void handleEvent(CQChartsModuleEventData *data) {
    if (data->id == MANDELBROT_MAIN) {
      if (data->mode == CQCHARTS_EVENT_MODE_PROBE || data->mode == CQCHARTS_EVENT_MODE_SELECT) {
        probe_pixel_x_ = (data->pixel_x - main_pixel_x_);
        probe_pixel_y_ = (data->pixel_y - main_pixel_y_);

        probe_plot_x_ = data->plot_x;
        probe_plot_y_ = data->plot_y;

        //std::cerr << "probe " << probe_pixel_x_ << " " << probe_pixel_y_ << "\n";
      }
    }
  }

  void draw(CQChartsModuleDrawData *data) {
    if (data->id == MANDELBROT_MAIN) {
      //std::cerr << "draw main\n";

      main_pixel_x_ = data->pixel_xmin;
      main_pixel_y_ = data->pixel_ymin;

      xsize_ = int(data->pixel_xmax - data->pixel_xmin);
      ysize_ = int(data->pixel_ymax - data->pixel_ymin);

      bool changed = mainChanged_;

      if (xsize_ != image_.width() || ysize_ != image_.height()) {
        image_ = QImage(QSize(xsize_, ysize_), QImage::Format_ARGB32_Premultiplied);

        changed = true;
      }

      if (data->plot_xmin != data_.plot_xmin || data->plot_ymin != data_.plot_ymin ||
          data->plot_xmax != data_.plot_xmax || data->plot_ymax != data_.plot_ymax) {
        changed = true;
      }

      if (changed) {
        data_ = *data;

        QPainter ipainter(&image_);

        CDisplayRange2D range(0, 0, xsize_ - 1, ysize_ - 1,
                              data->plot_xmin, data->plot_ymin, data->plot_xmax, data->plot_ymax);

        colors_.resize(xsize_*ysize_);

        for (int iy = 0; iy < ysize_; ++iy) {
          for (int ix = 0; ix < xsize_; ++ix) {
            double x, y;

            range.pixelToWindow(ix, iy, &x, &y);

            auto ic = mandelbrot_.calc(x, y, iterations_);

            colors_[iy*xsize_ + ix] = ic;

            double g = 0.0;

            if (ic < iterations_)
              g = double(ic)/double(iterations_ - 1);

            auto c = indColor(g);

            ipainter.setPen(c);

            ipainter.drawPoint(ix, iy);
          }
        }

        mainChanged_ = false;
      }

      data->painter->drawImage(data->pixel_xmin, data->pixel_ymin, image_);
    }
    else if (data->id == MANDELBROT_OVERVIEW) {
      //std::cerr << "draw overview\n";

      int xsize = int(data->pixel_xmax - data->pixel_xmin);
      int ysize = int(data->pixel_ymax - data->pixel_ymin);

      bool changed = false;

      if (xsize != overviewImage_.width() || ysize != overviewImage_.height()) {
        overviewImage_ = QImage(QSize(xsize, ysize), QImage::Format_ARGB32_Premultiplied);

        changed = true;
      }

      if (changed) {
        QPainter ipainter(&overviewImage_);

        CDisplayRange2D range(0, 0, xsize_ - 1, ysize_ - 1,
                              mandelbrot_.getXMin(), mandelbrot_.getYMin(),
                              mandelbrot_.getXMax(), mandelbrot_.getYMax());

        for (int iy = 0; iy < ysize; ++iy) {
          for (int ix = 0; ix < xsize; ++ix) {
            double x, y;

            range.pixelToWindow(ix, iy, &x, &y);

            auto ic = mandelbrot_.calc(x, y, overviewIterations_);

            double g = 0.0;

            if (ic < overviewIterations_)
              g = double(ic)/double(overviewIterations_ - 1);

            auto c = indColor(g);

            ipainter.setPen(c);

            ipainter.drawPoint(ix, iy);
          }
        }
      }

      data->painter->drawImage(data->pixel_xmin, data->pixel_ymin, overviewImage_);

      {
      CDisplayRange2D range(data->pixel_xmin, data->pixel_ymin, data->pixel_xmax, data->pixel_ymax,
                            mandelbrot_.getXMin(), mandelbrot_.getYMin(),
                            mandelbrot_.getXMax(), mandelbrot_.getYMax());

      double x1, y1, x2, y2;
      range.windowToPixel(data_.plot_xmin, data_.plot_ymin, &x1, &y1);
      range.windowToPixel(data_.plot_xmax, data_.plot_ymax, &x2, &y2);

      double x3, y3;
      range.windowToPixel(probe_plot_x_, probe_plot_y_, &x3, &y3);

      data->painter->setPen(Qt::red);
      data->painter->setBrush(Qt::NoBrush);

      data->painter->drawRect(QRectF(x1, y1, x2 - x1, y2 - y1));

      data->painter->drawLine(data->pixel_xmin, y3, data->pixel_xmax, y3);
      data->painter->drawLine(x3, data->pixel_ymin, x3, data->pixel_ymax);
      }
    }
    else if (data->id == MANDELBROT_PROBE_X) {
      //std::cerr << "draw probe x\n";

      CDisplayRange2D range(data->pixel_xmin, data->pixel_ymin, data->pixel_xmax, data->pixel_ymax,
                            data->plot_xmin, data->plot_ymin, data->plot_xmax, data->plot_ymax);

      data->painter->setPen(Qt::black);

      int iy = std::min(std::max(int(probe_pixel_y_), 0), ysize_);

      for (int ix = 0; ix < xsize_; ++ix) {
        auto ic = colors_[iy*xsize_ + ix];

        auto y = double(ic)/double(iterations_);

        double g = 0.0;

        if (ic < iterations_)
          g = double(ic)/double(iterations_ - 1);

        auto c = indColor(g);

        data->painter->setPen(c);

        double px, py1, py2;
        range.windowToPixel(0.0, 0.0, &px, &py1);
        range.windowToPixel(0.0,   y, &px, &py2);

        double px1 = ix + data->pixel_xmin;

        data->painter->drawLine(px1, py1, px1, py2);
      }
    }
    else if (data->id == MANDELBROT_PROBE_Y) {
      //std::cerr << "draw probe y\n";

      CDisplayRange2D range(data->pixel_xmin, data->pixel_ymin, data->pixel_xmax, data->pixel_ymax,
                            data->plot_xmin, data->plot_ymin, data->plot_xmax, data->plot_ymax);

      data->painter->setPen(Qt::black);

      int ix = std::min(std::max(int(probe_pixel_x_), 0), xsize_);

      auto dx = double(xsize_ - ysize_)/2.0;

      for (int iy = 0; iy < ysize_; ++iy) {
        auto ic = colors_[iy*xsize_ + ix];

        auto y = double(ic)/double(iterations_);

        double g = 0.0;

        if (ic < iterations_)
          g = double(ic)/double(iterations_ - 1);

        auto c = indColor(g);

        data->painter->setPen(c);

        double px, py1, py2;
        range.windowToPixel(0.0, 0.0, &px, &py1);
        range.windowToPixel(0.0,   y, &px, &py2);

        double px1 = iy + data->pixel_xmin + dx;

        data->painter->drawLine(px1, py1, px1, py2);
      }
    }
  }

  QColor indColor(double g) {
    int ic = std::min(std::max(int(g*255.0), 0), 255);
    auto rgb = colorRange_.getColor(ic);
    return QColor(rgb.r*255.0, rgb.g*255.0, rgb.b*255.0);

    //return QColor(g, g, g);
  }

  bool splitList(const QString &str, QStringList &strs) {
    int    argc;
    char **argv;

    int rc = Tcl_SplitList(nullptr, str.toLatin1().constData(),
                           &argc, const_cast<const char ***>(&argv));

    if (rc != TCL_OK)
      return false;

    for (int i = 0; i < argc; ++i)
      strs.push_back(QString(argv[i]));

    Tcl_Free(reinterpret_cast<char *>(argv));

    return true;
  }

 private:
  CMandelbrot            mandelbrot_;
  CColorRange            colorRange_;
  int                    xsize_              { 0 };
  int                    ysize_              { 0 };
  QImage                 image_;
  QImage                 overviewImage_;
  CQChartsModuleDrawData data_;
  uint                   iterations_         { 256 };
  uint                   overviewIterations_ { 128 };
  bool                   mainChanged_        { false };
  double                 main_pixel_x_       { 0.0 };
  double                 main_pixel_y_       { 0.0 };
  double                 probe_pixel_x_      { 0.0 };
  double                 probe_pixel_y_      { 0.0 };
  double                 probe_plot_x_       { 0.0 };
  double                 probe_plot_y_       { 0.0 };
  std::vector<int>       colors_;
};

static MandelbrotChartsImpl *s_impl;

//---

extern "C" {

int mandelbrot_charts_init(void *data) {
  //std::cerr << "mandelbrot_charts_init\n";

  const auto *args = reinterpret_cast<const char *>(data);

  s_impl = new MandelbrotChartsImpl;

  return s_impl->init(args);
}

int mandelbrot_charts_get_data(void *data) {
  auto *getData = reinterpret_cast<CQChartsModuleGetData *>(data);

  s_impl->getData(getData);

  return 0;
}

int mandelbrot_charts_set_data(void *data) {
  auto *setData = reinterpret_cast<CQChartsModuleSetData *>(data);

  s_impl->setData(setData);

  return 0;
}

int mandelbrot_charts_draw(void *data) {
  //std::cerr << "mandelbrot_charts_draw\n";

  auto *drawData = reinterpret_cast<CQChartsModuleDrawData *>(data);

  s_impl->draw(drawData);

  return 0;
}

int mandelbrot_charts_event(void *data) {
  //std::cerr << "mandelbrot_charts_event\n";

  auto *eventData = reinterpret_cast<CQChartsModuleEventData *>(data);

  s_impl->handleEvent(eventData);

  return 0;
}

}
