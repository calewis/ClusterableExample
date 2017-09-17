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
how ever many clusters were specified. 

I am sure that this example needs some work after all I made it from scratch
over the weekend, but hopefully this gives you some idea of the features of C++
that I have experience with and the types of programming that went into MPQC4.

To compile main.cpp I used 

```
clang++ -std=c++14 -I/path/to/eigen3 -I/path/to/libint/include main.cpp
```

To compile kmeans.cpp I used 

```
clang++ -std=c++14 -O2 -I/path/to/eigen3 -I/path/to/libint/include kmeans.cpp
```

To run kmeans.cpp I used 

```
./a.out Azithromycin.xyz
```

or
```
./a.out big_water_drop.xyz
```
