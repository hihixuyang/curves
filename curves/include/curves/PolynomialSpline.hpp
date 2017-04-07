/*
 * PolynomialSpline.hpp
 *
 *  Created on: Mar 7, 2017
 *      Author: Dario Bellicoso
 */

#pragma once

// eigen
#include <Eigen/Core>

// curves
#include "curves/polynomial_splines_traits.hpp"

namespace curves {

template <int splineOrder_>
class PolynomialSpline {
 public:

  static constexpr unsigned int splineOrder = splineOrder_;
  static constexpr unsigned int coefficientCount = splineOrder + 1;

  using SplineImplementation = spline_traits::spline_rep<double, splineOrder>;
  using SplineCoefficients = typename SplineImplementation::SplineCoefficients;

  PolynomialSpline() :
    currentTime_(0.0),
    duration_(0.0),
    didEvaluateCoeffs_(false),
    coefficients_()
  {

  }

  virtual ~PolynomialSpline() {

  }

  const SplineCoefficients& getCoefficients() const {
    return coefficients_;
  }

  bool computeCoefficients(const SplineOptions& options) {
    SplineImplementation::compute(options, coefficients_);
    duration_ = options.tf_;
    return true;
  }

  void setCoefficientsAndDuration(const SplineCoefficients& coefficients, double duration) {
    coefficients_ = coefficients;
    duration_ = duration;
  }

  void setCoefficientsAndDuration(const Eigen::Matrix<double, coefficientCount, 1>& coefficients, double duration) {
    for (unsigned int k=0; k<coefficientCount; k++) {
      coefficients_[k] = coefficients(k);
    }
    duration_ = duration;
  }

  constexpr double getPositionAtTime(double tk) const {
    return std::inner_product(coefficients_.begin(), coefficients_.end(), SplineImplementation::tau(tk).begin(), 0.0);
  }

  constexpr double getVelocityAtTime(double tk) const {
    return std::inner_product(coefficients_.begin(), coefficients_.end(), SplineImplementation::dtau(tk).begin(), 0.0);
  }

  constexpr double getAccelerationAtTime(double tk) const {
    return std::inner_product(coefficients_.begin(), coefficients_.end(), SplineImplementation::ddtau(tk).begin(), 0.0);
  }

  void advanceTime(double dt) {
    currentTime_ += dt;
  }

  void resetTime() {
    currentTime_ = 0.0;
  }

  double getTime() const {
    return currentTime_;
  }

  double getSplineDuration() const {
    return duration_;
  }

 protected:
  //! A helper counter which used to get the state of the spline.
  double currentTime_;

  //! The duration of the spline in seconds.
  double duration_;

  //! True if the coefficents were computed at least once.
  bool didEvaluateCoeffs_;

  /*
   * s(t) = an*t^n + ... + a1*t + a0
   * splineCoeff_ = [an ... a1 a0]
   */
  SplineCoefficients coefficients_;
};

} /* namespace */