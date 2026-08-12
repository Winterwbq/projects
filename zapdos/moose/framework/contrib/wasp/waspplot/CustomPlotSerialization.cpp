// CustomPlotSerialization.cpp
#include "waspplot/CustomPlotSerialization.h"

namespace wasp
{

DataObject serializeColor(const CustomPlot::Color& color)
{
    DataObject obj;
    obj["red"]   = color.red();
    obj["green"] = color.green();
    obj["blue"]  = color.blue();
    obj["alpha"] = color.alpha();
    return obj;
}

DataObject serializeFont(const CustomPlot::Font& font)
{
    DataObject obj;
    obj["family"]    = font.family();
    obj["pointsize"] = font.pointsize();
    obj["bold"]      = font.bold();
    obj["italic"]    = font.italic();
    obj["underline"] = font.underline();
    obj["strikeout"] = font.strikeout();
    return obj;
}

DataObject serializeTitle(const CustomPlot::Title& title)
{
    DataObject obj;
    obj["text"]    = title.text();
    obj["visible"] = title.visible();
    obj["font"]    = serializeFont(title.font());
    obj["color"]   = serializeColor(title.color());
    return obj;
}

DataObject serializeLegend(const CustomPlot::Legend& legend)
{
    DataObject obj;
    obj["visible"] = legend.visible();
    obj["font"]    = serializeFont(legend.font());
    return obj;
}
DataObject serializeBrush(const CustomPlot::Brush& brush)
{
    DataObject obj;
    obj["style"] = CustomPlot::brushStyle(brush.style());
    obj["color"] = serializeColor(brush.color());
    return obj;
}
DataObject serializePen(const CustomPlot::Pen& pen)
{
    DataObject obj;
    obj["width"] = pen.width();
    obj["style"] = CustomPlot::penStyle(pen.style());
    obj["capStyle"] = CustomPlot::penCapStyle(pen.capStyle());
    obj["joinStyle"] = CustomPlot::penJoinStyle(pen.joinStyle());
    obj["brush"] = serializeBrush(pen.brush());
    return obj;
}

DataObject serializeGrid(const CustomPlot::Grid& grid)
{
    DataObject obj;
    obj["visible"] = grid.visible();
    obj["subGridVisible"] = grid.subGridVisible();
    obj["pen"] = serializePen(grid.pen());
    obj["subGridPen"] = serializePen(grid.subGridPen());
    return obj;
}

DataObject serializeAxis(const CustomPlot::Axis& axis)
{
    DataObject obj;
    obj["label"]         = axis.label();
    obj["visible"]       = axis.visible();
    obj["scaleType"]     = CustomPlot::scaleType(axis.scaleType());
    obj["labelType"]     = CustomPlot::labelType(axis.labelType());
    obj["rangeMin"]      = axis.rangeMin();
    obj["rangeMax"]      = axis.rangeMax();
    obj["grid"]          = serializeGrid(axis.grid());
    obj["labelColor"]    = serializeColor(axis.labelColor());
    obj["labelFont"]     = serializeFont(axis.labelFont());
    obj["tickLabelFont"] = serializeFont(axis.tickLabelFont());
    return obj;
}

DataObject serializeSeries(const std::shared_ptr<CustomPlot::Series>& series)
{
    DataObject obj;
    obj["name"]      = series->name();
    obj["keyAxis"]   = series->keyAxis();
    obj["valueAxis"] = series->valueAxis();

    DataArray keys, values;
    for (double k : series->keys())
        keys.push_back(k);
    for (double v : series->values())
        values.push_back(v);
    obj["keys"]   = keys;
    obj["values"] = values;
    obj["pen"]    = serializePen(series->pen());
    obj["selectedbrush"] = serializeBrush(series->selectedBrush());
    obj["selectedpen"] = serializePen(series->selectedPen());

    if (auto graph = std::dynamic_pointer_cast<CustomPlot::Graph>(series))
    {
        obj["type"]         = "Graph";
        obj["areaOpacity"]  = graph->areaOpacity();
        obj["errorType"]    = CustomPlot::errorType(graph->errorType());
        obj["lineStyle"]    = CustomPlot::lineStyle(graph->lineStyle());
        obj["lineWeight"]   = graph->lineWeight();
        obj["scatterShape"] = CustomPlot::scatterShape(graph->scatterShape());
        obj["scatterSize"]  = graph->scatterSize();

        DataArray klow, khigh, vlow, vhigh;
        for (double d : graph->keysUncertaintyLow())
            klow.push_back(d);
        for (double d : graph->keysUncertaintyHigh())
            khigh.push_back(d);
        for (double d : graph->valuesUncertaintyLow())
            vlow.push_back(d);
        for (double d : graph->valuesUncertaintyHigh())
            vhigh.push_back(d);

        obj["keysUncertaintyLow"]    = klow;
        obj["keysUncertaintyHigh"]   = khigh;
        obj["valuesUncertaintyLow"]  = vlow;
        obj["valuesUncertaintyHigh"] = vhigh;
    }
    else if (auto bar = std::dynamic_pointer_cast<CustomPlot::Bar>(series))
    {
        obj["type"] = "Bar";
    }
    else if (auto cmap =
                 std::dynamic_pointer_cast<CustomPlot::ColorMap>(series))
    {
        obj["type"]     = "ColorMap";
        obj["gradient"] = CustomPlot::gradientPreset(cmap->gradient());

        const auto& matrix = cmap->data();
        DataArray   outer;
        for (const auto& row : matrix)
        {
            DataArray rowArray;
            for (double val : row)
                rowArray.push_back(val);
            outer.push_back(rowArray);
        }
        obj["data"] = outer;
    }
    else
    {
        obj["type"] = "Unknown";
    }

    return obj;
}

DataObject serializeCustomPlot(const CustomPlot& plot)
{
    DataObject obj;
    obj["title"]  = serializeTitle(plot.title());
    obj["legend"] = serializeLegend(plot.legend());
    obj["x1Axis"] = serializeAxis(plot.x1Axis());
    obj["x2Axis"] = serializeAxis(plot.x2Axis());
    obj["y1Axis"] = serializeAxis(plot.y1Axis());
    obj["y2Axis"] = serializeAxis(plot.y2Axis());

    DataArray seriesArray;
    for (const auto& s : plot.series())
    {
        seriesArray.push_back(serializeSeries(s));
    }
    obj["series"] = seriesArray;

    return obj;
}

CustomPlot::Color deserializeColor(const DataObject& obj)
{
    CustomPlot::Color color;
    if (obj.contains("red"))
        color.red(obj["red"].to_int());
    if (obj.contains("green"))
        color.green(obj["green"].to_int());
    if (obj.contains("blue"))
        color.blue(obj["blue"].to_int());
    if (obj.contains("alpha"))
        color.alpha(obj["alpha"].to_int());
    return color;
}

CustomPlot::Font deserializeFont(const DataObject& obj)
{
    CustomPlot::Font font;
    if (obj.contains("family"))
        font.family(obj["family"].to_string());
    if (obj.contains("pointsize"))
        font.pointsize(obj["pointsize"].to_int());
    if (obj.contains("bold"))
        font.bold(obj["bold"].to_bool());
    if (obj.contains("italic"))
        font.italic(obj["italic"].to_bool());
    if (obj.contains("underline"))
        font.underline(obj["underline"].to_bool());
    if (obj.contains("strikeout"))
        font.strikeout(obj["strikeout"].to_bool());
    return font;
}

CustomPlot::Title deserializeTitle(const DataObject& obj)
{
    CustomPlot::Title title;
    if (obj.contains("text"))
        title.text(obj["text"].to_string());
    if (obj.contains("visible"))
        title.visible(obj["visible"].to_bool());
    if (obj.contains("font"))
        title.font() = deserializeFont(*obj["font"].to_object());
    if (obj.contains("color"))
        title.color() = deserializeColor(*obj["color"].to_object());
    return title;
}

CustomPlot::Legend deserializeLegend(const DataObject& obj)
{
    CustomPlot::Legend legend;
    if (obj.contains("visible"))
        legend.visible(obj["visible"].to_bool());
    if (obj.contains("font"))
        legend.font() = deserializeFont(*obj["font"].to_object());
    return legend;
}

CustomPlot::Brush deserializeBrush(const DataObject& obj)
{
    CustomPlot::Brush brush;
    if (obj.contains("style"))
        brush.style(CustomPlot::brushStyle(obj["style"].to_string()));
    if (obj.contains("color"))
        brush.color() = deserializeColor(*obj["color"].to_object());
    return brush;
}

CustomPlot::Pen deserializePen(const DataObject& obj)
{
    CustomPlot::Pen pen;
    if (obj.contains("width"))
        pen.width(obj["width"].to_int());
    if (obj.contains("style"))
        pen.style(CustomPlot::penStyle(obj["style"].to_string()));
    if (obj.contains("capStyle"))
        pen.capStyle(CustomPlot::penCapStyle(obj["capStyle"].to_string()));
    if (obj.contains("joinStyle"))
        pen.joinStyle(CustomPlot::penJoinStyle(obj["joinStyle"].to_string()));
    if (obj.contains("brush"))
        pen.brush() = deserializeBrush(*obj["brush"].to_object());
    return pen;
}

CustomPlot::Grid deserializeGrid(const DataObject& obj)
{
    CustomPlot::Grid grid;
    if (obj.contains("visible"))
        grid.visible(obj["visible"].to_bool());
    if (obj.contains("subGridVisible"))
        grid.subGridVisible(obj["subGridVisible"].to_bool());
    if (obj.contains("pen"))
        grid.pen() = deserializePen(*obj["pen"].to_object());
    if (obj.contains("subGridPen"))
        grid.subGridPen() = deserializePen(*obj["subGridPen"].to_object());
    return grid;
}

CustomPlot::Axis deserializeAxis(const DataObject& obj)
{
    CustomPlot::Axis axis;
    if (obj.contains("label"))
        axis.label(obj["label"].to_string());
    if (obj.contains("visible"))
        axis.visible(obj["visible"].to_bool());
    if (obj.contains("scaleType"))
        axis.scaleType(CustomPlot::scaleType(obj["scaleType"].to_string()));
    if (obj.contains("labelType"))
        axis.labelType(CustomPlot::labelType(obj["labelType"].to_string()));
    if (obj.contains("rangeMin"))
        axis.rangeMin(obj["rangeMin"].to_double());
    if (obj.contains("rangeMax"))
        axis.rangeMax(obj["rangeMax"].to_double());
    if (obj.contains("grid"))
        axis.grid() = deserializeGrid(*obj["grid"].to_object());
    if (obj.contains("labelColor"))
        axis.labelColor() = deserializeColor(*obj["labelColor"].to_object());
    if (obj.contains("labelFont"))
        axis.labelFont() = deserializeFont(*obj["labelFont"].to_object());
    if (obj.contains("tickLabelFont"))
        axis.tickLabelFont() =
            deserializeFont(*obj["tickLabelFont"].to_object());
    return axis;
}

std::shared_ptr<CustomPlot::Series> deserializeSeries(const DataObject& obj)
{
    const std::string                   type = obj["type"].to_string();
    std::shared_ptr<CustomPlot::Series> series;

    if (type == "Graph")
    {
        auto graph = std::make_shared<CustomPlot::Graph>();
        if (obj.contains("areaOpacity"))
            graph->areaOpacity(obj["areaOpacity"].to_int());
        if (obj.contains("errorType"))
            graph->errorType(
                CustomPlot::errorType(obj["errorType"].to_string()));
        if (obj.contains("lineStyle"))
            graph->lineStyle(
                CustomPlot::lineStyle(obj["lineStyle"].to_string()));
        if (obj.contains("lineWeight"))
            graph->lineWeight(obj["lineWeight"].to_int());
        if (obj.contains("scatterShape"))
            graph->scatterShape(
                CustomPlot::scatterShape(obj["scatterShape"].to_string()));
        if (obj.contains("scatterSize"))
            graph->scatterSize(obj["scatterSize"].to_double());
        if (obj.contains("keysUncertaintyLow"))
            for (auto& d : *obj["keysUncertaintyLow"].to_array())
                graph->keysUncertaintyLow().push_back(d.to_double());
        if (obj.contains("keysUncertaintyHigh"))
            for (auto& d : *obj["keysUncertaintyHigh"].to_array())
                graph->keysUncertaintyHigh().push_back(d.to_double());
        if (obj.contains("valuesUncertaintyLow"))
            for (auto& d : *obj["valuesUncertaintyLow"].to_array())
                graph->valuesUncertaintyLow().push_back(d.to_double());
        if (obj.contains("valuesUncertaintyHigh"))
            for (auto& d : *obj["valuesUncertaintyHigh"].to_array())
                graph->valuesUncertaintyHigh().push_back(d.to_double());

        series = graph;
    }
    else if (type == "Bar")
    {
        series = std::make_shared<CustomPlot::Bar>();
    }
    else if (type == "ColorMap")
    {
        auto cmap = std::make_shared<CustomPlot::ColorMap>();
        if (obj.contains("gradient"))
            cmap->gradient(
                CustomPlot::gradientPreset(obj["gradient"].to_string()));

        std::vector<std::vector<double>> matrix;
        if (obj.contains("data"))
            for (auto& rowVal : *obj["data"].to_array())
            {
                std::vector<double> row;
                for (auto& colVal : *rowVal.to_array())
                    row.push_back(colVal.to_double());
                matrix.push_back(row);
            }
        cmap->data(matrix);

        series = cmap;
    }
    else
    {
        return nullptr;
    }

    if (obj.contains("name"))
        series->name(obj["name"].to_string());
    if (obj.contains("pen"))
        series->pen() = deserializePen(*obj["pen"].to_object());
    if (obj.contains("selectedbrush"))
        series->selectedBrush() = deserializeBrush(*obj["selectedbrush"].to_object());
    if (obj.contains("selectedpen"))
        series->pen() = deserializePen(*obj["selectedpen"].to_object());
    if (obj.contains("keyAxis"))
        series->keyAxis(obj["keyAxis"].to_int());
    if (obj.contains("valueAxis"))
        series->valueAxis(obj["valueAxis"].to_int());
    if (obj.contains("keys"))
        for (auto& k : *obj["keys"].to_array())
            series->keys().push_back(k.to_double());
    if (obj.contains("values"))
        for (auto& v : *obj["values"].to_array())
            series->values().push_back(v.to_double());

    return series;
}

std::shared_ptr<CustomPlot> deserializeCustomPlot(const DataObject& obj)
{
    std::shared_ptr<CustomPlot> plot = std::make_shared<CustomPlot>();
    if (obj.contains("title"))
        plot->title() = deserializeTitle(*obj["title"].to_object());
    if (obj.contains("legend"))
        plot->legend() = deserializeLegend(*obj["legend"].to_object());
    if (obj.contains("x1Axis"))
        plot->x1Axis() = deserializeAxis(*obj["x1Axis"].to_object());
    if (obj.contains("x2Axis"))
        plot->x2Axis() = deserializeAxis(*obj["x2Axis"].to_object());
    if (obj.contains("y1Axis"))
        plot->y1Axis() = deserializeAxis(*obj["y1Axis"].to_object());
    if (obj.contains("y2Axis"))
        plot->y2Axis() = deserializeAxis(*obj["y2Axis"].to_object());

    if (obj.contains("series"))
        for (const auto& s : *obj["series"].to_array())
        {
            auto series = deserializeSeries(*s.to_object());
            if (series)
                plot->series().push_back(series);
        }
    return plot;
}

}  // namespace wasp