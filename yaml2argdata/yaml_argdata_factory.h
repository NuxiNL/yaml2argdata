// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#ifndef YAML2ARGDATA_YAML_ARGDATA_FACTORY_H
#define YAML2ARGDATA_YAML_ARGDATA_FACTORY_H

#include <forward_list>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/mark.h>
#include <argdata.hpp>

#include <yaml2argdata/yaml_factory.h>

namespace yaml2argdata {

class YAMLArgdataFactory : public YAMLFactory<const argdata_t*> {
 public:
  explicit YAMLArgdataFactory(YAMLFactory<const argdata_t*>* fallback)
      : fallback_(fallback) {
  }

  const argdata_t* GetNull(const YAML::Mark& mark) override {
    return argdata_t::null();
  }

  const argdata_t* GetScalar(const YAML::Mark& mark, std::string_view tag,
                             std::string_view value) override {
    // TODO(ed): Make this logic more complete. This should at least
    // support the formats described in YAML 1.2 section 10.3.2.
    if (tag == "tag:yaml.org,2002:bool") {
      if (value == "true")
        return argdata_t::true_();
      if (value == "false")
        return argdata_t::false_();
      std::ostringstream ss;
      ss << "Unknown boolean value: " << value;
      throw YAML::ParserException(mark, ss.str());
    } else if (tag == "tag:yaml.org,2002:int") {
      {
        std::istringstream ss((std::string(value)));
        std::intmax_t i;
        ss >> std::noskipws >> i;
        if (ss && ss.eof())
          return argdatas_.emplace_back(argdata_t::create_int(i)).get();
      }
      {
        std::istringstream ss((std::string(value)));
        std::uintmax_t i;
        ss >> std::noskipws >> i;
        if (ss && ss.eof())
          return argdatas_.emplace_back(argdata_t::create_int(i)).get();
      }
      throw YAML::ParserException(mark, "Failed to parse integer literal");
    } else if (tag == "tag:yaml.org,2002:null") {
      return argdata_t::null();
    } else if (tag == "tag:yaml.org,2002:str") {
      return argdatas_
          .emplace_back(argdata_t::create_str(strings_.emplace_front(value)))
          .get();
    } else {
      return fallback_->GetScalar(mark, tag, value);
    }
  }

  const argdata_t* GetSequence(
      const YAML::Mark& mark, std::string_view tag,
      std::vector<const argdata_t*> elements) override {
    if (tag == "tag:yaml.org,2002:seq") {
      return argdatas_
          .emplace_back(
              argdata_t::create_seq(lists_.emplace_front(std::move(elements))))
          .get();
    } else {
      return fallback_->GetSequence(mark, tag, std::move(elements));
    }
  }

  const argdata_t* GetMap(const YAML::Mark& mark, std::string_view tag,
                          std::vector<const argdata_t*> keys,
                          std::vector<const argdata_t*> values) override {
    if (tag == "tag:yaml.org,2002:map") {
      return argdatas_
          .emplace_back(
              argdata_t::create_map(lists_.emplace_front(std::move(keys)),
                                    lists_.emplace_front(std::move(values))))
          .get();
    } else {
      return fallback_->GetMap(mark, tag, std::move(keys), std::move(values));
    }
  }

 private:
  YAMLFactory<const argdata_t*>* const fallback_;

  std::vector<std::unique_ptr<argdata_t>> argdatas_;
  std::forward_list<std::vector<const argdata_t*>> lists_;
  std::forward_list<std::string> strings_;
};

}  // namespace yaml2argdata

#endif
