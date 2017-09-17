#ifndef CLUSTER_H__
#define CLUSTER_H__

#include "Clusterable.h"
#include "common.h"

#include <random>
#include <vector>

namespace clustering {

/*
 * Cluster is going to be a tool we use for clustering importantly Clusters can
 * themselves be Clusterables.
 *
 * This implementation of Cluster is not really safe since center is not updated
 * by functions like add, but this is just an example.
 */
template <typename U>
class Cluster {
 private:
  std::vector<Clusterable<U>> elements_;
  Vector center_;

 public:
  Cluster() : center_(Vector::Zero(3)) {}
  Cluster(Vector v) : center_(std::move(v)) {}

  void add(Clusterable<U> u) { elements_.push_back(std::move(u)); }

  Vector const& center() const { return center_; }

  Vector const& compute_center() {
    center_.setZero();
    for (auto const& e : elements_) {
      center_ += e.center();
    }

    center_ /= static_cast<double>(elements_.size());

    return center_;
  }

  void erase() { elements_.clear(); }

  std::vector<U> collapse() const {
    std::vector<U> out;
    for (auto const& e : elements_) {
      auto us = e.collapse();
      out.insert(out.end(), us.begin(), us.end());
    }
    return out;
  }

  // Get a copy of the clusterables, stored in this cluster, should be cheap
  // since they are all shallow copied.
  std::vector<Clusterable<U>> clusterables() const { return elements_; }
};

/*
 * A simple K-means function for clustering, doesn't try hard to come up with a
 * good guess so the clusters might be pretty bad.
 *
 */
class Kmeans {
 private:
  int nclusters_;
  int seed_;

  // Just init based on randomly sampling the centers of the clusterables
  template <typename U>
  std::vector<Cluster<U>> init_clusters(
      std::vector<Clusterable<U>> const& clabls) {
    std::mt19937 gen(seed_);
    // Subtract 1 becasue the range includes the rightmost number
    std::uniform_int_distribution<int> dist(0, clabls.size() - 1);

    std::vector<Cluster<U>> out;
    out.reserve(nclusters_);
    for (auto i = 0; i < nclusters_; ++i) {
      auto center_guess = clabls[dist(gen)].center();
      out.push_back(Cluster<U>(std::move(center_guess)));
    }

    return out;
  }

  template <typename U>
  void attach_to_nearest(std::vector<Cluster<U>>& clusters,
                         std::vector<Clusterable<U>> const& clusterables) {
    for (auto const& clusterable : clusterables) {
      auto it = std::min_element(clusters.begin(), clusters.end(),
                                 [&clusterable](auto const& a, auto const& b) {
                                   auto cent = clusterable.center();
                                   return (cent - a.center()).norm() <
                                          (cent - b.center()).norm();
                                 });
      it->add(clusterable);
    }
  }

  template <typename U>
  void update_centers(std::vector<Cluster<U>>& clusters, bool erase = true) {
    for (auto& c : clusters) {
      c.compute_center();
      if (erase) {
        c.erase();
      }
    }
  }

 public:
  Kmeans(int nclusters, int seed = std::random_device{}())
      : nclusters_(nclusters), seed_(seed) {}

  template <typename U>
  std::vector<Cluster<U>> operator()(std::vector<Clusterable<U>> clusterables) {
    auto clusters = init_clusters<U>(clusterables);
    for (auto i = 0; i < 10; ++i) {
      attach_to_nearest(clusters, clusterables);
      update_centers(clusters, i != 9);  // not checking convergence here. Just
                                         // keep clusterables on the final
                                         // iteration
    }

    // Since we use a bad guess and don't try hard to converge only return
    // clusters that have members
    decltype(clusters) clusters_exist;
    for(auto const &cluster : clusters){
      if(!cluster.clusterables().empty()){
        clusters_exist.push_back(cluster);
      }
    }

    return clusters_exist;
  }

};
}  // namespace clustering

#endif  // CLUSTER_H__
