#include <curves/SE3CoefficientImplementation.hpp>
#include <iostream>
#include <glog/logging.h>
#include "kindr/minimal/quat-transformation.h"

namespace curves {

typedef kindr::minimal::QuatTransformationTemplate<double> SE3;
typedef SE3::Rotation SO3;


SE3CoefficientImplementation::SE3CoefficientImplementation() {}

SE3CoefficientImplementation::~SE3CoefficientImplementation() {}
  
bool SE3CoefficientImplementation::equals(const Eigen::VectorXd& thisCoeff,
                                                  const Eigen::VectorXd& otherCoeff, 
                                                  double tol) const {
  Eigen::VectorXd delta(6);
  localCoordinates(thisCoeff,otherCoeff,&delta);
  return delta.array().abs().maxCoeff() < tol;
}

void SE3CoefficientImplementation::makeUniqueInPlace(Eigen::VectorXd* thisCoeff ) const {
  CHECK_NOTNULL(thisCoeff);
  if ( (*thisCoeff)[3] < 0.0 ) {
    thisCoeff->segment<4>(3) *= -1.0;
  }  
}

void SE3CoefficientImplementation::makeUnique(const Eigen::VectorXd& thisCoeff,
                                                      Eigen::VectorXd* outUniqueCoeff) const {
  CHECK_NOTNULL(outUniqueCoeff);
  *outUniqueCoeff = thisCoeff;
  makeUniqueInPlace(outUniqueCoeff);
}
 
void SE3CoefficientImplementation::print(const Eigen::VectorXd& thisCoeff,
                                                 const std::string& str) const {
  std::cout << str << " " << thisCoeff.transpose();
}

void SE3CoefficientImplementation::makeValue(const Eigen::Matrix4d& matrix, Eigen::VectorXd *outValue) const {
  CHECK_NOTNULL(outValue);
  CHECK_EQ(7,outValue->size());
  SE3 pose(SO3(SO3::RotationMatrix(matrix.topLeftCorner<3,3>())),matrix.topRightCorner<3,1>());
  (*outValue) << pose.getPosition(),pose.getRotation().vector();
}



void SE3CoefficientImplementation::retract(const Eigen::VectorXd& thisCoeff,
                                                   const Eigen::VectorXd& delta, 
                                                   Eigen::VectorXd* outIncrementedCoeff) const {
  // \todo PTF Add check for dimension.
  CHECK_NOTNULL(outIncrementedCoeff);
  // the position is stored in the first 3 dimenstions, and the quaternion is in the next 4 or the coeff vector
  SE3 thisSE3(SO3(SO3::Vector4(thisCoeff.segment<4>(3))),thisCoeff.head<3>());
  // the SE3 constructor with a 6D vector is the exponential map
  SE3 updated = SE3(delta.head<6>())*thisSE3;

  (*outIncrementedCoeff) << updated.getPosition(), updated.getRotation().vector();
}

void SE3CoefficientImplementation::localCoordinates(const Eigen::VectorXd& thisCoeff,
                                                            const Eigen::VectorXd& otherCoeff, 
                                                            Eigen::VectorXd* outLocalCoordinates) const {
  // \todo PTF Add check for dimension.
  CHECK_NOTNULL(outLocalCoordinates);
  SE3 thisSE3(SO3(SO3::Vector4(thisCoeff.segment<4>(3))),thisCoeff.head<3>());
  SE3 otherSE3(SO3(SO3::Vector4(otherCoeff.segment<4>(3))),otherCoeff.head<3>());

  // local coordinates are defined to be on the left side of the transformation,
  // ie other = [delta]^ A
  SE3 delta = otherSE3*thisSE3.inverted();
  (*outLocalCoordinates) = delta.log();
}

} // namespace curves