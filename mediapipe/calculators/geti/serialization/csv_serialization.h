#ifndef CSV_SERIALIZATION_H_
#define CSV_SERIALIZATION_H_

#include "../utils/data_structures.h"
#include <string>
#include <vector>

namespace geti {

inline std::string join(const std::vector<std::string>& list, const std::string delim = ",") {
    if (list.size() == 0){
        return "";
    }
    std::string result = list[0];
    for (size_t i = 1; i < list.size(); i++) {
        result += delim + list[i];
    }
    return result;
}

inline std::vector<std::string> to_csv(const RectanglePrediction& prediction) {
    std::vector<std::string> rows = {};

    for (auto& label: prediction.labels) {
        std::vector<std::string> columns = {
            prediction.labels[0].label.label,
            prediction.labels[0].label.label_id,
            "rectangle",
            std::to_string(prediction.labels[0].probability),
            std::to_string(prediction.shape.x),
            std::to_string(prediction.shape.y),
            std::to_string(prediction.shape.width),
            std::to_string(prediction.shape.height),
        };

        rows.push_back(join(columns));
    }

    return rows;
}

inline std::vector<std::string> to_csv(const RotatedRectanglePrediction& prediction) {
    std::vector<std::string> rows = {};

    for (auto& label: prediction.labels) {
        std::vector<std::string> columns = {
            prediction.labels[0].label.label,
            prediction.labels[0].label.label_id,
            std::to_string(prediction.labels[0].probability),
            "rotated rectangle",
            std::to_string(prediction.shape.angle),
            std::to_string(prediction.shape.center.x),
            std::to_string(prediction.shape.center.y),
            std::to_string(prediction.shape.size.width),
            std::to_string(prediction.shape.size.height),
        };

        rows.push_back(join(columns));
    }

    return rows;
}

inline std::vector<std::string> to_csv(const PolygonPrediction& prediction) {
    std::vector<std::string> rows = {};

    for (auto& label: prediction.labels) {
        std::vector<std::string> columns = {
            prediction.labels[0].label.label,
            prediction.labels[0].label.label_id,
            std::to_string(prediction.labels[0].probability),
            "polygon",
        };

        for (auto& point: prediction.shape) {
            columns.push_back(std::to_string(point.x));
            columns.push_back(std::to_string(point.y));
        }

        rows.push_back(join(columns));
    }

    return rows;
}

inline std::string csv_serialize(const InferenceResult& inferenceResult) {
    std::vector<std::string> rows = {};

    for (auto& rectangle: inferenceResult.rectangles ){
        auto output = to_csv(rectangle);
        rows.insert(rows.end(), output.begin(), output.end());
    }

    for (auto& rectangle: inferenceResult.rotated_rectangles ){
        auto output = to_csv(rectangle);
        rows.insert(rows.end(), output.begin(), output.end());
    }

    for (auto& polygons: inferenceResult.polygons ){
        auto output = to_csv(polygons);
        rows.insert(rows.end(), output.begin(), output.end());
    }

    return join(rows, "\n");
}

}

#endif // CSV_SERIALIZATION_H_
