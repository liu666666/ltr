// Copyright 2012 Yandex

#ifndef LTR_LEARNERS_DECISION_TREE_DECISION_TREE_H_
#define LTR_LEARNERS_DECISION_TREE_DECISION_TREE_H_

#include <rlog/rlog.h>

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>

#include "ltr/learners/decision_tree/vertex/vertex.h"

#include "ltr/utility/boost/shared_ptr.h"

using std::string;
using std::vector;

namespace ltr {
namespace decision_tree {
/**
 * Decision tree. It contains a pointer to the root Vertex.
 */
class DecisionTree : public SerializableFunctor<double> {
  friend class ltr::utility::shared_ptr<DecisionTree>;
 public:
  typedef ltr::utility::shared_ptr<DecisionTree> Ptr;
 protected:
  ~DecisionTree();

 public:
  explicit DecisionTree(Vertex::Ptr root)
  : root_(root) {}
  /**
   * Returns the decision of the tree for the given object.
   * @param object - object to decide.
   */
  double value(const Object& object) const;
  /**
   * Sets the root vertex.
   * @param root - new root of the tree.
   */
  void setRoot(Vertex::Ptr root);

  virtual string generateCppCode(const string& function_name) const;

 private:
  virtual string getDefaultAlias() const;

  Vertex::Ptr root_;
};
};
};

#endif  // LTR_LEARNERS_DECISION_TREE_DECISION_TREE_H_
