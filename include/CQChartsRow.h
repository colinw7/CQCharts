#ifndef CQChartsRow_H
#define CQChartsRow_H

/*!
 * \brief Model row class
 * \ingroup Charts
 */
class CQChartsRow {
 public:
  CQChartsRow(int row=-1) :
   row_(row) {
  }

  //--

  bool isValid() const { return row_ != -1; }

  int row() const { return row_; }
  void setRow(int row) { row_ = row; }

 private:
  int row_ { -1 };
};

#endif
