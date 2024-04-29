#ifndef CQChartsWidget_H
#define CQChartsWidget_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>
#include <QWidget>
#include <QPointer>
#include <cassert>

/*!
 * \brief Widget Data
 * \ingroup Charts
 */
class CQChartsWidget : public CQChartsEqBase<CQChartsWidget> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsWidget, metaTypeId)

 public:
  CQChartsWidget() = default;

  explicit CQChartsWidget(QWidget *widget);
  explicit CQChartsWidget(const QString &s);

  //---

  QWidget *widget() const;
  void setWidget(QWidget *widget);

  //---

  QWidget *parent() const;
  void setParent(QWidget *widget);

  //---

  bool isVisible() const;
  void setVisible(bool b);

  //---

  bool isValid() const { return widget_; }

  int width () const;
  int height() const;

  QSize size() const { return QSize(width(), height()); }

  QSize sizeHint() const;

  //---

  QString id() const;
  void setId(const QString &id);

  //---

  QString path() const { return path_; }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  void initSize();

  void move(double x, double y);
  void resize(double w, double h);

  //---

  friend bool operator==(const CQChartsWidget &lhs, const CQChartsWidget &rhs) {
    return (lhs.path() == rhs.path());
  }

 private:
  void updatePath();

 private:
  using WidgetP = QPointer<QWidget>;

  WidgetP widget_;
  QString path_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsWidget)

#endif
