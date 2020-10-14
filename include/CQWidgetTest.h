#ifndef CQWidgetTest_H
#define CQWidgetTest_H

#include <QObject>
#include <map>
#include <set>

class CQWidgetTest : public QObject {
  Q_OBJECT

 public:
  enum class TestType {
    OBJECT_NAME,
    TOOLTIP
  };

 public:
  CQWidgetTest();

  void addType(const TestType &type);

  void exec();

  void test(QWidget *w);

  bool isObjectNameType(QWidget *w) const;
  bool isToolTipType   (QWidget *w) const;

  QString widgetPath(QWidget *w) const;

 private:
  QString genObjectName(QWidget *w);

 private:
  using TestTypes      = std::set<TestType>;
  using ClassNameCount = std::map<QString, int>;

  TestTypes      testTypes_;
  ClassNameCount classNameCount_;
};

#endif
