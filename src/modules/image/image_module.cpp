#include <CQChartsModuleData.h>
#include <QPainter>
#include <tcl.h>

#include <iostream>

enum ImageChartsId {
  IMAGE_MAIN = 0
};

class ImageChartsImpl {
 public:
  ImageChartsImpl() { }

  int init(const char *args) {
    QString str(args);

    drawImage_ = QImage(str);

    return IMAGE_MAIN;
  }

  void getData(CQChartsModuleGetData *data) {
    static char *buffer = nullptr;
    static uint  bufferSize = 0;

    QString res = "{properties {";

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
    if (data->id == IMAGE_MAIN) {
      std::cerr << "setData: " << data->buffer << "\n";

      QStringList words;

      splitList(data->buffer, words);

      for (auto &word : words) {
        //std::cerr << "Word: " << word.toStdString() << "\n";
      }
    }
  }

  void handleEvent(CQChartsModuleEventData *data) {
    if (data->id == IMAGE_MAIN) {
    }
  }

  void draw(CQChartsModuleDrawData *data) {
    if (data->id == IMAGE_MAIN) {
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

      if (changed) {
        data_ = *data;

        QPainter ipainter(&image_);

        auto image1 = drawImage_.scaled(QSize(xsize_, ysize_));

        ipainter.drawImage(0, 0, image1);

        mainChanged_ = false;
      }

      data->painter->drawImage(data->pixel_xmin, data->pixel_ymin, image_);
    }
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
  int                    xsize_              { 0 };
  int                    ysize_              { 0 };
  QImage                 image_;
  QImage                 drawImage_;
  CQChartsModuleDrawData data_;
  bool                   mainChanged_        { false };
  double                 main_pixel_x_       { 0.0 };
  double                 main_pixel_y_       { 0.0 };
};

static ImageChartsImpl *s_impl;

//---

extern "C" {

int image_charts_init(void *data) {
  //std::cerr << "image_charts_init\n";

  const auto *args = reinterpret_cast<const char *>(data);

  s_impl = new ImageChartsImpl;

  return s_impl->init(args);
}

int image_charts_get_data(void *data) {
  auto *getData = reinterpret_cast<CQChartsModuleGetData *>(data);

  s_impl->getData(getData);

  return 0;
}

int image_charts_set_data(void *data) {
  auto *setData = reinterpret_cast<CQChartsModuleSetData *>(data);

  s_impl->setData(setData);

  return 0;
}

int image_charts_draw(void *data) {
  //std::cerr << "image_charts_draw\n";

  auto *drawData = reinterpret_cast<CQChartsModuleDrawData *>(data);

  s_impl->draw(drawData);

  return 0;
}

int image_charts_event(void *data) {
  //std::cerr << "image_charts_event\n";

  auto *eventData = reinterpret_cast<CQChartsModuleEventData *>(data);

  s_impl->handleEvent(eventData);

  return 0;
}

}
