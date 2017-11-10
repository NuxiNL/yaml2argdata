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

#ifndef YAML2ARGDATA_YAML_ERROR_FACTORY_H
#define YAML2ARGDATA_YAML_ERROR_FACTORY_H

#include <sstream>
#include <string_view>
#include <vector>

#include <yaml-cpp/exceptions.h>

#include <yaml2argdata/yaml_factory.h>

namespace yaml2argdata {

template <typename T>
class YAMLErrorFactory : public YAMLFactory<T> {
 public:
  T GetNull(const YAML::Mark& mark) override {
    throw YAML::ParserException(mark, "Unsupported null");
  }

  T GetScalar(const YAML::Mark& mark, std::string_view tag,
              std::string_view value) override {
    std::ostringstream ss;
    ss << "Unsupported scalar with tag \"" << tag << "\"";
    throw YAML::ParserException(mark, ss.str());
  }

  T GetSequence(const YAML::Mark& mark, std::string_view tag,
                std::vector<T> elements) override {
    std::ostringstream ss;
    ss << "Unsupported sequence with tag \"" << tag << "\"";
    throw YAML::ParserException(mark, ss.str());
  }

  T GetMap(const YAML::Mark& mark, std::string_view tag, std::vector<T> keys,
           std::vector<T> values) override {
    std::ostringstream ss;
    ss << "Unsupported map with tag \"" << tag << "\"";
    throw YAML::ParserException(mark, ss.str());
  }
};

}  // namespace yaml2argdata

#endif
