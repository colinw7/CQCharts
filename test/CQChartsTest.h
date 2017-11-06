#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>
#include <QAbstractItemModel>
#include <QPointer>
#include <QSharedPointer>

#include <CBBox2D.h>
#include <map>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsTable;
class CQChartsTree;
class CQChartsLoader;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsPlotType;
class CQChartsPlotObj;
class CQExprModel;

class QItemSelectionModel;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QGridLayout;

class CQChartsTest : public CQAppWindow {
  Q_OBJECT

 public:
  using OptReal = boost::optional<double>;
  using ModelP  = QSharedPointer<QAbstractItemModel>;

 public:
  enum class FileType {
    NONE,
    CSV,
    TSV,
    JSON,
    DATA,
    EXPR
  };

  using NameValues = std::map<QString,QString>;
  using NameReals  = std::map<QString,double>;
  using NameBools  = std::map<QString,bool>;

  struct InitData {
    using FileNames = std::vector<QString>;

    FileNames  filenames;
    FileType   fileType        { FileType::NONE };
    bool       commentHeader   { false };
    bool       firstLineHeader { false };
    int        numRows         { 100 };
    QString    typeName;
    QString    filterStr;
    QString    process;
    NameValues nameValues;
    NameValues nameStrings;
    NameReals  nameReals;
    NameBools  nameBools;
    QString    columnType;
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

    QString nameString(const QString &name) const {
      auto p = nameStrings.find(name);

      return (p != nameStrings.end() ? (*p).second : "");
    }

    void setNameString(const QString &name, const QString &value) {
      nameStrings[name] = value;
    }

    double nameReal(const QString &name) const {
      auto p = nameReals.find(name);

      return (p != nameReals.end() ? (*p).second : 0.0);
    }

    void setNameReal(const QString &name, double value) {
      nameReals[name] = value;
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

  bool loadFileModel(const QString &filename, FileType type,
                     bool commentHeader, bool firstLineHeader, int n=100);

  QAbstractItemModel *loadFile(const QString &filename, FileType type,
                               bool commentHeader, bool firstLineHeader, int n,
                               bool &hierarchical);

  QAbstractItemModel *loadCsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  QAbstractItemModel *loadTsv (const QString &filename, bool commentHeader, bool firstLineHeader);
  QAbstractItemModel *loadJson(const QString &filename, bool &hierarchical);
  QAbstractItemModel *loadData(const QString &filename, bool commentHeader, bool firstLineHeader);

  QAbstractItemModel *createExprModel(int n=100);

  bool initPlot(const InitData &initData);

  CQChartsPlot *init(const ModelP &model, const InitData &initData, int i);

  void setColumnFormats(const ModelP &model, const QString &columnType);

  QString fixTypeName(const QString &typeName) const;

  void loop();

  void timeout();

 private:
  void addMenus();

  void setTableModel(const ModelP &model);
  void setTreeModel (const ModelP &model);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool stringToColumn(const ModelP &model, const QString &str, int &column) const;

  CQChartsPlot *createPlot(const ModelP &model, CQChartsPlotType *type,
                           const NameValues &nameValues, const NameValues &nameStrings,
                           const NameReals &nameReals, const NameBools &nameBools, bool reuse);

  void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  void updateModelDetails();

  QSize sizeHint() const;

  FileType stringToFileType(const QString &str) const;
  QString fileTypeToString(FileType type) const;

  void processExpression(const QString &expr);

 private:
  void parseLine(const std::string &line);

  using Args = std::vector<std::string>;

  void setCmd   (const Args &args);
  void getCmd   (const Args &args);
  void plotCmd  (const Args &args);
  void loadCmd  (const Args &args);
  void sourceCmd(const Args &args);

 private slots:
  void loadSlot();

  void loadFileSlot(const QString &type, const QString &filename);

  void createSlot();

  void filterSlot();

  void exprSlot();

  void tableColumnClicked(int column);
  void typeOKSlot();

  void plotDialogCreatedSlot(CQChartsPlot *plot);

  void plotObjPressedSlot(CQChartsPlotObj *obj);

 private:
  using Plots   = std::vector<CQChartsPlot*>;
  using ViewP   = QPointer<CQChartsView>;
  using WindowP = QPointer<CQChartsWindow>;

  Plots                plots_;
  CQChartsPlot*        rootPlot_          { nullptr };
  CQCharts*            charts_            { nullptr };
  ModelP               model_;
  QLineEdit*           columnNumEdit_     { nullptr };
  QLineEdit*           columnTypeEdit_    { nullptr };
  QStackedWidget*      stack_             { nullptr };
  QLineEdit*           filterEdit_        { nullptr };
  CQChartsTable*       table_             { nullptr };
  CQChartsTree*        tree_              { nullptr };
  QItemSelectionModel* sm_                { nullptr };
  QLineEdit*           exprEdit_          { nullptr };
  QTextEdit*           detailsText_       { nullptr };
  CQChartsLoader*      loader_            { nullptr };
  ViewP                view_;
  WindowP              window_;
  QString              id_;
  CBBox2D              bbox_;
};

#endif
