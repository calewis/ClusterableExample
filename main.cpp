#include "Clusterable.h"
#include "common.h"
#include "molecule.h"

#include <boost/core/demangle.hpp>
#include <iostream>

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

// Center for our molecule class.
Vector center(Molecule const& m) { return libint2::center(m.atoms()); }

// Specialization of Collapse needed to collapse the Molecule class
namespace clustering {
template <>
std::vector<Atom> collapse<Atom, Molecule>(Molecule const& m) {
  return m.atoms();
}
}  // namespace clustering

int main(int argc, char** argv) {
  // Clusterable of an Atom, see nearest_neighbor.cpp for an example that
  // actually does clustering
  clustering::Clusterable<Atom> c1(Atom{});

  // Some other type we have provided an external interface to work with
  // clusterable, std::vector<Atom>
  clustering::Clusterable<Atom> c2(std::vector<Atom>(2, Atom{}));

  // Nested Vectors of Atoms
  std::vector<std::vector<Atom>> meta_vec{c1.collapse(), c2.collapse()};
  clustering::Clusterable<Atom> c3(std::move(meta_vec));

  // A class representing a collection of Atoms
  Molecule mol(std::vector<Atom>(4, Atom{}));
  clustering::Clusterable<Atom> c4(std::move(mol));

  // Even though we have 4 different collections of atoms we can hold a vector
  // to all of them similar to haveing something like
  // std::vector<Clusterable*> if we were inheriting from some common base type
  // Clusterable instead of using type erasure
  std::vector<clustering::Clusterable<Atom>> clusterable_vec;
  clusterable_vec.emplace_back(std::move(c1));
  clusterable_vec.emplace_back(std::move(c2));
  clusterable_vec.emplace_back(std::move(c3));
  clusterable_vec.emplace_back(std::move(c4));

  // Loop over all of the differnt types and print their types, centers, and
  // member atoms. 
  auto i = 1;
  for (auto const& c : clusterable_vec) {
    std::cout << "Clusterable " << i << "\n";
    std::cout << "\tstored type: " << boost::core::demangle(c.type().name())
              << "\n";
    std::cout << "\tcenter: " << c.center().transpose() << "\n";
    std::cout << "\tAtoms:\n";
    for (auto const& a : c.collapse()) {
      std::cout << "\t\t" << a.atomic_number << " " << center(a).transpose()
                << "\n";
    }
    std::cout << std::flush;
    ++i;
  }

  return 0;
}
