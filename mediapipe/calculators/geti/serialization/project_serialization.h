#ifndef PROJECT_SERIALIZATION_H_
#define PROJECT_SERIALIZATION_H_

#include <nlohmann/json.hpp>
#include <string>
#include <blend2d.h>
#include <fstream>

namespace geti {

inline nlohmann::json read_file(const std::string &path) {
  std::ifstream f(path);
  nlohmann::json data = nlohmann::json::parse(f);
  f.close();
  return data;
}

class ProjectLabel {
public:
  std::string id;
  std::string name;
  BLRgba32 color;
  bool is_empty;
};

inline const ProjectLabel &get_label_by_id(const std::string &id,
                             const std::vector<ProjectLabel> &label_definitions) {
  for (const auto &label : label_definitions) {
    if (label.id == id) {
      return label;
    }
  }
  throw std::runtime_error("No label found by id " + id);
}


BLRgba32 hex_to_color(std::string color) {
  std::stringstream ss;
  color.erase(0, 1);
  unsigned int x = std::stoul("0x" + color, nullptr, 16);
  return BLRgba32(x >> 8 | (x & 0x000000FF) << 24);
}

inline void from_json(const nlohmann ::json &nlohmann_json_j,
                      ProjectLabel &nlohmann_json_t) {
  nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
  nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
  nlohmann_json_t.color = hex_to_color(nlohmann_json_j["color"]);
  nlohmann_json_j.at("is_empty").get_to(nlohmann_json_t.is_empty);
};

}

#endif // PROJECT_SERIALIZATION_H_
