#ifndef CQChartsRand_H
#define CQChartsRand_H

#include <random>

namespace CQChartsRand {

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

class RealInRange {
 public:
  RealInRange(double min, double max) :
   eng_(rd_()), rdis_(min, max) {
  }

  double gen() {
    return rdis_(eng_);
  }

 private:
  std::random_device                     rd_;
  std::default_random_engine             eng_;
//std::mt19937                           mt_; // mersine twister
  std::uniform_real_distribution<double> rdis_;
};

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
