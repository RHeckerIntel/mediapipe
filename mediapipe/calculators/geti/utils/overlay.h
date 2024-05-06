#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <blend2d.h>
#include "mediapipe/calculators/geti/serialization/project_serialization.h"
#include "data_structures.h"

inline double draw_label(BLContext &ctx, const BLFont& font, const BLRgba32& color, const std::string label_text, const BLPoint& bl) {
    //std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    BLGlyphBuffer buffer;
    BLFontMetrics fontMetrics = font.metrics();
    buffer.setUtf8Text(label_text.c_str(), SIZE_MAX);
    font.shape(buffer);

    BLTextMetrics metrics;
    {
      auto result = font.getTextMetrics(buffer, metrics);

      if (result != BL_SUCCESS) {
        printf("Failed to load a font (err=%u)\n", result);
      }
    }


    float height = fontMetrics.ascent + fontMetrics.descent;
    float width = metrics.boundingBox.x1 - metrics.boundingBox.x0;
    float padding = 4.0f;
    BLRect textArea{ bl.x - 1, bl.y - padding * 2 - height, width + padding * 2 , height + padding * 2}; //1 for border?


    ctx.fillRect(textArea, color);
    ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
    ctx.fillUtf8Text(BLPoint(bl.x + padding - 1, bl.y - padding - fontMetrics.descent), font, label_text.c_str());

    return textArea.x + textArea.w;
}

inline void draw_rect_prediction(BLContext &ctx, const BLFont& font, const geti::RectanglePrediction &prediction,
                     const std::vector<geti::ProjectLabel> &label_definitions) {

  BLRect rect{prediction.shape.x, prediction.shape.y, prediction.shape.width,
              prediction.shape.height};
  bool rect_drawn = false;
  for (auto &label : prediction.labels) {
    const auto &label_info = get_label_by_id(label.label.label_id, label_definitions);
    if (!rect_drawn) {
      auto fill_color = label_info.color;
      fill_color.setA(102);
      ctx.fillRect(rect, fill_color);
      auto border_color = label_info.color;
      ctx.strokeRect(rect, label_info.color);
      rect_drawn = true;
    }

    std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    draw_label(ctx, font, label_info.color, label_text, BLPoint(rect.x, rect.y));
  }
}

inline void draw_polygon_prediction(BLContext &ctx, const BLFont& font, const geti::PolygonPrediction &prediction,
                     const std::vector<geti::ProjectLabel> &label_definitions) {
  bool shapeDrawn = false;
  for (const auto& label: prediction.labels) {
    const auto &label_info = get_label_by_id(label.label.label_id, label_definitions);

    BLArrayView<BLPoint> points;
    BLPath path;
    points.reset((BLPoint*)(prediction.shape.data()), prediction.shape.size());
    path.addPolygon(points);
    BLBox box;
    path.getBoundingBox(&box);
    if (!shapeDrawn) {
      auto fill_color = label_info.color;
      fill_color.setA(102);
      ctx.setFillStyle(fill_color);
      ctx.setStrokeStyle(label_info.color);
      ctx.fillPath(path);
      ctx.strokePath(path);
    }

    BLPoint center(box.x0 + (box.x1 - box.x0) / 2, box.y0 + (box.y1 - box.y0) / 2);
    BLPoint centerTop(center.x, box.y0 - 30);
    //BLRect rect(box.x0, box.y0, box.x1 - box.x0, box.y1 - box.y0);
    //ctx.strokeRect(rect);
    ctx.strokeLine(center, centerTop);

    std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    draw_label(ctx, font, label_info.color, label_text, centerTop);
  }


}



#endif // OVERLAY_H_
