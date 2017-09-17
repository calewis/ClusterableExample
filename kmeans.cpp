#include "Clusterable.h"
#include "cluster.h"
#include "common.h"
#include "molecule.h"

#include <boost/core/demangle.hpp>
#include <fstream>
#include <iostream>

namespace clustering {
  template<>
    std::vector<Atom> collapse<Atom, Cluster<Atom>>(Cluster<Atom> const& c){
      return c.collapse();
    }

  auto center(Cluster<Atom> const& c){
    return c.center();
  }
}

auto atom_to_clusterables(std::vector<Atom> const& vs) {
  std::vector<clustering::Clusterable<Atom>> out;
  for (auto const& v : vs) {
    out.emplace_back(v);
  }
  return out;
}

int main(int argc, char** argv) {
  std::string filename = (argc > 1) ? argv[1] : "";
  if (filename.empty()) {
    std::cout << "Didn't pass in an xyz file\n";
    return 1;
  }

  std::ifstream is(filename);
  std::vector<Atom> atoms;
  if (filename.rfind(".xyz") != std::string::npos)
    atoms = libint2::read_dotxyz(is);
  else
    throw "only .xyz files are accepted";

  auto natoms = atoms.size();
  std::cout << "Number of atoms: " << natoms << std::endl;

  auto clusterables = atom_to_clusterables(atoms);

  auto n_small_clusters = std::max(int(natoms)/2, 1);
  auto magic_seed = 42;
  clustering::Kmeans kmeans_small(n_small_clusters, magic_seed);

  auto small_clusters = kmeans_small(clusterables);

  auto n = 1;
  for(auto const& c : small_clusters){
    std::cout << "Cluster " << n << " has atoms:\n";
    for(auto atom : c.collapse()){
      std::cout << "\t" << center(atom).transpose() << "\n";
    }
    std::cout << std::flush;
    ++n;
  }

  std::vector<clustering::Clusterable<Atom>> meta_clusters;
  for (auto const& cluster : small_clusters) {
    meta_clusters.push_back(cluster);
  }
  
  if(meta_clusters.size() > 10){
    std::cout << "\n\nClustering our small clusters\n";
    clustering::Kmeans kmeans_big(4, magic_seed);
    auto clusters_of_clusters = kmeans_big(meta_clusters);

    auto n = 1;
    for(auto const& c : clusters_of_clusters){
      std::cout << "Cluster " << n << " has atoms:\n";
      for(auto atom : c.collapse()){
        std::cout << "\t" << center(atom).transpose() << "\n";
      }
      std::cout << std::flush;
      ++n;
    }
  }

  return 0;
}
