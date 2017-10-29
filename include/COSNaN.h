#ifndef COSNaN_H
#define COSNaN_H

namespace COSNaN {
  bool has_nan();
  bool is_nan(double real);
  bool set_nan(double *real);
  bool set_nan(double &real);

  double get_nan();

  bool is_inf(double real);
  bool is_pos_inf(double real);
  bool is_neg_inf(double real);
  bool set_pos_inf(double &real);
  bool set_neg_inf(double &real);
}

#endif
