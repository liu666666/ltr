// Copyright 2012 Yandex

#include <string>
#include <vector>
#include <algorithm>

#include "ltr/utility/boost/lexical_cast.hpp"
#include "ltr/serialization_test/generator/generator_utility.h"
#include "ltr/serialization_test/generator/config.h"
#include "ltr/scorers/scorer.h"
#include "ltr/feature_converters/feature_converter.h"
#include "ltr/data/data_set.h"
#include "ltr/data/utility/io_utility.h"

using std::string;
using std::vector;
using std::ofstream;
using std::copy;

using ltr::DataSet;
using ltr::Object;
using ltr::io_utility::loadDataSet;
using ltr::utility::lexical_cast;
using ltr::Scorer;
using ltr::FeatureConverter;

namespace serialization_test {
  void Generator::addIncludes() {
    tester_code << "// Copyright 2012 Yandex\n\n"
      << "#include <vector>\n\n"
      << "#include \"gtest/gtest.h\"\n\n"
      << "#include \"ltr/serialization_test/generator/config.h\"\n"
      << "#include \"ltr/serialization_test/tester/tester_utility.h\"\n\n"
      << "#include \"ltr/data/data_set.h\"\n"
      << "#include \"ltr/data/utility/io_utility.h\"\n"
      << "using std::vector;\n"
      << "using ltr::Object;\n"
      << "using ltr::DataSet;\n"
      << "using ltr::io_utility::loadDataSet;\n\n"
      << "using namespace serialization_test;\n\n";
  }

  void Generator::addFixture() {
    tester_code << "class SerializationTest : public ::testing::Test {\n"
      << "  protected:\n"
      << "  DataSet<Object> dataset;\n"
      << "  vector<double> serializated_labels;\n"
      << "  vector<double> test_labels;\n\n"
      << "  virtual void SetUp() {\n"
      << "    dataset = loadDataSet<Object>(TestDataPath(), \"SVMLIGHT\");\n"
      << "  }\n"
      << "};\n\n";
  }

  void Generator::addCheckLabelsFunction() {
    LexicalCastConfig::getInstance().setPrecision(12);
    LexicalCastConfig::getInstance().setFixed(true);
    tester_code << "void checkLabels" << serializable_object_number
      << "(const vector<double>& labels) {\n";

    tester_code << "  vector<double> test_labels;\n";
    for (int object_index = 0;
         object_index < (int)test_data.size();
         ++object_index) {
      tester_code << "  test_labels.push_back("
        << lexical_cast<string>(test_data[object_index].predicted_label())
        << ");\n";
    }

    tester_code << "  EXPECT_TRUE(Equal(test_labels, labels))\n"
      << "    << Report<vector<double> >(test_labels, labels);\n"
      << "}\n\n";
  }

  void Generator::addCheckFeaturesFunction(
      const DataSet<Object>& processed_data) {
    LexicalCastConfig::getInstance().setPrecision(12);
    LexicalCastConfig::getInstance().setFixed(true);
    tester_code << "void checkFeatures" << serializable_object_number
      << "(const vector<vector<double> >& data) {\n";

    tester_code << "  vector<vector<double> > test_data("
      << processed_data.size() << ");\n";
    for (int object_index = 0;
         object_index < processed_data.size();
         ++object_index) {
      for (int feature_index = 0;
           feature_index < processed_data.feature_count();
           ++feature_index) {
        tester_code << "  test_data[" << object_index << "].push_back("
          << lexical_cast<string>(
          processed_data[object_index][feature_index]) << ");\n";
        }
    }

    tester_code << "  EXPECT_TRUE(Equal(test_data, data))\n"
      << "    << Report<vector< vector<double> > >(test_data, data);\n"
      << "}\n\n";
  }

  void Generator::addScorerTest(
      Learner<Object>::Ptr learner,
      const string& test_name) {
    LexicalCastConfig::getInstance().setPrecision(12);
    LexicalCastConfig::getInstance().setFixed(true);
    learner->learn(train_data);
    Scorer::Ptr scorer = learner->make();
    scorer->predict(test_data);

    addBeginBlockComment(test_name);
    addCheckLabelsFunction();
    tester_code << scorer->generateCppCode(
      "SavedScorer" + lexical_cast<string>(serializable_object_number))
      << "\n";
    tester_code << "TEST_F(SerializationTest, " << test_name << ") {\n"
      << "  vector<double> testing_labels;\n"
      << "  for (int i = 0; i < dataset.size(); ++i) {\n"
      << "    int feature_count =  dataset[i].feature_count();\n"
      << "    vector<double> test(feature_count);\n"
      << "    for (int feature = 0; feature < feature_count; ++feature) {\n"
      << "      test[feature] = dataset[i][feature];\n"
      << "    }\n"
      << "    testing_labels.push_back(SavedScorer"
      << serializable_object_number << "(test));\n"
      << "  }\n"
      << "  checkLabels" << serializable_object_number
      << "(testing_labels);\n" << "}\n\n";
    addEndBlockComment(test_name);

    ++serializable_object_number;
  }

  void Generator::addFeatureConverterTest(
      FeatureConverterLearner<Object>::Ptr feature_converter_learner,
      const string& test_name) {
    LexicalCastConfig::getInstance().setPrecision(12);
    LexicalCastConfig::getInstance().setFixed(true);
    feature_converter_learner->learn(train_data);
    FeatureConverter::Ptr feature_converter = feature_converter_learner->make();
    DataSet<Object> processed_data;
    feature_converter->apply(test_data, &processed_data);

    addBeginBlockComment(test_name);
    addCheckFeaturesFunction(processed_data);
    tester_code << feature_converter->generateCppCode(
      "SavedFeatureConverter" +
      lexical_cast<string>(serializable_object_number));

    tester_code << "TEST_F(SerializationTest, " << test_name << ") {\n"
      << "  vector<vector<double> > testing_data(dataset.size());\n"
      << "  for (int i = 0; i < dataset.size(); ++i) {\n"
      << "    int feature_count =  dataset[i].feature_count();\n"
      << "    vector<double> test(feature_count);\n"
      << "    for (int feature = 0; feature < feature_count; ++feature) {\n"
      << "      test[feature] = dataset[i][feature];\n"
      << "    }\n"
      << "    SavedFeatureConverter" << serializable_object_number
      << "(test, &testing_data[i]);\n"
      << "  }\n"
      << "  checkFeatures" << serializable_object_number
      << "(testing_data);\n" << "}\n\n";
    addEndBlockComment(test_name);

    ++serializable_object_number;
  }

  Generator::Generator():
      train_data(loadDataSet<Object>(TrainDataPath(), "SVMLIGHT")),
      test_data(loadDataSet<Object>(TestDataPath(), "SVMLIGHT")),
      serializable_object_number(0) {
    addIncludes();
    addFixture();
  }

  string Generator::code() const {
    return tester_code.str();
  }

  void Generator::write(const char* filename) const {
    ofstream fout(filename);
    fout << code();
    fout.close();
  }

  void Generator::addBeginBlockComment(const string& message) {
      // 80 - correct length of code string, 80 = "//" + 78
    string slashes(78, '#');
    string inserted = message + "_block_begin";
      // 20 - near the middle of line, to make message centered
    copy(inserted.begin(), inserted.end(), slashes.begin() + 20);
    tester_code << "//" << slashes << "\n";
  }

  void Generator::addEndBlockComment(const string& message) {
    string slashes(78, '#');
    string inserted = message + "_block_end";
    copy(inserted.begin(), inserted.end(), slashes.begin() + 20);
    tester_code << "//" << slashes << "\n\n";
  }
};
