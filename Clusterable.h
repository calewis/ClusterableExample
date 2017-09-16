#ifndef CLUSTERABLE_H__
#define CLUSTERABLE_H__

#include "common.h"

#include <memory>
#include <typeindex>
#include <typeinfo>

namespace clustering {

// Function to collapse a single U into a vector
template <typename U>
auto collapse(U const& u) {
  return std::vector<U>{u};
}

// Function to collapse some type T into a vector of U, this exists to be
// specialized for user types
template <typename U, typename T,
          typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
std::vector<U> collapse(T const& t);

// Function to just copy a vector of U
template <typename U, typename A>
auto collapse(std::vector<U, A> const& u_vec) {
  return std::vector<U>(u_vec);
}

// Function to collapse some vector<T> into a vector of U.  SFINAE is needed so
// that when T = U the call is not ambiguous
template <typename U, typename T, typename A,
          typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
auto collapse(std::vector<T, A> const& ts) {
  std::vector<U> out;
  for (auto const& t : ts) {
    auto u_vec = collapse<U>(t);
    out.insert(out.end(), u_vec.begin(), u_vec.end());
  }
  return out;
}

/* Here is where the fun begins
 *
 * Let's say that you want some polymorphic like behavior such that a bunch of
 * different types satisfy some interface, but you don't want to have those
 * types inherit from a common base class. Well one option for this is using
 * type erasure.
 *
 * So here we have a ClusterConcept, which is going to be collapsible down to
 * some type U and is going to provide a function center so that any type T can
 * be clustered as long as it is collapsible to U and has a function called
 * center which can be found via ADL.
 *
 */
template <typename U>
class ClusterConcept {
 public:
  virtual Vector center_() const = 0;
  virtual std::type_index type_() const = 0;
  virtual std::vector<U> collapse_() const = 0;
};

/* ClusterModel is the type that actually holds our T that has a center and can
 * be collapsed to a U.
 */
template <typename T, typename U>
class ClusterModel : public ClusterConcept<U> {
 private:
  T element_;

 public:
  ClusterModel(T t) : element_(std::move(t)) {}

  // Need to proved the function Vector center(T const& t); for each type T
  Vector center_() const override final { return center(element_); }

  std::type_index type_() const override final {
    return std::type_index(typeid(element_));
  }

  // Need to proved the function std::vector<U> collapse<U>(T const& t); for
  // each type T a few have been given above.
  std::vector<U> collapse_() const override final {
    return collapse<U>(element_);
  }
};

/*
 * Clusterable is the class that actually holds the type erased object inside a
 * shared_ptr.  This is nice because we can now do things like
 * vector<Clusterable<U>> where every Clusterable an represent a different type.
 * Think of this as being similar to boost any except that the types must
 * provide a call to collapse<U> and also much provide a center function to
 * retrieve a vector representing the types center.
 *
 * I left all of the compiler generated functions to be defaulted, although
 * that means that element_ptr will always be shallow copied.  It is trivial to
 * add a clone method for deep copies though.
 */
template <typename U>
class Clusterable {
 private:
  std::shared_ptr<ClusterConcept<U>> element_ptr_;

 public:
  // Constructor that will take any type T that satisfies the ClusterConcept
  // interface and will type erase that type.
  template <typename T>
  Clusterable(T t)
      : element_ptr_(std::make_shared<ClusterModel<T, U>>(
            ClusterModel<T, U>(std::move(t)))) {}

  Vector center() const { return element_ptr_->center_(); }
  std::type_index type() const { return element_ptr_->type_(); }
  std::vector<U> collapse() const { return element_ptr_->collapse_(); }
};

}  // namespace clustering

#endif  // CLUSTERABLE_H__
