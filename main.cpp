#include "Clusterable.h"
#include "common.h"
#include "molecule.h"

#include <boost/optional.hpp>
#include <iostream>

// Center for our molecule class.
Vector center(Molecule const& m) { return libint2::center(m.atoms()); }

// Specialization of Collapse needed to collapse the Molecule class
namespace clustering {
template <>
std::vector<Atom> collapse<Atom, Molecule>(Molecule const& m) {
  return m.atoms();
}
}  // namespace clustering

template <typename... Types>
class ChargeVisitor : public clustering::ClusterConceptVisitor,
                      public clustering::ClusterModelVisitor<Types>... {
 private:
  boost::optional<int> charge_ = boost::optional<int>(boost::none);

 public:
  void visit(Atom const& a) override {
    charge_ = boost::optional<int>(boost::none);
    charge_ = a.atomic_number;
  }

  void visit(std::vector<Atom> const& as) override {
    charge_ = boost::optional<int>(boost::none);
    int c = 0;
    for (auto const& a : as) {
      c += a.atomic_number;
    }
    charge_ = c;
  }

  boost::optional<int> get_charge() {
    auto mycopy = charge_;
    charge_ = boost::optional<int>(boost::none);
    return mycopy;
  }
};

int main(int argc, char** argv) {
  // Clusterable of an Atom, see nearest_neighbor.cpp for an example that
  // actually does clustering
  clustering::Clusterable<Atom> c1(Atom{});
  ChargeVisitor<Atom, std::vector<Atom>> a{};
  c1.accept(a);
  auto charge = a.get_charge();

  if (charge) {
    std::cout << "Charge was available\n";
  } else {
    std::cout << "Charge was not available\n";
  }

  // Some other type we have provided an external interface to work with
  // clusterable, std::vector<Atom>
  clustering::Clusterable<Atom> c2(std::vector<Atom>(2, Atom{}));

  c2.accept(a);
  charge = a.get_charge();

  if (charge) {
    std::cout << "Charge was available\n";
  } else {
    std::cout << "Charge was not available\n";
  }

  // Nested Vectors of Atoms
  std::vector<std::vector<Atom>> meta_vec{c1.collapse(), c2.collapse()};
  clustering::Clusterable<Atom> c3(std::move(meta_vec));

  c3.accept(a);
  charge = a.get_charge();

  if (charge) {
    std::cout << "Charge was available\n";
  } else {
    std::cout << "Charge was not available\n";
  }

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
