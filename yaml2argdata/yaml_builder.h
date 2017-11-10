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

#ifndef YAML2ARGDATA_YAML_BUILDER_H
#define YAML2ARGDATA_YAML_BUILDER_H

#include <cassert>
#include <istream>
#include <string>
#include <variant>
#include <vector>

#include <yaml-cpp/anchor.h>
#include <yaml-cpp/emitterstyle.h>
#include <yaml-cpp/eventhandler.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/mark.h>
#include <yaml-cpp/parser.h>

#include <yaml2argdata/yaml_factory.h>

namespace yaml2argdata {

template <typename T>
class YAMLBuilder : YAML::EventHandler {
 public:
  explicit YAMLBuilder(YAMLFactory<T>* factory) : factory_(factory), root_() {
  }

  T Build(std::istream* stream) {
    YAML::Parser parser(*stream);
    root_ = factory_->GetNull({});
    parser.HandleNextDocument(*this);
    assert(stack_.empty() &&
           "Composite structures remain on the parsing stack");
    return std::move(root_);
  }

 private:
  struct Sequence {
    YAML::Mark mark;
    std::string tag;
    std::vector<T> elements;
  };

  struct Map {
    YAML::Mark mark;
    std::string tag;
    std::vector<T> keys;
    std::vector<T> values;
  };

  YAMLFactory<T>* const factory_;

  std::vector<std::variant<Sequence, Map>> stack_;
  T root_;

  void OnDocumentStart(const YAML::Mark& mark) override {
  }

  void OnDocumentEnd() override {
  }

  void OnNull(const YAML::Mark& mark, YAML::anchor_t anchor) override {
    Append(factory_->GetNull(mark));
  }

  void OnAlias(const YAML::Mark& mark, YAML::anchor_t anchor) override {
    throw YAML::ParserException(mark, "Unsupported alias");
  }

  void OnScalar(const YAML::Mark& mark, const std::string& tag,
                YAML::anchor_t anchor, const std::string& value) override {
    Append(factory_->GetScalar(mark, tag, value));
  }

  void OnSequenceStart(const YAML::Mark& mark, const std::string& tag,
                       YAML::anchor_t anchor,
                       YAML::EmitterStyle::value style) override {
    stack_.emplace_back(Sequence{mark, tag, {}});
  }

  void OnSequenceEnd() override {
    Sequence sequence = std::move(std::get<Sequence>(stack_.back()));
    stack_.pop_back();
    Append(factory_->GetSequence(sequence.mark, sequence.tag,
                                 std::move(sequence.elements)));
  }

  void OnMapStart(const YAML::Mark& mark, const std::string& tag,
                  YAML::anchor_t anchor,
                  YAML::EmitterStyle::value style) override {
    stack_.emplace_back(Map{mark, tag, {}, {}});
  }

  void OnMapEnd() override {
    Map map = std::move(std::get<Map>(stack_.back()));
    stack_.pop_back();
    Append(factory_->GetMap(map.mark, map.tag, std::move(map.keys),
                            std::move(map.values)));
  }

  void Append(T node) {
    if (stack_.empty()) {
      root_ = std::move(node);
    } else {
      auto composite = &stack_.back();
      if (Sequence* sequence = std::get_if<Sequence>(composite)) {
        sequence->elements.push_back(std::move(node));
      } else {
        Map* map = std::get_if<Map>(composite);
        if (map->keys.size() == map->values.size())
          map->keys.push_back(std::move(node));
        else
          map->values.push_back(std::move(node));
      }
    }
  }
};

}  // namespace yaml2argdata

#endif
