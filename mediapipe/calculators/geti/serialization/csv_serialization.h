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
        std::stringstream ss;
        ss.imbue(std::locale("C"));
        ss << label.label.label << ","
           << label.label.label_id << ","
           << label.probability << ","
           << "rectangle" << ","
           << prediction.shape.x << ","
           << prediction.shape.y << ","
           << prediction.shape.width << ","
           << prediction.shape.height;
        std::cout << ss.str() << std::endl;
        rows.push_back(ss.str());
    }

    return rows;
}

inline std::vector<std::string> to_csv(const RotatedRectanglePrediction& prediction) {
    std::vector<std::string> rows = {};

    for (auto& label: prediction.labels) {
        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF8"));
        ss << label.label.label << ","
           << label.label.label_id << ","
           << label.probability << ","
           << "rotated_rectangle" << ","
           << prediction.shape.angle << ","
           << prediction.shape.center.x << ","
           << prediction.shape.center.y << ","
           << prediction.shape.size.width << ","
           << prediction.shape.size.height;
        rows.push_back(ss.str());
    }

    return rows;
}

inline std::vector<std::string> to_csv(const PolygonPrediction& prediction) {
    std::vector<std::string> rows = {};

    for (auto& label: prediction.labels) {
        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF8"));
        ss << label.label.label << ","
           << label.label.label_id << ","
           << label.probability << ","
           << "polygon";

        for (auto& point: prediction.shape) {
            ss << "," << point.x
               << "," << point.y;
        }

        rows.push_back(ss.str());
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

    return join(rows, "\r\n");
}

}

#endif // CSV_SERIALIZATION_H_
