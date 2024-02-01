#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "../utils/data_structures.h"

namespace geti {

extern const std::string GETI_EMPTY_LABEL;

static inline std::vector<Label>
get_labels_from_configuration(ov::AnyMap configuration) {
  auto labels_iter = configuration.find("labels");
  auto label_ids_iter = configuration.find("label_ids");
  std::vector<Label> labels = {};
  if (labels_iter != configuration.end() &&
      label_ids_iter != configuration.end()) {
    std::vector<std::string> label_ids =
        label_ids_iter->second.as<std::vector<std::string>>();
    std::vector<std::string> label_names =
        labels_iter->second.as<std::vector<std::string>>();
    for (size_t i = 0; i < label_ids.size(); i++) {
      if (label_names.size() > i)
        labels.push_back({label_ids[i], label_names[i]});
      else
        labels.push_back({label_ids[i], ""});
    }
  }
  return labels;
}

} // namespace geti

#endif // UTILS_H_
