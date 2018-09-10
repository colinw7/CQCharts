#ifndef CQChartsInvalidator_H
#define CQChartsInvalidator_H

class QObject;

class CQChartsInvalidator {
 public:
  CQChartsInvalidator(QObject *obj) :
    obj_(obj) {
  }

  void invalidate(bool reload);

 private:
  QObject *obj_ { nullptr };
};

#endif
