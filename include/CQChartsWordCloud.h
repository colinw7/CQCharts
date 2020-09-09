#ifndef CQChartsWordCloud_H
#define CQChartsWordCloud_H

#include <CQChartsQuadTree.h>

#include <QObject>

#include <map>

class CQChartsPlot;

class CQChartsWordCloud : public QObject {
  Q_OBJECT

 public:
  class Rect {
   public:
    Rect() { }

    Rect(double xmin, double ymin, double xmax, double ymax) :
     xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax) {
    }

    bool isSet() const { return true; }

    double xmin() const { return xmin_; }
    void setXMin(double r) { xmin_ = r; }

    double ymin() const { return ymin_; }
    void setYMin(double r) { ymin_ = r; }

    double xmax() const { return xmax_; }
    void setXMax(double r) { xmax_ = r; }

    double ymax() const { return ymax_; }
    void setYMax(double r) { ymax_ = r; }

    //---

    double getXMin() const { return xmin_; }
    double getYMin() const { return ymin_; }
    double getXMax() const { return xmax_; }
    double getYMax() const { return ymax_; }

   private:
    double xmin_ { 0.0 };
    double ymin_ { 0.0 };
    double xmax_ { 1.0 };
    double ymax_ { 1.0 };
  };

  //! Word data
  struct WordData {
    QString word;
    int     count    { 0 };
    double  fontSize { 1.0 };
    double  angle    { 0.0 };
    double  x        { 0.0 };
    double  y        { 0.0 };
    Rect    wordRect;

    WordData(const QString &word, int count=0) :
     word(word), count(count) {
    }

    const Rect &rect() { return wordRect; }
  };

  using WordDataMap   = std::map<QString, WordData *>;
  using WordDataArray = std::vector<WordData *>;

  using Plot = CQChartsPlot;

 public:
  CQChartsWordCloud();
 ~CQChartsWordCloud();

  double minFontSize() const { return minFontSize_; }
  void setMinFontSize(double s) { minFontSize_ = s; }

  double maxFontSize() const { return maxFontSize_; }
  void setMaxFontSize(double s) { maxFontSize_ = s; }

  void addWord(const QString &word, int count);

  void place(const Plot *plot);

  const WordDataArray &wordDatas() const { return wordDataArray_; }

 private:
  void spiralPos(double t, double &x, double &y) const;

 private:
  using Tree           = CQChartsQuadTree<WordData, Rect>;
  using CountWordDatas = std::map<int, WordDataArray>;

  WordDataMap   wordDatas_;
  WordDataArray wordDataArray_;
  int           minCount_    { -1 };
  int           maxCount_    { -1 };
  double        minFontSize_ { 6 };
  double        maxFontSize_ { 48 };
  Tree          tree_;
  double        spiralDelta_ { 0.001 };
  double        spiralWidth_ { 0.002 };
  int           spiralTurns_ { 500000 };
};

#endif
