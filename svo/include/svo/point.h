// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// SVO is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or any later version.
//
// SVO is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SVO_POINT_H_
#define SVO_POINT_H_

#include <boost/noncopyable.hpp>
#include <svo/global.h>

namespace g2o {
  class VertexSBAPointXYZ;
}
typedef g2o::VertexSBAPointXYZ g2oPoint;

namespace svo {

class Feature;

typedef Matrix<double, 2, 3> Matrix23d;

/// A 3D point that was triangulated from at least two corresponding image features.
class Point : boost::noncopyable
{
public:

  enum PointType {DELETED, CANDIDATE, UNKNOWN_QUALITY, GOOD_QUALITY };
  enum PointOrigin { TRIANGULATION, DEPTH, VOGIATZIS, IDFILTER };

  static int                  point_counter_;           //!< Counts the number of created points. Used to set the unique id.
  int                         id_;                      //!< Unique ID of the point.
  Vector3d                    pos_;                     //!< 3d pos of the point in the world coordinate frame.
  list<Feature*>              obs_;                     //!< References to keyframes which observe the point.
  size_t                      n_obs_;                   //!< Number of obervations, not only keyframes.
  g2oPoint*                   v_pt_;                    //!< Temporary pointer to the point-vertex in g2o during bundle adjustment.
  int                         last_published_ts_;       //!< Timestamp of last publishing.
  int                         last_projected_kf_id_;    //!< Flag for the reprojection: don't reproject a pt twice.
  PointType                   type_;                    //!< Quality of the point.
  PointOrigin                 origin_;                  //!< What is the source of the point, i.e. DEPTH if a kinect is used.
  int                         n_failed_reproj_;         //!< Number of failed reprojections. Used to assess the quality of the point.
  int                         n_succeeded_reproj_;      //!< Number of succeeded reprojections. Used to assess the quality of the point.
  int                         last_structure_optim_;

  Point(Vector3d pos, PointOrigin origin);
  ~Point();

  /// Add a reference to a frame.
  void addFrameRef(Feature* ftr);

  /// Remove reference to a frame.
  bool deleteFrameRef(Frame* frame);

  /// Check whether mappoint has reference to a frame.
  Feature* findFrameRef(Frame* frame);

  /// Get Frame with similar viewpoint.
  bool getCloseViewObs(const Vector3d& pos, Feature*& obs) const;

  /// Get number of observations.
  inline size_t nRefs() const { return obs_.size(); }

  /// Point jacobian w.r.t frame f.
  void pointJacobian(const Vector3d& p_in_f, const Matrix3d& R_f_w, Matrix23d& point_jac) const;

  /// Optimize point position through minimizing the reprojection error.
  void optimize(const size_t n_iter);
};

} // namespace svo

#endif // SVO_POINT_H_