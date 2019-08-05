#ifndef CQChartsRand_H
#define CQChartsRand_H

#include <random>

namespace CQChartsRand {

/*!
 * \brief Integer in range
 * \ingroup Charts
 */
class IntInRange {
 public:
  IntInRange(int min, int max) :
   eng_(rd_()), idis_(min, max) {
  }

  int gen() {
    return idis_(eng_);
  }

 private:
  std::random_device                 rd_;
  std::default_random_engine         eng_;
  std::uniform_int_distribution<int> idis_;
};

/*!
 * \brief Real in range
 * \ingroup Charts
 */
class RealInRange {
 public:
  RealInRange(double min, double max) :
   eng_(rd_()), rdis_(min, max) {
  }

  double gen() {
    return rdis_(eng_);
  }

 private:
  std::random_device                     rd_;   //!< random device
  std::default_random_engine             eng_;  //!< random engine
//std::mt19937                           mt_;   //!< mersine twister
  std::uniform_real_distribution<double> rdis_; //!< uniform distribution
};

/*!
 * \brief Normalized Real
 * \ingroup Charts
 */
class NormalRealInRange {
 public:
  NormalRealInRange(double mean, double stddev) :
   eng_(rd_()), rdis_(mean, stddev) {
  }

  double gen() {
    return rdis_(eng_);
  }

 private:
  std::random_device               rd_;
  std::default_random_engine       eng_;
  std::normal_distribution<double> rdis_;
};

}

#endif
