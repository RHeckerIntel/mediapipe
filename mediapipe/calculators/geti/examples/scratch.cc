#include <iostream>

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/calculators/geti/utils/data_structures.h"
#include "mediapipe/calculators/geti/utils/overlay.h"
#include "mediapipe/calculators/geti/serialization/project_serialization.h"

#include <blend2d.h>

int main() {
    std::cout << "hello world" << std::endl;
    cv::Mat cv_input = cv::imread("/data/cattle.jpg");
    cv::cvtColor(cv_input, cv_input, cv::COLOR_BGR2RGB); // this happens inside the ModelInferRequest
    cv::Mat cv_image;
    cv::cvtColor(cv_input, cv_image, cv::COLOR_RGB2BGRA);
    BLImage img;
    img.createFromData(cv_image.cols, cv_image.rows, BLFormat::BL_FORMAT_XRGB32, cv_image.data, cv_image.step);


    geti::Label label{"653b84c34e88964031d81a48", "label name"};
    geti::RectanglePrediction pred{{geti::LabelResult{0.5, label}}, cv::Rect(763, 906, 359, 239)};

    BLFontFace face;
    const char fontName[] = "/home/rhecker/Projects/blend2d_workspace/app/intelone-text-regular.ttf";
    BLResult result = face.createFromFile(fontName);

    if (result != BL_SUCCESS) {
        printf("Failed to load a font (err=%u)\n", result);
    }

    std::vector<geti::ProjectLabel> label_definitions = geti::read_file("/home/rhecker/Projects/blend2d_workspace/app/project.json")["tasks"][0]["labels"];

    BLFont font;
    font.createFromFace(face, 12.0f);

    BLContext ctx(img);
    ctx.setStrokeWidth(2);
    draw_rect_prediction(ctx, font, pred, label_definitions);

    //img.writeToFile("/data/output.png");


    ctx.end();

    img.writeToFile("/data/output.png");

    std::cout << "done" << std::endl;
    return 0;
}
