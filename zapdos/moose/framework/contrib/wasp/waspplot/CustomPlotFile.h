#ifndef WASPPLOT_CUSTOMPLOTFILE_H
#define WASPPLOT_CUSTOMPLOTFILE_H

#include "waspcore/decl.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * @file CustomPlotFile.h
 * @brief Defines the wasp::CustomPlot class and supporting types for plot
 * representation and serialization.
 *
 * This header file provides the definition for the `wasp::CustomPlot` class,
 * which encapsulates all elements of a customizable 2D plot in a
 * platform-agnostic and Qt-inspired structure. It includes support for
 * rendering styles, axes, series types, legend, grid, and graphical attributes
 * such as pens, brushes, fonts, and color maps.
 *
 * The design is modular, leveraging Qt-style enums and concepts (e.g., QPen,
 * QBrush, QColor) to allow for flexible translation to plotting frontends like
 * QCustomPlot, or others with similar feature sets.
 *
 * ## Core Components:
 * - **Enums**: Define rendering options for brushes, pens, gradients, lines,
 * axes, etc.
 * - **Color/Brush/Font/Pen**: Represent graphical styling elements, similar to
 * their Qt counterparts.
 * - **Axis & Grid**: Represent plot axes (x/y, top/bottom) and configurable
 * grid settings.
 * - **Series**: Base class for plotted data sets. Derived classes include:
 *     - `Bar`: Histogram representation.
 *     - `Graph`: Line or scatter series with error bars and uncertainty.
 *     - `ColorMap`: 2D matrix of values with a gradient mapping.
 * - **Legend & Title**: Metadata and labels for the plot.
 *
 * ## Features:
 * - Full styling control of all plot components via C++ classes (no direct Qt
 * dependency).
 * - Enumeration mapping between string labels and enum values for easy
 * serialization/deserialization.
 * - Support for uncertainty, multiple axes, and rich formatting of axis labels
 * and ticks.
 * - Plot layout consists of four axes (X1, X2, Y1, Y2), title, legend, and a
 * series collection.
 *
 * ## For example usage see tests in ./test directory
 *
 * ## Notes:
 * - This API is designed for backend-agnostic use cases and require adapters
 * for actual rendering.
 */

namespace wasp
{

/**
 * @brief The CustomPlot class provides a full representation of a customizable
 * 2D plot.
 *
 * This class acts as a container for axes, data series, styling options, and
 * plot metadata such as title and legend. It is inspired by the QCustomPlot
 * layout and follows a modular structure for complete control over plot
 * presentation and export.
 */
class WASP_PUBLIC CustomPlot
{
  public:
    /**
     * @brief Enum for brush fill patterns (based on Qt::BrushStyle).
     */
    enum BrushStyle
    {
        NoBrush,
        SolidPattern,
        Dense1Pattern,
        Dense2Pattern,
        Dense3Pattern,
        Dense4Pattern,
        Dense5Pattern,
        Dense6Pattern,
        Dense7Pattern,
        HorPattern,
        VerPattern,
        CrossPattern,
        BDiagPattern,
        FDiagPattern,
        DiagCrossPattern
    };

    /**
     * @brief Enum for error bar types in graphs.
     */
    enum ErrorType
    {
        etNone,   ///< No error bars
        etKey,    ///< Error in key (X) values
        etValue,  ///< Error in value (Y) values
        etBoth    ///< Error in both X and Y
    };

    /**
     * @brief Enum for predefined color gradient styles.
     */
    enum GradientPreset
    {
        gpGrayscale,
        gpHot,
        gpCold,
        gpNight,
        gpCandy,
        gpGeography,
        gpIon,
        gpThermal,
        gpPolar,
        gpSpectrum,
        gpJet,
        gpHues
    };

    /**
     * @brief Enum for axis label type formatting.
     */
    enum LabelType
    {
        ltNumber,   ///< Numeric formatting
        ltDateTime  ///< Date/time formatting
    };

    /**
     * @brief Enum for line style rendering (based on QCPGraph::LineStyle).
     */
    enum LineStyle
    {
        lsNone,
        lsLine,
        lsStepLeft,
        lsStepRight,
        lsStepCenter,
        lsImpulse
    };

    /**
     * @brief Enum for pen cap styles (based on Qt::PenCapStyle).
     */
    enum PenCapStyle
    {
        FlatCap   = 0x00,
        SquareCap = 0x10,
        RoundCap  = 0x20
    };

    /**
     * @brief Enum for pen join styles (based on Qt::PenJoinStyle).
     */
    enum PenJoinStyle
    {
        MiterJoin = 0x00,
        BevelJoin = 0x40,
        RoundJoin = 0x80
    };

    /**
     * @brief Enum for pen styles (based on Qt::PenStyle).
     */
    enum PenStyle
    {
        NoPen,
        SolidLine,
        DashLine,
        DotLine,
        DashDotLine,
        DashDotDotLine
    };

    /**
     * @brief Enum for axis scale type.
     */
    enum ScaleType
    {
        stLinear,
        stLogarithmic
    };

    /**
     * @brief Enum for scatter marker shapes.
     */
    enum ScatterShape
    {
        ssNone,
        ssDot,
        ssCross,
        ssPlus,
        ssCircle,
        ssDisc,
        ssSquare,
        ssDiamond,
        ssStar,
        ssTriangle,
        ssTriangleInverted,
        ssCrossSquare,
        ssPlusSquare,
        ssCrossCircle,
        ssPlusCircle,
        ssPeace
    };

    static BrushStyle     brushStyle(const std::string& s);
    static std::string    brushStyle(BrushStyle s);
    static ErrorType      errorType(const std::string& t);
    static std::string    errorType(ErrorType t);
    static GradientPreset gradientPreset(const std::string& p);
    static std::string    gradientPreset(GradientPreset p);
    static LabelType      labelType(const std::string& s);
    static std::string    labelType(LabelType t);
    static LineStyle      lineStyle(const std::string& s);
    static std::string    lineStyle(LineStyle s);
    static PenCapStyle    penCapStyle(const std::string& s);
    static std::string    penCapStyle(PenCapStyle s);
    static PenJoinStyle   penJoinStyle(const std::string& s);
    static std::string    penJoinStyle(PenJoinStyle s);
    static PenStyle       penStyle(const std::string& s);
    static std::string    penStyle(PenStyle s);
    static ScaleType      scaleType(const std::string& s);
    static std::string    scaleType(ScaleType s);
    static ScatterShape   scatterShape(const std::string& s);
    static std::string    scatterShape(ScatterShape s);

    /**
     * @brief Represents an RGBA color, similar to QColor.
     */
    class WASP_PUBLIC Color
    {
      public:
        Color();
        Color(const Color& orig);
        Color& operator=(const Color& c);

        int  alpha() const;
        void alpha(int a);

        int  blue() const;
        void blue(int b);

        int  green() const;
        void green(int g);

        bool isValid() const;

        int  red() const;
        void red(int r);

      private:
        int mAlpha;
        int mBlue;
        int mGreen;
        int mRed;
    };

    /**
     * @brief Represents a fill brush (pattern and color), similar to QBrush.
     */
    class WASP_PUBLIC Brush
    {
      public:
        Brush();
        Brush(const Brush& b);
        Brush& operator=(const Brush& b);

        const Color& color() const;
        Color&       color();

        BrushStyle style() const;
        void       style(BrushStyle s);

      private:
        Color      mColor;
        BrushStyle mStyle;
    };

    /**
     * @brief Represents a font for text styling (labels, legend, etc.).
     */
    class WASP_PUBLIC Font
    {
      public:
        Font();
        Font(const Font& f);
        Font& operator=(const Font& f);

        bool bold() const;
        void bold(bool b);

        const std::string& family() const;
        void               family(const std::string& f);

        bool italic() const;
        void italic(bool i);

        int  pointsize() const;
        void pointsize(int p);

        bool strikeout() const;
        void strikeout(bool s);

        bool underline() const;
        void underline(bool u);

      private:
        bool        mBold;
        std::string mFamily;
        bool        mItalic;
        int         mPointSize;
        bool        mStrikeOut;
        bool        mUnderline;
    };

    /**
     * @brief Represents a pen (stroke color, style, width), similar to QPen.
     */
    class WASP_PUBLIC Pen
    {
      public:
        Pen();
        Pen(const Pen& p);
        Pen& operator=(const Pen& p);

        const Brush& brush() const;
        Brush&       brush();

        PenCapStyle capStyle() const;
        void        capStyle(PenCapStyle pcs);

        PenJoinStyle joinStyle() const;
        void         joinStyle(PenJoinStyle pjs);

        PenStyle style() const;
        void     style(PenStyle ps);

        int  width() const;
        void width(int w);

      private:
        Brush        mBrush;
        PenCapStyle  mCapStyle;
        PenJoinStyle mJoinStyle;
        PenStyle     mStyle;
        int          mWidth;
    };

    /**
     * @brief Represents grid lines for an axis.
     */
    class WASP_PUBLIC Grid
    {
      public:
        Grid();
        Grid(const Grid& g);
        Grid& operator=(const Grid& g);

        const Pen& pen() const;
        Pen&       pen();

        const Pen& subGridPen() const;
        Pen&       subGridPen();

        bool subGridVisible() const;
        void subGridVisible(bool v);

        bool visible() const;
        void visible(bool v);

      private:
        Pen  mPen;
        Pen  mSubGridPen;
        bool mSubGridVisible;
        bool mVisible;
    };

    /**
     * @brief Represents a plot axis with labeling, ticks, scaling, and grid.
     */
    class WASP_PUBLIC Axis
    {
      public:
        typedef std::vector<std::pair<std::string, double>> tick_values;

        Axis();
        Axis(const Axis& a);
        Axis& operator=(const Axis& a);

        const Grid& grid() const;
        Grid&       grid();

        bool hasRangeMax() const;
        bool hasRangeMin() const;

        bool isNumberFormatEB() const;
        bool isNumberFormatEBC() const;
        bool isNumberFormatG() const;
        bool isNumberFormatGB() const;

        const std::string& label() const;
        void               label(const std::string& l);

        const Color& labelColor() const;
        Color&       labelColor();

        const std::string& labelDateFormat() const;
        void               labelDateFormat(const std::string& ldf);

        const Font& labelFont() const;
        Font&       labelFont();

        double labelPrecision() const;
        void   labelPrecision(double lp);

        LabelType labelType() const;
        void      labelType(LabelType lt);

        const std::string& numberFormat() const;
        void               numberFormat(const std::string& nf);

        double rangeMax() const;
        void   rangeMax(double d);

        double rangeMin() const;
        void   rangeMin(double d);

        ScaleType scaleType() const;
        void      scaleType(ScaleType st);

        const Font& tickLabelFont() const;
        Font&       tickLabelFont();

        double tickLabelRotation() const;
        void   tickLabelRotation(double tlr);

        const tick_values& ticks() const;
        tick_values&       ticks();

        bool visible() const;
        void visible(bool v);

      private:
        Grid        mGrid;
        std::string mLabel;
        Color       mLabelColor;
        std::string mLabelDateFormat;
        Font        mLabelFont;
        double      mLabelPrecision;
        LabelType   mLabelType;
        std::string mNumberFormat;
        double      mRangeMax;
        double      mRangeMin;
        ScaleType   mScaleType;
        Font        mTickLabelFont;
        double      mTickLabelRotation;
        tick_values mTicks;
        bool        mVisible;
    };

    /**
     * @brief Base class for all series (data) types: Bar, Graph, ColorMap.
     */
    class WASP_PUBLIC Series
    {
      public:
        using Collection = std::vector<std::shared_ptr<Series>>;

        Series(const Series& s);
        virtual ~Series();

        const Brush& brush() const;
        Brush&       brush();

        int  keyAxis() const;
        void keyAxis(int a);

        const std::vector<double>& keys() const;
        std::vector<double>&       keys();

        const std::string& name() const;
        void               name(const std::string& n);

        const Pen& pen() const;
        Pen&       pen();

        const Brush& selectedBrush() const;
        Brush&       selectedBrush();

        const Pen& selectedPen() const;
        Pen&       selectedPen();

        int  valueAxis() const;
        void valueAxis(int a);

        const std::vector<double>& values() const;
        std::vector<double>&       values();

      protected:
        Series();

        Brush               mBrush;
        int                 mKeyAxis;
        std::vector<double> mKeys;
        std::string         mName;
        Pen                 mPen;
        Brush               mSelectedBrush;
        Pen                 mSelectedPen;
        int                 mValueAxis;
        std::vector<double> mValues;
    };

    /**
     * @brief Represents a bar chart series.
     */
    class WASP_PUBLIC Bar : public Series
    {
      public:
        Bar();
        Bar(const Bar& b);
    };

    /**
     * @brief Represents a 2D color map plot (like a heatmap).
     */
    class WASP_PUBLIC ColorMap : public Series
    {
      public:
        ColorMap();
        ColorMap(const ColorMap& cm);

        std::vector<std::vector<double>>&       data();
        const std::vector<std::vector<double>>& data() const;
        void data(std::vector<std::vector<double>>& d);

        GradientPreset gradient() const;
        void           gradient(GradientPreset p);

      private:
        std::vector<std::vector<double>> mData;
        GradientPreset                   mGradient;
    };

    /**
     * @brief Represents a graph series (line, scatter, or area).
     */
    class WASP_PUBLIC Graph : public Series
    {
      public:
        Graph();
        Graph(const Graph& g);

        int  areaOpacity() const;
        void areaOpacity(int ao);

        ErrorType errorType() const;
        void      errorType(ErrorType et);

        const std::vector<double>& keysUncertaintyLow() const;
        std::vector<double>&       keysUncertaintyLow();

        const std::vector<double>& keysUncertaintyHigh() const;
        std::vector<double>&       keysUncertaintyHigh();

        LineStyle lineStyle() const;
        void      lineStyle(LineStyle ls);
        size_t    lineWeight() const;
        void      lineWeight(size_t lineWeight);

        ScatterShape scatterShape() const;
        void         scatterShape(ScatterShape ss);

        double scatterSize() const;
        void   scatterSize(double ss);

        const std::vector<double>& valuesUncertaintyLow() const;
        std::vector<double>&       valuesUncertaintyLow();

        const std::vector<double>& valuesUncertaintyHigh() const;
        std::vector<double>&       valuesUncertaintyHigh();

      private:
        int                 mAreaOpacity;
        ErrorType           mErrorType;
        std::vector<double> mKeysUncertaintyHigh;
        std::vector<double> mKeysUncertaintyLow;
        LineStyle           mLineStyle;
        size_t              mLineWeight;
        ScatterShape        mScatterShape;
        double              mScatterSize;
        std::vector<double> mValuesUncertaintyHigh;
        std::vector<double> mValuesUncertaintyLow;
    };

    /**
     * @brief Represents the plot legend.
     */
    class WASP_PUBLIC Legend
    {
      public:
        Legend();
        Legend(const Legend& l);
        Legend& operator=(const Legend& l);

        const Font& font() const;
        Font&       font();

        bool visible() const;
        void visible(bool v);

      private:
        Font mFont;
        bool mVisible;
    };

    /**
     * @brief Represents the main title of the plot.
     */
    class WASP_PUBLIC Title
    {
      public:
        Title();
        Title(const Title& t);
        Title& operator=(const Title& t);

        const Color& color() const;
        Color&       color();

        const Font& font() const;
        Font&       font();

        const std::string& text() const;
        void               text(const std::string& text);

        bool visible() const;
        void visible(bool v);

      private:
        Color       mColor;
        Font        mFont;
        std::string mText;
        bool        mVisible;
    };

    CustomPlot();
    CustomPlot(const CustomPlot& cpi);
    ~CustomPlot();

    const Legend& legend() const;
    Legend&       legend();

    void reset();

    const Series::Collection& series() const;
    Series::Collection&       series();

    const Title& title() const;
    Title&       title();

    const Axis& x1Axis() const;
    Axis&       x1Axis();

    const Axis& x2Axis() const;
    Axis&       x2Axis();

    const Axis& y1Axis() const;
    Axis&       y1Axis();

    const Axis& y2Axis() const;
    Axis&       y2Axis();

  private:
    Legend             mLegend;
    Series::Collection mSeries;
    Title              mTitle;
    Axis               mX1Axis;
    Axis               mX2Axis;
    Axis               mY1Axis;
    Axis               mY2Axis;
};

}  // namespace wasp

#endif  // WASPPLOT_CUSTOMPLOTFILE_H
