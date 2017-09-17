This a simple example of how one can use type erasure to adapt an interface
onto a large number of different types, without having to use inheritance. 
It is based on ideas from Sean Parent originally from his talk called 
Inheritance is the Base Class of Evil 
https://channel9.msdn.com/Events/GoingNative/2013/Inheritance-Is-The-Base-Class-of-Evil

This is a technique used in MPQC4 to generate blocks of atoms or shells that
can then be converted into blocks of a tensor. The main goal is to end up with something like a std::vector<std::vector<Atom>> where each blocks in a dimension of the tensor are blocked such that { functions from atom in cluster 0 , functions from atom in cluster 1 , . . . , functions from atoms in cluster N}

The motivating reason for MPQC to use this type erasing like interface instead of something more simple is to have the ability for combining clustering steps in a composable fashion such as 
```
std::vector<Atom> atoms = read_input(some_file_name);
std::vector<Clusterable<Atom>> clusterable_atoms = atom_to_clusterables(std::move(atoms));

auto number_of_clusters = nclusters(input_file);
std::vector<std::vector<Atom>> clustered_atoms = kmeans(group_by_charge(clusterable_atoms), number_of_clusters);
```

Where something like the group_by_charge function will first create groups of
atoms that have the same charge and will then kmeans cluster those groups in
to how ever many clusters were specified. 

I am sure that this example needs some work after all I made it from scratch
over the weekend, but hopefully this gives you some idea of the features of C++
that I have experience with and the types of programming that went into MPQC4.

# Executables
## main.cpp
main.cpp gives an example of how we can make a clusterable type that can capture
a bunch of different types and put all of them in a vector while still having
access to certain information about each element. 

## kmeans.cpp
Is an example that shows how this might be useful.  It first clusters a molecule
(provided in an xyz file) into a large number of small clusters.  Then since
each cluster can also be type erased into a clusterable we can take these small
clusters and cluster them into large clusters, finally collapsing to atoms and
printing the members of the clusters. 

One long term goal (not implemented here, but the idea should be apparent) is
is to allow for a hierarchy of clustering which could for example allow for
efficient formation of H-matrices using functions centered on Atoms or just
allow for hierarchical blocks of functions providing a way to minimize
replicated meta data in a sparse tensor data structure. 

## Dependencies 

* Eigen 3: Just for a nice already implemented vector
* Libint2: https://github.com/evaleev/libint an integral engine library
  developed in our research group that already has an atom and a way to read
  xyz files. Libint2 can be a bit tricky to install so email for help if needed.

## Compiling
To compile main.cpp use

```
clang++ -std=c++14 -I/path/to/eigen3 -I/path/to/libint/include main.cpp
```

To compile kmeans.cpp use (-O2 because otherwise the large water drop can take
a few seconds).

```
clang++ -std=c++14 -O2 -I/path/to/eigen3 -I/path/to/libint/include kmeans.cpp
```

## Running Kmean example

To run kmeans.cpp I used 

```
./a.out Azithromycin.xyz
```

or (The clustering code does not take care to optimize for copies and
allocations so a big molecule can be a little slow.)

```
./a.out big_water_drop.xyz
```
