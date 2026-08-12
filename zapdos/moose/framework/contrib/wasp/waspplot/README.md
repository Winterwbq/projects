## CustomPlot

The `CustomPlot` class provides a Qt-style object model for describing 2D plots in a structured, serializable form. It supports rich plot styling, multiple series types, and complete axis/label customization. This abstraction can be used for backend-independent plot definitions, file export, or data-driven plot generation.

### Features

- Supports multiple series types: `Graph`, `Bar`, `ColorMap`
- Axis scaling: linear, log
- Marker shapes, line styles, area fills
- Full font and color control (including transparency)
- Serializable to/from `DataObject` format
- Compatible with Qt/QCustomPlot-style plotting
- Error bars and uncertainty support for graphs

### Plot Structure

The `CustomPlot` class is composed of the following core components:

#### Title
- Represents the plot title.
- Includes properties for:
  - Text content
  - Visibility
  - Font (family, size, weight, style)
  - Color (RGBA)

#### Legend
- Controls display of plot legend.
- Configurable visibility and font.

#### Axes
- Each plot includes up to 4 axes:
  - `x1Axis`, `x2Axis`, `y1Axis`, `y2Axis`
- Each `Axis` includes:
  - Axis label and label font/color
  - Tick label formatting (font, rotation)
  - Tick definitions (custom labels + values)
  - Axis range (min, max) and scale type
  - Label type (numeric or date/time)
  - Grid configuration

#### Grid
- Associated with each `Axis`.
- Includes:
  - Visibility of main and subgrid lines
  - Custom `Pen` objects for styling

---

### Series Types

Plot data is organized into a polymorphic collection of `Series` objects. Each series type represents a different visualization mode:

#### Series (base class)
- Common fields:
  - Name
  - Keys and values (X/Y data)
  - Associated key/value axes
  - `Pen` and `Brush` for standard and selected appearance

#### Graph
- Represents a line or scatter plot.
- Includes:
  - `LineStyle`: line, step, impulse
  - `ScatterShape`: marker type
  - Line weight and scatter size
  - Optional uncertainty data (`Low/High` bounds)
  - `ErrorType`: X, Y, or both
  - Area fill with adjustable opacity

#### Bar
- Represents a vertical or horizontal bar series.
- Uses standard `keys()` and `values()` for positioning and height.

#### ColorMap
- Represents a 2D heatmap or matrix of values.
- Includes:
  - 2D array of values (`data`)
  - Gradient preset (`GradientPreset`)
  - Mapping of X/Y keys to matrix dimensions

---

### Graphical Primitives

#### Color
- RGBA representation of a color.
- Supports transparency (alpha).

#### Font
- Font family and size
- Style options:
  - Bold
  - Italic
  - Underline
  - Strikeout

#### Brush
- Defines fill color and pattern.
- Uses `BrushStyle` enum.

#### Pen
- Defines stroke color, width, and line pattern.
- Uses `PenStyle`, `PenCapStyle`, `PenJoinStyle`.

---

### Enumerations

The system defines several enums for styling and formatting:

- `BrushStyle`: fill patterns (solid, dense, crosshatch, etc.)
- `PenStyle`: line patterns (solid, dashed, dotted, etc.)
- `PenCapStyle`: endcap rendering for lines
- `PenJoinStyle`: joins between line segments
- `LineStyle`: plot curve rendering (lines, steps, impulses)
- `ScatterShape`: symbol used at data points
- `ScaleType`: axis scaling (linear or logarithmic)
- `LabelType`: format of tick labels (numbers or datetime)
- `ErrorType`: type of error bars shown on graphs
- `GradientPreset`: named gradient maps for color mapping

---