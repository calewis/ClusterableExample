#ifndef COMMON_H__
#define COMMON_H__

#include <libint2/atom.h>
#include <Eigen/Dense>

using Atom = libint2::Atom;
using Vector = Eigen::VectorXd;

namespace libint2 {
/*
 * The next three functions are just defining the center function for different
 * types.  This illistrates the power of this type erased method where we can
 * create Clusterables of different types, and adapt them to our purposes
 * without having to modify the type.  For example libint2::Atom is provided by
 * a third party and might be unreasonable or impossible to modify.
 */
Vector center(Atom const& a) {
  Vector cent(3);
  cent.setZero();

  cent[0] = a.x;
  cent[1] = a.y;
  cent[2] = a.z;

  return cent;
}

Vector center(std::vector<Atom> const& a_vec) {
  Vector cent(3);
  cent.setZero();

  for (auto const& a : a_vec) {
    cent += center(a);
  }

  return cent / static_cast<double>(a_vec.size());
}

Vector center(std::vector<std::vector<Atom>> const& meta_vec) {
  Vector cent(3);  
  cent.setZero();

  for (auto const& a_vec : meta_vec) {
    cent += center(a_vec);
  }

  return cent / static_cast<double>(meta_vec.size());
}
}  // namespace libint2

#endif  // COMMON_H__
