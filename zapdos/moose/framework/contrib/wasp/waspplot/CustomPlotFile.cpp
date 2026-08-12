#include "waspplot/CustomPlotFile.h"
#include "waspcore/wasp_bug.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace wasp
{
CustomPlot::BrushStyle CustomPlot::brushStyle(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "solidpattern")
    {
        return SolidPattern;
    }
    else if (l == "dense1pattern")
    {
        return Dense1Pattern;
    }
    else if (l == "dense2pattern")
    {
        return Dense2Pattern;
    }
    else if (l == "dense3pattern")
    {
        return Dense3Pattern;
    }
    else if (l == "dense4pattern")
    {
        return Dense4Pattern;
    }
    else if (l == "dense5pattern")
    {
        return Dense5Pattern;
    }
    else if (l == "dense6pattern")
    {
        return Dense6Pattern;
    }
    else if (l == "dense7pattern")
    {
        return Dense7Pattern;
    }
    else if (l == "horpattern")
    {
        return HorPattern;
    }
    else if (l == "verpattern")
    {
        return VerPattern;
    }
    else if (l == "crosspattern")
    {
        return CrossPattern;
    }
    else if (l == "bdiagpattern")
    {
        return BDiagPattern;
    }
    else if (l == "fdiagpattern")
    {
        return FDiagPattern;
    }
    else if (l == "diagcrosspattern")
    {
        return DiagCrossPattern;
    }

    return NoBrush;
}

std::string CustomPlot::brushStyle(BrushStyle s)
{
    switch (s)
    {
        case SolidPattern:
            return "solidpattern";
        case Dense1Pattern:
            return "dense1pattern";
        case Dense2Pattern:
            return "dense2pattern";
        case Dense3Pattern:
            return "dense3pattern";
        case Dense4Pattern:
            return "dense4pattern";
        case Dense5Pattern:
            return "dense5pattern";
        case Dense6Pattern:
            return "dense6pattern";
        case Dense7Pattern:
            return "dense7pattern";
        case HorPattern:
            return "horpattern";
        case VerPattern:
            return "verpattern";
        case CrossPattern:
            return "crosspattern";
        case BDiagPattern:
            return "bdiagpattern";
        case FDiagPattern:
            return "fdiagpattern";
        case DiagCrossPattern:
            return "diagcrosspattern";
        default:
            return "nobrush";
    }
}

CustomPlot::ErrorType CustomPlot::errorType(const std::string& t)
{
    // compare to lowercase version
    std::string l;
    std::transform(t.begin(), t.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "key")
    {
        return etKey;
    }
    else if (l == "value")
    {
        return etValue;
    }
    else if (l == "both")
    {
        return etBoth;
    }

    return etNone;
}

std::string CustomPlot::errorType(ErrorType t)
{
    switch (t)
    {
        case etKey:
            return "key";
        case etValue:
            return "value";
        case etBoth:
            return "both";
        default:
            return "none";
    }
}

CustomPlot::GradientPreset CustomPlot::gradientPreset(const std::string& p)
{
    // compare to lowercase version
    std::string l;
    std::transform(p.begin(), p.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "grayscale")
    {
        return gpGrayscale;
    }
    else if (l == "hot")
    {
        return gpHot;
    }
    else if (l == "cold")
    {
        return gpCold;
    }
    else if (l == "night")
    {
        return gpNight;
    }
    else if (l == "candy")
    {
        return gpCandy;
    }
    else if (l == "geography")
    {
        return gpGeography;
    }
    else if (l == "ion")
    {
        return gpIon;
    }
    else if (l == "thermal")
    {
        return gpThermal;
    }
    else if (l == "polar")
    {
        return gpPolar;
    }
    else if (l == "spectrum")
    {
        return gpSpectrum;
    }
    else if (l == "hues")
    {
        return gpHues;
    }

    return gpJet;
}

std::string CustomPlot::gradientPreset(GradientPreset p)
{
    switch (p)
    {
        case gpGrayscale:
            return "grayscale";
        case gpHot:
            return "hot";
        case gpCold:
            return "cold";
        case gpNight:
            return "night";
        case gpCandy:
            return "candy";
        case gpGeography:
            return "geography";
        case gpIon:
            return "ion";
        case gpThermal:
            return "thermal";
        case gpPolar:
            return "polar";
        case gpSpectrum:
            return "spectrum";
        case gpHues:
            return "hues";
        default:
            return "jet";
    }
}

CustomPlot::LabelType CustomPlot::labelType(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "datetime")
    {
        return ltDateTime;
    }

    return ltNumber;
}

std::string CustomPlot::labelType(LabelType t)
{
    switch (t)
    {
        case ltDateTime:
            return "datetime";
        default:
            return "number";
    }
}

CustomPlot::LineStyle CustomPlot::lineStyle(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "line")
    {
        return lsLine;
    }
    else if (l == "stepleft")
    {
        return lsStepLeft;
    }
    else if (l == "stepright")
    {
        return lsStepRight;
    }
    else if (l == "stepcenter")
    {
        return lsStepCenter;
    }
    else if (l == "impulse")
    {
        return lsImpulse;
    }

    return lsNone;
}

std::string CustomPlot::lineStyle(LineStyle s)
{
    switch (s)
    {
        case lsLine:
            return "line";
        case lsStepLeft:
            return "stepleft";
        case lsStepRight:
            return "stepright";
        case lsStepCenter:
            return "stepcenter";
        case lsImpulse:
            return "impulse";
        default:
            return "none";
    }
}

CustomPlot::PenCapStyle CustomPlot::penCapStyle(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "squarecap")
    {
        return SquareCap;
    }
    else if (l == "roundcap")
    {
        return RoundCap;
    }

    return FlatCap;
}

std::string CustomPlot::penCapStyle(PenCapStyle s)
{
    switch (s)
    {
        case SquareCap:
            return "squarecap";
        case RoundCap:
            return "roundcap";
        default:
            return "flatcap";
    }
}

CustomPlot::PenJoinStyle CustomPlot::penJoinStyle(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "beveljoin")
    {
        return BevelJoin;
    }
    else if (l == "roundjoin")
    {
        return RoundJoin;
    }

    return MiterJoin;
}

std::string CustomPlot::penJoinStyle(PenJoinStyle s)
{
    switch (s)
    {
        case BevelJoin:
            return "beveljoin";
        case RoundJoin:
            return "roundjoin";
        default:
            return "miterjoin";
    }
}

CustomPlot::PenStyle CustomPlot::penStyle(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "solidline")
    {
        return SolidLine;
    }
    else if (l == "dashline")
    {
        return DashLine;
    }
    else if (l == "dotline")
    {
        return DotLine;
    }
    else if (l == "dashdotline")
    {
        return DashDotLine;
    }
    else if (l == "dashdotdotline")
    {
        return DashDotDotLine;
    }

    return NoPen;
}

std::string CustomPlot::penStyle(PenStyle s)
{
    switch (s)
    {
        case SolidLine:
            return "solidline";
        case DashLine:
            return "dashline";
        case DotLine:
            return "dotline";
        case DashDotLine:
            return "dashdotline";
        case DashDotDotLine:
            return "dashdotdotline";
        default:
            return "nopen";
    }
}

CustomPlot::ScaleType CustomPlot::scaleType(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "log")
    {
        return stLogarithmic;
    }

    return stLinear;
}

std::string CustomPlot::scaleType(ScaleType s)
{
    switch (s)
    {
        case stLogarithmic:
            return "log";
        default:
            return "linear";
    }
}

CustomPlot::ScatterShape CustomPlot::scatterShape(const std::string& s)
{
    // compare to lowercase version
    std::string l;
    std::transform(s.begin(), s.end(), std::back_inserter(l),
                   [](char c) { return std::tolower(c); });

    if (l == "dot")
    {
        return ssDot;
    }
    else if (l == "cross")
    {
        return ssCross;
    }
    else if (l == "plus")
    {
        return ssPlus;
    }
    else if (l == "circle")
    {
        return ssCircle;
    }
    else if (l == "disc")
    {
        return ssDisc;
    }
    else if (l == "square")
    {
        return ssSquare;
    }
    else if (l == "diamond")
    {
        return ssDiamond;
    }
    else if (l == "star")
    {
        return ssStar;
    }
    else if (l == "triangle")
    {
        return ssTriangle;
    }
    else if (l == "triangleinverted")
    {
        return ssTriangleInverted;
    }
    else if (l == "crosssquare")
    {
        return ssCrossSquare;
    }
    else if (l == "plussquare")
    {
        return ssPlusSquare;
    }
    else if (l == "crosscircle")
    {
        return ssCrossCircle;
    }
    else if (l == "pluscircle")
    {
        return ssPlusCircle;
    }
    else if (l == "peace")
    {
        return ssPeace;
    }

    return ssNone;
}

std::string CustomPlot::scatterShape(ScatterShape s)
{
    switch (s)
    {
        case ssDot:
            return "dot";
        case ssCross:
            return "cross";
        case ssPlus:
            return "plus";
        case ssCircle:
            return "circle";
        case ssDisc:
            return "disc";
        case ssSquare:
            return "square";
        case ssDiamond:
            return "diamond";
        case ssStar:
            return "star";
        case ssTriangle:
            return "triangle";
        case ssTriangleInverted:
            return "triangleinverted";
        case ssCrossSquare:
            return "crosssquare";
        case ssPlusSquare:
            return "plussquare";
        case ssCrossCircle:
            return "crosscircle";
        case ssPlusCircle:
            return "pluscircle";
        case ssPeace:
            return "peace";
        default:
            return "none";
    }
}

CustomPlot::CustomPlot()
{
    // default *1Axis (sub)grid to light grey
    for (auto g : {&mX1Axis.grid(), &mY1Axis.grid()})
    {
        for (auto c :
             {&g->pen().brush().color(), &g->subGridPen().brush().color()})
        {
            c->red(192);
            c->green(192);
            c->blue(192);
        }
    }

    // default *2Axis (sub)grid to yellow, hidden
    for (auto g : {&mX2Axis.grid(), &mY2Axis.grid()})
    {
        g->visible(false);

        for (auto c :
             {&g->pen().brush().color(), &g->subGridPen().brush().color()})
        {
            c->red(255);
            c->green(255);
        }
    }
}

CustomPlot::CustomPlot(const CustomPlot& cpi)
    : mLegend(cpi.mLegend)
    , mSeries(cpi.mSeries)
    , mX1Axis(cpi.mX1Axis)
    , mX2Axis(cpi.mX2Axis)
    , mY1Axis(cpi.mY1Axis)
    , mY2Axis(cpi.mY2Axis)
{
}

CustomPlot::~CustomPlot() {}

CustomPlot::Legend& CustomPlot::legend()
{
    return mLegend;
}

const CustomPlot::Legend& CustomPlot::legend() const
{
    return mLegend;
}

void CustomPlot::reset()
{
    mTitle  = Title();
    mLegend = Legend();
    mX1Axis = Axis();
    mY1Axis = Axis();
    mX2Axis = Axis();
    mY2Axis = Axis();

    mSeries.clear();
}

CustomPlot::Series::Collection& CustomPlot::series()
{
    return mSeries;
}

const CustomPlot::Series::Collection& CustomPlot::series() const
{
    return mSeries;
}

const CustomPlot::Title& CustomPlot::title() const
{
    return mTitle;
}

CustomPlot::Title& CustomPlot::title()
{
    return mTitle;
}

CustomPlot::Axis& CustomPlot::x1Axis()
{
    return mX1Axis;
}

const CustomPlot::Axis& CustomPlot::x1Axis() const
{
    return mX1Axis;
}

CustomPlot::Axis& CustomPlot::x2Axis()
{
    return mX2Axis;
}

const CustomPlot::Axis& CustomPlot::x2Axis() const
{
    return mX2Axis;
}

CustomPlot::Axis& CustomPlot::y1Axis()
{
    return mY1Axis;
}

const CustomPlot::Axis& CustomPlot::y1Axis() const
{
    return mY1Axis;
}

CustomPlot::Axis& CustomPlot::y2Axis()
{
    return mY2Axis;
}

const CustomPlot::Axis& CustomPlot::y2Axis() const
{
    return mY2Axis;
}

CustomPlot::Axis::Axis()
    : mLabelDateFormat("hh:mm:ss dd.MM.yy")
    , mLabelPrecision(6.0)
    , mLabelType(ltNumber)
    , mNumberFormat("gb")
    , mRangeMax(std::numeric_limits<double>::max())
    , mRangeMin(std::numeric_limits<double>::lowest())
    , mScaleType(stLinear)
    , mTickLabelRotation(0.0)
    , mVisible(true)
{
}

CustomPlot::Axis::Axis(const Axis& a)
    : mGrid(a.mGrid)
    , mLabel(a.mLabel)
    , mLabelDateFormat(a.mLabelDateFormat)
    , mLabelFont(a.mLabelFont)
    , mLabelPrecision(a.mLabelPrecision)
    , mLabelType(a.mLabelType)
    , mNumberFormat(a.mNumberFormat)
    , mRangeMax(a.mRangeMax)
    , mRangeMin(a.mRangeMin)
    , mScaleType(a.mScaleType)
    , mTickLabelFont(a.mTickLabelFont)
    , mTickLabelRotation(a.mTickLabelRotation)
    , mTicks(a.mTicks)
    , mVisible(a.mVisible)
{
}

CustomPlot::Axis& CustomPlot::Axis::operator=(const Axis& a)
{
    mGrid              = a.mGrid;
    mLabel             = a.mLabel;
    mLabelDateFormat   = a.mLabelDateFormat;
    mLabelFont         = a.mLabelFont;
    mLabelPrecision    = a.mLabelPrecision;
    mLabelType         = a.mLabelType;
    mNumberFormat      = a.mNumberFormat;
    mRangeMax          = a.mRangeMax;
    mRangeMin          = a.mRangeMin;
    mScaleType         = a.mScaleType;
    mTickLabelFont     = a.mTickLabelFont;
    mTickLabelRotation = a.mTickLabelRotation;
    mTicks             = a.mTicks;
    mVisible           = a.mVisible;

    return *this;
}

const CustomPlot::Grid& CustomPlot::Axis::grid() const
{
    return mGrid;
}

CustomPlot::Grid& CustomPlot::Axis::grid()
{
    return mGrid;
}

bool CustomPlot::Axis::hasRangeMax() const
{
    return mRangeMax != std::numeric_limits<double>::max();
}

bool CustomPlot::Axis::hasRangeMin() const
{
    return mRangeMin != std::numeric_limits<double>::lowest();
}

bool CustomPlot::Axis::isNumberFormatEB() const
{
    return mNumberFormat == "eb";
}

bool CustomPlot::Axis::isNumberFormatEBC() const
{
    return mNumberFormat == "ebc";
}

bool CustomPlot::Axis::isNumberFormatG() const
{
    return mNumberFormat == "g";
}

bool CustomPlot::Axis::isNumberFormatGB() const
{
    return mNumberFormat == "gb";
}

const std::string& CustomPlot::Axis::label() const
{
    return mLabel;
}

void CustomPlot::Axis::label(const std::string& l)
{
    mLabel = l;
}

const CustomPlot::Color& CustomPlot::Axis::labelColor() const
{
    return mLabelColor;
}

CustomPlot::Color& CustomPlot::Axis::labelColor()
{
    return mLabelColor;
}

const std::string& CustomPlot::Axis::labelDateFormat() const
{
    return mLabelDateFormat;
}

void CustomPlot::Axis::labelDateFormat(const std::string& ldf)
{
    mLabelDateFormat = ldf;
}

CustomPlot::Font& CustomPlot::Axis::labelFont()
{
    return mLabelFont;
}

const CustomPlot::Font& CustomPlot::Axis::labelFont() const
{
    return mLabelFont;
}

double CustomPlot::Axis::labelPrecision() const
{
    return mLabelPrecision;
}

void CustomPlot::Axis::labelPrecision(double lp)
{
    mLabelPrecision = lp;
}

CustomPlot::LabelType CustomPlot::Axis::labelType() const
{
    return mLabelType;
}

void CustomPlot::Axis::labelType(LabelType lt)
{
    mLabelType = lt;
}

const std::string& CustomPlot::Axis::numberFormat() const
{
    return mNumberFormat;
}

void CustomPlot::Axis::numberFormat(const std::string& nf)
{
    mNumberFormat = nf;
}

double CustomPlot::Axis::rangeMax() const
{
    return mRangeMax;
}

void CustomPlot::Axis::rangeMax(double d)
{
    mRangeMax = d;
}

double CustomPlot::Axis::rangeMin() const
{
    return mRangeMin;
}

void CustomPlot::Axis::rangeMin(double d)
{
    mRangeMin = d;
}

CustomPlot::ScaleType CustomPlot::Axis::scaleType() const
{
    return mScaleType;
}

void CustomPlot::Axis::scaleType(ScaleType st)
{
    mScaleType = st;
}

CustomPlot::Font& CustomPlot::Axis::tickLabelFont()
{
    return mTickLabelFont;
}

const CustomPlot::Font& CustomPlot::Axis::tickLabelFont() const
{
    return mTickLabelFont;
}

double CustomPlot::Axis::tickLabelRotation() const
{
    return mTickLabelRotation;
}

void CustomPlot::Axis::tickLabelRotation(double tlr)
{
    mTickLabelRotation = tlr;
}

CustomPlot::Axis::tick_values& CustomPlot::Axis::ticks()
{
    return mTicks;
}

const CustomPlot::Axis::tick_values& CustomPlot::Axis::ticks() const
{
    return mTicks;
}

bool CustomPlot::Axis::visible() const
{
    return mVisible;
}

void CustomPlot::Axis::visible(bool v)
{
    mVisible = v;
}

CustomPlot::Bar::Bar() = default;

CustomPlot::Bar::Bar(const Bar& b) = default;

CustomPlot::Brush::Brush() : mStyle(SolidPattern) {}

CustomPlot::Brush::Brush(const Brush& b) : mColor(b.mColor), mStyle(b.mStyle) {}

CustomPlot::Brush& CustomPlot::Brush::operator=(const Brush& b)
{
    mColor = b.mColor;
    mStyle = b.mStyle;
    return *this;
}

const CustomPlot::Color& CustomPlot::Brush::color() const
{
    return mColor;
}

CustomPlot::Color& CustomPlot::Brush::color()
{
    return mColor;
}

CustomPlot::BrushStyle CustomPlot::Brush::style() const
{
    return mStyle;
}

void CustomPlot::Brush::style(BrushStyle s)
{
    mStyle = s;
}

CustomPlot::Color::Color() : mAlpha(255), mBlue(0), mGreen(0), mRed(0) {}

CustomPlot::Color::Color(const Color& c)
    : mAlpha(c.mAlpha), mBlue(c.mBlue), mGreen(c.mGreen), mRed(c.mRed)
{
}

CustomPlot::Color& CustomPlot::Color::operator=(const Color& orig)
{
    mAlpha = orig.mAlpha;
    mBlue  = orig.mBlue;
    mGreen = orig.mGreen;
    mRed   = orig.mRed;

    return *this;
}

int CustomPlot::Color::alpha() const
{
    return mAlpha;
}

void CustomPlot::Color::alpha(int a)
{
    mAlpha = a;
}

int CustomPlot::Color::blue() const
{
    return mBlue;
}

void CustomPlot::Color::blue(int b)
{
    mBlue = b;
}

int CustomPlot::Color::green() const
{
    return mGreen;
}

void CustomPlot::Color::green(int g)
{
    mGreen = g;
}

bool CustomPlot::Color::isValid() const
{
    return mRed >= 0 && mRed < 256 && mGreen >= 0 && mGreen < 256 &&
           mBlue >= 0 && mBlue < 256 && mAlpha >= 0 && mAlpha < 256;
}

int CustomPlot::Color::red() const
{
    return mRed;
}

void CustomPlot::Color::red(int r)
{
    mRed = r;
}

CustomPlot::ColorMap::ColorMap() : Series(), mGradient(gpJet) {}

CustomPlot::ColorMap::ColorMap(const ColorMap& cm) = default;

std::vector<std::vector<double>>& CustomPlot::ColorMap::data()
{
    return mData;
}
const std::vector<std::vector<double>>& CustomPlot::ColorMap::data() const
{
    return mData;
}

void CustomPlot::ColorMap::data(std::vector<std::vector<double>>& d)
{
    mData = d;
}

CustomPlot::GradientPreset CustomPlot::ColorMap::gradient() const
{
    return mGradient;
}

void CustomPlot::ColorMap::gradient(GradientPreset p)
{
    mGradient = p;
}

CustomPlot::Font::Font()
    : mBold(false)
    , mItalic(false)
    , mPointSize(13)
    , mStrikeOut(false)
    , mUnderline(false)
{
}

CustomPlot::Font::Font(const Font& f)
    : mBold(f.mBold)
    , mFamily(f.mFamily)
    , mItalic(f.mItalic)
    , mPointSize(f.mPointSize)
    , mStrikeOut(f.mStrikeOut)
    , mUnderline(f.mUnderline)
{
}

CustomPlot::Font& CustomPlot::Font::operator=(const Font& orig)
{
    mBold      = orig.mBold;
    mFamily    = orig.mFamily;
    mItalic    = orig.mItalic;
    mPointSize = orig.mPointSize;
    mStrikeOut = orig.mStrikeOut;
    mUnderline = orig.mUnderline;

    return *this;
}

bool CustomPlot::Font::bold() const
{
    return mBold;
}

void CustomPlot::Font::bold(bool b)
{
    mBold = b;
}

const std::string& CustomPlot::Font::family() const
{
    return mFamily;
}

void CustomPlot::Font::family(const std::string& f)
{
    mFamily = f;
}

bool CustomPlot::Font::italic() const
{
    return mItalic;
}

void CustomPlot::Font::italic(bool i)
{
    mItalic = i;
}

int CustomPlot::Font::pointsize() const
{
    return mPointSize;
}

void CustomPlot::Font::pointsize(int p)
{
    mPointSize = p;
}

bool CustomPlot::Font::strikeout() const
{
    return mStrikeOut;
}

void CustomPlot::Font::strikeout(bool s)
{
    mStrikeOut = s;
}

bool CustomPlot::Font::underline() const
{
    return mUnderline;
}

void CustomPlot::Font::underline(bool u)
{
    mUnderline = u;
}

CustomPlot::Graph::Graph()
    : Series()
    , mAreaOpacity(0.0)
    , mErrorType(etNone)
    , mLineStyle(lsLine)
    , mLineWeight(1)
    , mScatterShape(ssDot)
    , mScatterSize(6.0)
{
}

CustomPlot::Graph::Graph(const Graph& g) = default;

int CustomPlot::Graph::areaOpacity() const
{
    return mAreaOpacity;
}

void CustomPlot::Graph::areaOpacity(int ao)
{
    mAreaOpacity = ao;
}

CustomPlot::ErrorType CustomPlot::Graph::errorType() const
{
    return mErrorType;
}

void CustomPlot::Graph::errorType(ErrorType et)
{
    mErrorType = et;
}

const std::vector<double>& CustomPlot::Graph::keysUncertaintyLow() const
{
    return mKeysUncertaintyLow;
}

std::vector<double>& CustomPlot::Graph::keysUncertaintyLow()
{
    return mKeysUncertaintyLow;
}

const std::vector<double>& CustomPlot::Graph::keysUncertaintyHigh() const
{
    return mKeysUncertaintyHigh;
}

std::vector<double>& CustomPlot::Graph::keysUncertaintyHigh()
{
    return mKeysUncertaintyHigh;
}

CustomPlot::LineStyle CustomPlot::Graph::lineStyle() const
{
    return mLineStyle;
}

void CustomPlot::Graph::lineStyle(LineStyle ls)
{
    mLineStyle = ls;
}

size_t CustomPlot::Graph::lineWeight() const
{
    return mLineWeight;
}

void CustomPlot::Graph::lineWeight(size_t lw)
{
    mLineWeight = lw;
}

double CustomPlot::Graph::scatterSize() const
{
    return mScatterSize;
}

void CustomPlot::Graph::scatterSize(double ss)
{
    if (ss >= 0)
        mScatterSize = ss;
}

CustomPlot::ScatterShape CustomPlot::Graph::scatterShape() const
{
    return mScatterShape;
}

void CustomPlot::Graph::scatterShape(ScatterShape ss)
{
    mScatterShape = ss;
}

const std::vector<double>& CustomPlot::Graph::valuesUncertaintyLow() const
{
    return mValuesUncertaintyLow;
}

std::vector<double>& CustomPlot::Graph::valuesUncertaintyLow()
{
    return mValuesUncertaintyLow;
}

const std::vector<double>& CustomPlot::Graph::valuesUncertaintyHigh() const
{
    return mValuesUncertaintyHigh;
}

std::vector<double>& CustomPlot::Graph::valuesUncertaintyHigh()
{
    return mValuesUncertaintyHigh;
}

CustomPlot::Grid::Grid() : mSubGridVisible(false), mVisible(true) {}

CustomPlot::Grid::Grid(const Grid& g)
    : mPen(g.mPen)
    , mSubGridPen(g.mSubGridPen)
    , mSubGridVisible(g.mSubGridVisible)
    , mVisible(g.mVisible)
{
}

CustomPlot::Grid& CustomPlot::Grid::operator=(const Grid& g)
{
    mPen            = g.mPen;
    mSubGridPen     = g.mSubGridPen;
    mSubGridVisible = g.mSubGridVisible;
    return *this;
}

const CustomPlot::Pen& CustomPlot::Grid::pen() const
{
    return mPen;
}

CustomPlot::Pen& CustomPlot::Grid::pen()
{
    return mPen;
}

const CustomPlot::Pen& CustomPlot::Grid::subGridPen() const
{
    return mSubGridPen;
}

CustomPlot::Pen& CustomPlot::Grid::subGridPen()
{
    return mSubGridPen;
}

bool CustomPlot::Grid::subGridVisible() const
{
    return mSubGridVisible;
}

void CustomPlot::Grid::subGridVisible(bool v)
{
    mSubGridVisible = v;
}

bool CustomPlot::Grid::visible() const
{
    return mVisible;
}

void CustomPlot::Grid::visible(bool v)
{
    mVisible = v;
}

CustomPlot::Legend::Legend() : mVisible(true) {}

CustomPlot::Legend::Legend(const Legend& orig)
    : mFont(orig.mFont), mVisible(orig.mVisible)
{
}

CustomPlot::Legend& CustomPlot::Legend::operator=(const Legend& orig)
{
    mFont    = orig.mFont;
    mVisible = orig.mVisible;

    return *this;
}

CustomPlot::Font& CustomPlot::Legend::font()
{
    return mFont;
}

const CustomPlot::Font& CustomPlot::Legend::font() const
{
    return mFont;
}

bool CustomPlot::Legend::visible() const
{
    return mVisible;
}

void CustomPlot::Legend::visible(bool v)
{
    mVisible = v;
}

CustomPlot::Pen::Pen()
    : mCapStyle(SquareCap), mJoinStyle(BevelJoin), mStyle(SolidLine), mWidth(1)
{
}

CustomPlot::Pen::Pen(const Pen& p)
    : mBrush(p.mBrush)
    , mCapStyle(p.mCapStyle)
    , mJoinStyle(p.mJoinStyle)
    , mStyle(p.mStyle)
    , mWidth(p.mWidth)
{
}

CustomPlot::Pen& CustomPlot::Pen::operator=(const Pen& p)
{
    mBrush     = p.mBrush;
    mCapStyle  = p.mCapStyle;
    mJoinStyle = p.mJoinStyle;
    mStyle     = p.mStyle;
    mWidth     = p.mWidth;

    return *this;
}

const CustomPlot::Brush& CustomPlot::Pen::brush() const
{
    return mBrush;
}

CustomPlot::Brush& CustomPlot::Pen::brush()
{
    return mBrush;
}

CustomPlot::PenCapStyle CustomPlot::Pen::capStyle() const
{
    return mCapStyle;
}

void CustomPlot::Pen::capStyle(PenCapStyle pcs)
{
    mCapStyle = pcs;
}

CustomPlot::PenJoinStyle CustomPlot::Pen::joinStyle() const
{
    return mJoinStyle;
}

void CustomPlot::Pen::joinStyle(PenJoinStyle pjs)
{
    mJoinStyle = pjs;
}

CustomPlot::PenStyle CustomPlot::Pen::style() const
{
    return mStyle;
}

void CustomPlot::Pen::style(PenStyle ps)
{
    mStyle = ps;
}

int CustomPlot::Pen::width() const
{
    return mWidth;
}

void CustomPlot::Pen::width(int w)
{
    if (w >= 0)
    {
        mWidth = w;
    }
}

CustomPlot::Series::Series() : mKeyAxis(1), mValueAxis(3)
{
    mBrush.style(NoBrush);
    mSelectedBrush.style(NoBrush);
}

CustomPlot::Series::Series(CustomPlot::Series const&) = default;

CustomPlot::Series::~Series() = default;

const CustomPlot::Brush& CustomPlot::Series::brush() const
{
    return mBrush;
}

CustomPlot::Brush& CustomPlot::Series::brush()
{
    return mBrush;
}

int CustomPlot::Series::keyAxis() const
{
    return mKeyAxis;
}

void CustomPlot::Series::keyAxis(int a)
{
    mKeyAxis = a;
}

const std::vector<double>& CustomPlot::Series::keys() const
{
    return mKeys;
}

std::vector<double>& CustomPlot::Series::keys()
{
    return mKeys;
}

const std::string& CustomPlot::Series::name() const
{
    return mName;
}

void CustomPlot::Series::name(const std::string& n)
{
    mName = n;
}

const CustomPlot::Pen& CustomPlot::Series::pen() const
{
    return mPen;
}

CustomPlot::Pen& CustomPlot::Series::pen()
{
    return mPen;
}

const CustomPlot::Brush& CustomPlot::Series::selectedBrush() const
{
    return mSelectedBrush;
}

CustomPlot::Brush& CustomPlot::Series::selectedBrush()
{
    return mSelectedBrush;
}

const CustomPlot::Pen& CustomPlot::Series::selectedPen() const
{
    return mSelectedPen;
}

CustomPlot::Pen& CustomPlot::Series::selectedPen()
{
    return mSelectedPen;
}

int CustomPlot::Series::valueAxis() const
{
    return mValueAxis;
}

void CustomPlot::Series::valueAxis(int a)
{
    mValueAxis = a;
}

const std::vector<double>& CustomPlot::Series::values() const
{
    return mValues;
}

std::vector<double>& CustomPlot::Series::values()
{
    return mValues;
}

CustomPlot::Title::Title() : mVisible(true) {}

CustomPlot::Title::Title(const Title& t)
    : mColor(t.mColor), mFont(t.mFont), mText(t.mText), mVisible(t.mVisible)
{
}

CustomPlot::Title& CustomPlot::Title::operator=(const Title& t)
{
    mColor   = t.mColor;
    mFont    = t.mFont;
    mText    = t.mText;
    mVisible = t.mVisible;

    return *this;
}

const CustomPlot::Color& CustomPlot::Title::color() const
{
    return mColor;
}

CustomPlot::Color& CustomPlot::Title::color()
{
    return mColor;
}

const CustomPlot::Font& CustomPlot::Title::font() const
{
    return mFont;
}

CustomPlot::Font& CustomPlot::Title::font()
{
    return mFont;
}

const std::string& CustomPlot::Title::text() const
{
    return mText;
}

void CustomPlot::Title::text(const std::string& text)
{
    mText = text;
}

bool CustomPlot::Title::visible() const
{
    return mVisible;
}

void CustomPlot::Title::visible(bool v)
{
    mVisible = v;
}
}  // namespace wasp
