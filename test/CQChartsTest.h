#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQAppWindow.h>
#include <CQChartsGeom.h>
#include <CQExprModel.h>

#include <QAbstractItemModel>
#include <QPointer>
#include <QSharedPointer>
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
class CQChartsGradientPalette;

class CQExprModel;
class CQFoldedModel;
class CQHistoryLineEdit;

class CExpr;

class QSortFilterProxyModel;
class QItemSelectionModel;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QTabWidget;
class QComboBox;
class QGridLayout;

#ifdef CQ_CHARTS_CEIL
class ClLanguageCommand;
class ClLanguageArgs;
#endif

class CQChartsTest : public CQAppWindow {
  Q_OBJECT

 public:
  using OptReal = boost::optional<double>;
  using OptInt  = boost::optional<int>;
  using OptBool = boost::optional<bool>;
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

  struct InputData {
    bool    commentHeader   { false };
    bool    firstLineHeader { false };
    int     numRows         { 100 };
    QString filter;
    QString fold;
    QString sort;
  };

  struct NameValueData {
    NameValues values;
    NameValues strings;
    NameReals  reals;
    NameBools  bools;
  };

  struct InitData {
    using FileNames = std::vector<QString>;

    FileNames     filenames;
    FileType      fileType        { FileType::NONE };
    InputData     inputData;
    QString       typeName;
    QString       filterStr;
    QString       process;
    QString       processAdd;
    NameValueData nameValueData;
    QString       columnType;
    bool          xintegral       { false };
    bool          yintegral       { false };
    bool          xlog            { false };
    bool          ylog            { false };
    QString       title;
    QString       viewTitle;
    QString       viewProperties;
    QString       plotProperties;
    OptReal       xmin, ymin, xmax, ymax;
    bool          y1y2            { false };
    bool          overlay         { false };
    int           nr              { 1 };
    int           nc              { 1 };
    double        dx              { 1000.0 };
    double        dy              { 1000.0 };

    QString nameValue(const QString &name) const {
      auto p = nameValueData.values.find(name);

      return (p != nameValueData.values.end() ? (*p).second : QString());
    }

    void setNameValue(const QString &name, const QString &value) {
      nameValueData.values[name] = value;
    }

    QString nameString(const QString &name) const {
      auto p = nameValueData.strings.find(name);

      return (p != nameValueData.strings.end() ? (*p).second : "");
    }

    void setNameString(const QString &name, const QString &value) {
      nameValueData.strings[name] = value;
    }

    double nameReal(const QString &name) const {
      auto p = nameValueData.reals.find(name);

      return (p != nameValueData.reals.end() ? (*p).second : 0.0);
    }

    void setNameReal(const QString &name, double value) {
      nameValueData.reals[name] = value;
    }

    bool nameBool(const QString &name) const {
      auto p = nameValueData.bools.find(name);

      return (p != nameValueData.bools.end() ? (*p).second : false);
    }

    void setNameBool(const QString &name, bool value) {
      nameValueData.bools[name] = value;
    }
  };

  using FoldedModels = std::vector<ModelP>;

  struct ViewData {
    int                  tabInd       { -1 };
    bool                 hierarchical { false };
    QStackedWidget*      stack        { nullptr };
    CQChartsTable*       table        { nullptr };
    CQChartsTree*        tree         { nullptr };
    QLineEdit*           filterEdit   { nullptr };
    ModelP               model;
    QItemSelectionModel* sm           { nullptr };
    QTextEdit*           detailsText  { nullptr };
    ModelP               foldProxyModel;
    FoldedModels         foldedModels;

    ModelP currentModel() const {
      if (! foldedModels.empty())
        return foldProxyModel;
      else
        return model;
    }
  };

 public:
  CQChartsTest();

 ~CQChartsTest();

  CQChartsView *view() const;

#ifdef CQ_CHARTS_CEIL
  bool isCeil() const { return ceil_; }
  void setCeil(bool b);
#endif

  bool loadFileModel(const QString &filename, FileType type, const InputData &inputData);

  QAbstractItemModel *loadFile(const QString &filename, FileType type,
                               const InputData &inputData, bool &hierarchical);

  QAbstractItemModel *loadCsv (const QString &filename, const InputData &inputData);
  QAbstractItemModel *loadTsv (const QString &filename, const InputData &inputData);
  QAbstractItemModel *loadJson(const QString &filename, bool &hierarchical);
  QAbstractItemModel *loadData(const QString &filename, const InputData &inputData);

  QAbstractItemModel *createExprModel(int n=100);

  bool initPlot(const InitData &initData);

  CQChartsPlot *initPlotView(const ViewData *viewData, const InitData &initData,
                             int i, const CQChartsGeom::BBox &bbox);

  void setColumnFormats(const ModelP &model, const QString &columnType);

  QString fixTypeName(const QString &typeName) const;

  bool exec(const QString &filename);

  void loop();

  void timeout();

 private:
  void addMenus();

  ViewData &addViewData(bool hierarchical);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  bool stringToColumn(const ModelP &model, const QString &str, int &column) const;

  CQChartsPlot *createPlot(const ViewData *viewData, const ModelP &model, CQChartsPlotType *type,
                           const NameValueData &nameValueData, bool reuse,
                           const CQChartsGeom::BBox &bbox);

  void setViewProperties(CQChartsView *view, const QString &properties);
  void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  void updateModel(ViewData *viewData);

  void sortModel(const ViewData *viewData, const QString &args);

  void updateModelDetails(const ViewData *viewData);

  QSize sizeHint() const;

  FileType stringToFileType(const QString &str) const;
  QString fileTypeToString(FileType type) const;

  void processExpression(const QString &expr);

  void processExpression   (ModelP &model, const QString &expr);
  void processAddExpression(ModelP &model, const QString &expr);

  void processExpression(ModelP &model, CQExprModel::Function function,
                         int column, const QString &expr);

  void foldModel(ViewData *viewData, const QString &str);

  void foldClear(ViewData *viewData);

  CQExprModel *getExprModel(ModelP &model) const;

  ViewData *getViewData(int ind);

  ViewData *currentViewData();

  CQChartsView *currentView() const;

 private:
  bool isCompleteLine(QString &line) const;

  void parseLine(const QString &line);

  using Args = std::vector<QString>;

#ifdef CQ_CHARTS_CEIL
  static void setPropertyLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void getPropertyLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void viewLCmd       (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void plotLCmd       (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void loadLCmd       (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void modelLCmd      (ClLanguageCommand *, ClLanguageArgs *args, void *data);
#endif

  bool loadCmd       (const Args &args);
  void modelCmd      (const Args &args);
  void processCmd    (const Args &args);
  void sortCmd       (const Args &args);
  void viewCmd       (const Args &args);
  void plotCmd       (const Args &args);
  void overlayCmd    (const Args &args);
  void y1y2Cmd       (const Args &args);
  void paletteCmd    (const Args &args);
  void themeCmd      (const Args &args);
  void setPropertyCmd(const Args &args);
  void getPropertyCmd(const Args &args);

  void letCmd        (const Args &args);
  void ifCmd         (const Args &args);
  void whileCmd      (const Args &args);
  void continueCmd   (const Args &args);
  void printCmd      (const Args &args);

  void sourceCmd     (const Args &args);

  QStringList stringToCmds(const QString &str) const;

#ifdef CQ_CHARTS_CEIL
  Args parseCommandArgs(ClLanguageCommand *command, ClLanguageArgs *largs);
#endif

  void setCmdRc(int rc);
  void setCmdRc(const QString &rc);

 private:
  struct DefinedColor {
    double v { -1.0 };
    QColor c;

    DefinedColor(double v, const QColor &c) :
     v(v), c(c) {
    }
  };

  struct PaletteColorData {
    using DefinedColors = std::vector<DefinedColor>;

    QString       colorTypeStr;
    QString       colorModelStr;
    OptInt        redModel;
    OptInt        greenModel;
    OptInt        blueModel;
    OptBool       negateRed;
    OptBool       negateGreen;
    OptBool       negateBlue;
    OptReal       redMin;
    OptReal       redMax;
    OptReal       greenMin;
    OptReal       greenMax;
    OptReal       blueMin;
    OptReal       blueMax;
    DefinedColors definedColors;
    bool          getColorScale { false };
    bool          getColorFlag { false };
    double        getColorValue { 0.0 };

    PaletteColorData() {
      redModel    = boost::make_optional(false, 0);
      greenModel  = boost::make_optional(false, 0);
      blueModel   = boost::make_optional(false, 0);
      negateRed   = boost::make_optional(false, false);
      negateGreen = boost::make_optional(false, false);
      negateBlue  = boost::make_optional(false, false);
      redMin      = boost::make_optional(false, 0.0);
      redMax      = boost::make_optional(false, 0.0);
      greenMin    = boost::make_optional(false, 0.0);
      greenMax    = boost::make_optional(false, 0.0);
      blueMin     = boost::make_optional(false, 0.0);
      blueMax     = boost::make_optional(false, 0.0);
    }
  };

  void setPaleteData(CQChartsGradientPalette *palette, const PaletteColorData &paletteData);

 private slots:
  void loadSlot();

  bool loadFileSlot(const QString &type, const QString &filename);

  void createSlot();

  void filterSlot();

  void exprSlot();

  void foldSlot();

  void tableColumnClicked(int column);

  void typeSetSlot();

  void plotDialogCreatedSlot(CQChartsPlot *plot);

  void plotObjPressedSlot(CQChartsPlotObj *obj);

 private:
  using Plots   = std::vector<CQChartsPlot*>;
  using ViewP   = QPointer<CQChartsView>;
  using WindowP = QPointer<CQChartsWindow>;

  using ViewDatas = std::vector<ViewData>;

 private:
//Plots              plots_;
//CQChartsPlot*      rootPlot_          { nullptr };
  CQCharts*          charts_            { nullptr };
  QLineEdit*         foldEdit_          { nullptr };
  QLineEdit*         columnNumEdit_     { nullptr };
  QLineEdit*         columnNameEdit_    { nullptr };
  QLineEdit*         columnTypeEdit_    { nullptr };
  QTabWidget*        viewTab_           { nullptr };
  ViewDatas          viewDatas_;
  QComboBox*         exprCombo_         { nullptr };
  CQHistoryLineEdit* exprEdit_          { nullptr };
  QLineEdit*         exprColumn_        { nullptr };
  CQChartsLoader*    loader_            { nullptr };
  ViewP              view_;
//QString            id_;
  CExpr*             expr_              { nullptr };
#ifdef CQ_CHARTS_CEIL
  bool               ceil_              { false };
#endif
  bool               continueFlag_      { false };
};

#endif
