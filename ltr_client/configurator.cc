// Copyright 2012 Yandex

#include <rlog/rlog.h>

#include <stdexcept>
#include <memory>
#include <sstream>

#include "ltr_client/configurator.h"

#include "ltr/utility/container_utility.h"

#include "ltr_client/utility/common_utility.h"
#include "ltr_client/utility/parameterized_info.h"
#include "ltr_client/utility/tag_handlers.h"

#include "tinyxml/tinyxml.h"

using std::string;
using std::stringstream;
using std::logic_error;
using std::list;
using std::cout;
using std::auto_ptr;
using std::endl;

using ltr::utility::ToString;

// ==========================  XML tokens  =====================================

// =========================== various helpers =================================

// ====================== ConfiguratorPrivate impl =============================

ConfigParser::ConfigParser() {
  root_ = NULL;
  general_xml_token_ = new OnGeneralParameterized(this);
  tag_handlers_[CONFIG] = new OnConfigParser(this);
  tag_handlers_[DATA] = new TOnDataTag(this);
  tag_handlers_[LAUNCH] = new OnLaunchTag(this);
}
ConfigParser::~ConfigParser() {
  DeleteAllFromMap(&tag_handlers_);
  delete general_xml_token_;
}

void ConfigParser::
parseConfig(const string& file_name) {
  document_ = auto_ptr<TiXmlDocument>(new TiXmlDocument(file_name));
  if (!document_->LoadFile()) {
    throw logic_error("not valid config in " + file_name);
  }

  root_ = document_->FirstChildElement(ROOT);
  if (!root_) {
    throw logic_error("can't find <LTR_experiment>");
  }

  TiXmlElement* config = root_->FirstChildElement(CONFIG);
  if (!config) {
    throw logic_error("can't find <config>");
  }

  TiXmlElement* root_dir = config->FirstChildElement(ROOT_DIR);
  if (!root_dir || !root_dir->GetText()) {
      throw logic_error("no root directory specified");
  }

  root_path_ = root_dir->GetText();

  rInfo(" LTR Client. Copyright 2011 Yandex");
  rInfo(" Experiment started ");

  GenericParse(tag_handlers_,
               root_->FirstChildElement(),
               general_xml_token_);

  rInfo("\n\nEnd of loadConfig. Collected data:\n");
  rInfo("data_infos_\n%s\n", ToString(dataInfos()).c_str());
  rInfo("xml_token_specs\n%s\n", ToString(xmlTokenSpecs()).c_str());
  rInfo("train_infos\n%s\n", ToString(trainInfos()).c_str());
  rInfo("crossvalidation_infos\n%s\n",
    ToString(crossvalidationInfos()).c_str());

  for (ParameterizedInfos::iterator it = xmlTokenSpecs().begin();
      it != xmlTokenSpecs().end();
      ++it) {
    ParameterizedInfo& spec = it->second;
    spec.fill_dependency_list(xmlTokenSpecs());
  }
}

const ConfigParser::DataInfos& ConfigParser::dataInfos() const {
  return data_infos_;
}
ConfigParser::DataInfos& ConfigParser::dataInfos() {
  return data_infos_;
}

const ConfigParser::ParameterizedInfos& ConfigParser::xmlTokenSpecs() const {
  return xml_token_specs;
}

ConfigParser::ParameterizedInfos& ConfigParser::xmlTokenSpecs() {
  return xml_token_specs;
}

const ConfigParser::TrainInfos& ConfigParser::trainInfos() const {
  return train_infos;
}

ConfigParser::TrainInfos& ConfigParser::trainInfos() {
  return train_infos;
}

const ConfigParser::CrossvalidationInfos&
                                  ConfigParser::crossvalidationInfos() const {
  return crossvalidation_infos;
}

ConfigParser::CrossvalidationInfos& ConfigParser::crossvalidationInfos() {
  return crossvalidation_infos;
}

const ParameterizedInfo& ConfigParser::findParameterized(
    const string& name) const {
  for (ParameterizedInfos::const_iterator it = xml_token_specs.begin();
       it != xml_token_specs.end();
       ++it) {
    const ParameterizedInfo& spec = it->second;
    if (spec.get_name() == name) {
      return spec;
    }
  }
  throw logic_error("Can not find parameterized object " + name);
}

const DataInfo& ConfigParser::findData(const string& name) const {
  for (DataInfos::const_iterator it = dataInfos().begin();
       it != dataInfos().end();
       ++it) {
    const DataInfo& data_info = it->second;
    if (data_info.name == name) {
      return data_info;
    }
  }
  throw logic_error("Can not find data " + name);
}


const string& ConfigParser::rootPath() const {
  return root_path_;
}
