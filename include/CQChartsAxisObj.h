#ifndef CQChartsAxisObj_H
#define CQChartsAxisObj_H

#include <CQChartsTextBoxObj.h>

class CQChartsAxis;

class CQChartsAxisLabel : public CQChartsTextBoxObj {
  Q_OBJECT

 public:
  CQChartsAxisLabel(CQChartsAxis *axis);

  void redrawBoxObj() override;

 private:
  CQChartsAxis *axis_ { nullptr };
};

//---

class CQChartsAxisTickLabel : public CQChartsTextBoxObj {
  Q_OBJECT

 public:
  CQChartsAxisTickLabel(CQChartsAxis *axis);

  void redrawBoxObj() override;

 private:
  CQChartsAxis *axis_ { nullptr };
};

#endif
