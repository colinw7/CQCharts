#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>
#include <CBBox2D.h>
#include <QPointer>
#include <map>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsLoader;
class CQChartsView;
class CQChartsPlot;
class CQChartsPlotType;
class CQChartsCsv;
class CQChartsTsv;
class CQChartsModel;

class QAbstractItemModel;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QHBoxLayout;

class CQChartsTest : public CQAppWindow {
  Q_OBJECT

 public:
  typedef boost::optional<double> OptReal;

 public:
  struct InitData {
    typedef std::map<QString,QString> NameValues;
    typedef std::map<QString,bool>    NameBools;
    typedef std::vector<QString>      FileNames;

    bool       csv             { false };
    bool       tsv             { false };
    bool       json            { false };
    bool       data            { false };
    QString    typeName;
    NameValues nameValues;
    NameBools  nameBools;
    QString    columnType;
    FileNames  filenames;
    bool       commentHeader   { false };
    bool       firstLineHeader { false };
    bool       xintegral       { false };
    bool       yintegral       { false };
    QString    title;
    QString    properties;
    OptReal    xmin, ymin, xmax, ymax;
    bool       y1y2            { false };
    bool       overlay         { false };
    int        nr              { 1 };
    int        nc              { 1 };
    double     dx              { 1000.0 };
    double     dy              { 1000.0 };

    QString nameValue(const QString &name) const {
      auto p = nameValues.find(name);

      return (p != nameValues.end() ? (*p).second : QString());
    }

    void setNameValue(const QString &name, const QString &value) {
      nameValues[name] = value;
    }

    bool nameBool(const QString &name) const {
      auto p = nameBools.find(name);

      return (p != nameBools.end() ? (*p).second : false);
    }

    void setNameBool(const QString &name, bool value) {
      nameBools[name] = value;
    }
  };

 public:
  CQChartsTest();

 ~CQChartsTest();

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  CQChartsView *view() const;

  CQChartsModel *loadCsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  CQChartsModel *loadTsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  CQChartsModel *loadJson(const QString &filename);
  CQChartsModel *loadData(const QString &filename, bool commentHeader, bool firstLineHeader);

  bool initPlot(const InitData &initData);

  CQChartsPlot *init(const InitData &initData, int i);

 private:
  void addMenus();

  void setTableModel(QAbstractItemModel *model);
  void setTreeModel (QAbstractItemModel *model);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool stringToColumn(const QString &str, int &column) const;

  CQChartsPlot * createPlot(CQChartsPlotType *type, const InitData &initData, bool reuse);

  CQChartsView *getView(bool reuse=true);

  QSize sizeHint() const;

 private slots:
  void loadSlot();

  void loadFileSlot(const QString &type, const QString &filename);

  void createSlot();

  void filterSlot();
  void tableColumnClicked(int column);
  void typeOKSlot();

 private:
  typedef std::vector<CQChartsPlot *> Plots;
  typedef QPointer<CQChartsView>      ViewP;

  Plots               plots_;
  CQChartsPlot*       rootPlot_          { nullptr };
  CQCharts*           charts_            { nullptr };
  CQChartsCsv*        csv_               { nullptr };
  CQChartsTsv*        tsv_               { nullptr };
  QAbstractItemModel* model_             { nullptr };
  QLineEdit*          columnTypeEdit_    { nullptr };
  QStackedWidget*     stack_             { nullptr };
  QLineEdit*          filterEdit_        { nullptr };
  CQChartsTable*      table_             { nullptr };
  CQChartsTree*       tree_              { nullptr };
  int                 tableColumn_       { 0 };
  CQChartsLoader*     loader_            { nullptr };
  ViewP               view_;
  QString             id_;
  CBBox2D             bbox_;
};

#endif
