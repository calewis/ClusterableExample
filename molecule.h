#ifndef MOLECULE_H__
#define MOLECULE_H__

#include "common.h"

#include <vector>

/*
 * A dummy Molecule class that is just functional enough to work for our cluster
 * example. In reality this class would provide significantly more functionality
 */
class Molecule {
 private:
  std::vector<Atom> atoms_;

 public:
  Molecule(std::vector<Atom> atoms) : atoms_(std::move(atoms)) {}

  std::vector<Atom> const& atoms() const { return atoms_; }
};


#endif  // MOLECULE_H__
