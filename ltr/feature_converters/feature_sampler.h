// Copyright 2011 Yandex

#ifndef LTR_FEATURE_CONVERTERS_FEATURE_SAMPLER_H_
#define LTR_FEATURE_CONVERTERS_FEATURE_SAMPLER_H_

#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include "ltr/data/object.h"
#include "ltr/data/feature_info.h"
#include "ltr/data/data_set.h"
#include "ltr/utility/indices.h"
#include "ltr/feature_converters/feature_converter.h"

using std::max_element;
using std::vector;
using std::string;
using ltr::utility::Indices;
using ltr::utility::IndicesPtr;

namespace ltr {
/**
* \brief Samples features with specified indices.
* \param indices indices of features to sample
*/
class FeatureSampler : public FeatureConverter {
  public:
  typedef boost::shared_ptr<FeatureSampler> Ptr;

  /**
  * \param indices indices of features to sample
  */
  explicit FeatureSampler(Indices indices = Indices(),
                          const FeatureInfo& feature_info = FeatureInfo())
      : FeatureConverter(feature_info), indices_(indices) {
    fillOutputFeatureInfo();
  }

  virtual void fillOutputFeatureInfo();

  virtual string generateCppCode(const string& function_name) const;

  GET(Indices, indices);
  void set_indices(const Indices& indices);
 private:
  void applyImpl(const Object& input, Object* output) const;
  Indices indices_;
};
}
#endif  // LTR_FEATURE_CONVERTERS_FEATURE_SAMPLER_H_
