#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <blend2d.h>
#include "mediapipe/calculators/geti/serialization/project_serialization.h"
#include "data_structures.h"

struct DrawOptions {
  double strokeWidth;
  double opacity;
  double fontSize;
};

inline double draw_label(BLContext &ctx, const BLFont& font, const BLRgba32& color, const std::string label_text, BLPoint bl) {
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


    float padding = 4.0f;
    float height = fontMetrics.ascent + fontMetrics.descent + padding * 2;
    float width = metrics.boundingBox.x1 - metrics.boundingBox.x0 + padding * 2 ;

    if (bl.y - height < 0) {
      bl.y = height;
    }


    BLRect textArea{ bl.x - 1, bl.y - height, width , height}; //1 for border?


    ctx.fillRect(textArea, color);

    float luminance = (0.299f*color.r() + 0.587f*color.g() + 0.114f*color.b());
    if (luminance < 128) {
      ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
    } else {
      ctx.setFillStyle(BLRgba32(0xFF000000));
    }
    ctx.fillUtf8Text(BLPoint(bl.x + padding - 1, bl.y - padding - fontMetrics.descent), font, label_text.c_str());

    return textArea.w;
}

inline void draw_rect_prediction(BLContext &ctx, const BLFont& font, const geti::RectanglePrediction &prediction,
                     const std::vector<geti::ProjectLabel> &label_definitions, const DrawOptions &drawOptions) {

  BLRect rect{(double)prediction.shape.x, (double)prediction.shape.y, (double)prediction.shape.width,
              (double)prediction.shape.height};
  bool rect_drawn = false;
  bool fullImageRect = ctx.targetWidth() == rect.w && ctx.targetHeight() == rect.h;
  double offset = 0.0;
  for (auto &label : prediction.labels) {
    const auto &label_info = get_label_by_id(label.label.label_id, label_definitions);

    if (!fullImageRect && !rect_drawn) {
      auto fill_color = label_info.color;
      fill_color.setA(255 * drawOptions.opacity);
      ctx.fillRect(rect, fill_color);
      auto border_color = label_info.color;
      ctx.strokeRect(rect, label_info.color);
      rect_drawn = true;
    }

    std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    offset += draw_label(ctx, font, label_info.color, label_text, BLPoint(rect.x + offset, rect.y));
  }
}

inline void draw_polygon_prediction(BLContext &ctx, const BLFont& font, const geti::PolygonPrediction &prediction,
                     const std::vector<geti::ProjectLabel> &label_definitions, const DrawOptions &drawOptions) {
  bool shapeDrawn = false;
  BLBox box;
  for (const auto& label: prediction.labels) {
    const auto &label_info = get_label_by_id(label.label.label_id, label_definitions);

    if (!shapeDrawn) {
      BLArrayView<BLPointI> points;
      BLPath path;
      points.reset((BLPointI*)(prediction.shape.data()), prediction.shape.size());
      path.addPolygon(points);
      path.getBoundingBox(&box);

      auto fill_color = label_info.color;
      fill_color.setA(255 * drawOptions.opacity);
      ctx.setFillStyle(fill_color);
      ctx.setStrokeStyle(label_info.color);
      ctx.fillPath(path);
      ctx.strokePath(path);
      shapeDrawn = true;
    }

    BLPoint center(box.x0 + (box.x1 - box.x0) / 2, box.y0 + (box.y1 - box.y0) / 2);
    BLPoint centerTop(center.x, box.y0 - 30);
    ctx.strokeLine(center, centerTop);

    std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    draw_label(ctx, font, label_info.color, label_text, centerTop);
  }
}

inline void draw_rotated_rect_prediction(BLContext &ctx, const BLFont& font, const geti::RotatedRectanglePrediction &prediction,
                     const std::vector<geti::ProjectLabel> &label_definitions, const DrawOptions &drawOptions) {
  bool shapeDrawn = false;
  BLBox box;
  for (const auto& label: prediction.labels) {
    const auto &label_info = get_label_by_id(label.label.label_id, label_definitions);

    if (!shapeDrawn) {
      BLArrayView<BLPointI> points;
      BLPath path;
      cv::Point2f vertices[4];
      prediction.shape.points(vertices);
      path.moveTo(vertices[0].x, vertices[0].y);
      for (size_t i = 1; i < 4; i++) {
        path.lineTo(vertices[i].x, vertices[i].y);
      }
      path.getBoundingBox(&box);

      auto fill_color = label_info.color;
      fill_color.setA(255 * drawOptions.opacity);
      ctx.setFillStyle(fill_color);
      ctx.setStrokeStyle(label_info.color);
      ctx.fillPath(path);
      ctx.strokePath(path);
      shapeDrawn = true;
    }

    BLPoint center(box.x0 + (box.x1 - box.x0) / 2, box.y0 + (box.y1 - box.y0) / 2);
    BLPoint centerTop(center.x, box.y0 - 30);
    ctx.strokeLine(center, centerTop);

    std::string label_text = label_info.name + " " + std::to_string((int) round(label.probability * 100)) + "%";
    draw_label(ctx, font, label_info.color, label_text, centerTop);
  }
}



#endif // OVERLAY_H_
