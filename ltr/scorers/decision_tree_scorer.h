// Copyright 2011 Yandex

#ifndef LTR_SCORERS_DECISION_TREE_SCORER_H_
#define LTR_SCORERS_DECISION_TREE_SCORER_H_

#include <string>

#include "ltr/scorers/scorer.h"

#include "ltr/learners/decision_tree/vertex/vertex.h"
#include "ltr/learners/decision_tree/decision_tree.h"

using std::string;

namespace ltr {
namespace decision_tree {
class DecisionTreeScorer : public Scorer {
 public:
  typedef ltr::utility::shared_ptr<DecisionTreeScorer> Ptr;

  DecisionTreeScorer() {}

  explicit DecisionTreeScorer(DecisionTree::Ptr tree)
  : tree_(tree) {}

  void setTree(DecisionTree::Ptr tree);

  string toString() const;

 private:
  double scoreImpl(const Object& obj) const;

  string generateCppCodeImpl(const string& function_name) const;

  virtual string getDefaultAlias() const;

  DecisionTree::Ptr tree_;
};
};
};

#endif  // LTR_SCORERS_DECISION_TREE_SCORER_H_
