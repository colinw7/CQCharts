High
 + zoom/pan on overlay is recalcing objects
 + Documentation
 + Use CQChartsModelP everywhere !!!!
 + More testing of x/y flip for axis/key/...
 + Filter Stack (Push/Pop) Tree - keep/delete
 + Generic Filter support (plugin)
 + Allow disable theme for fixed color plot
 + Radial Cooords
 + More annotation range support (bar chart, pie ...)
 + Compose BoxObj, FillObj, TextBoxObj together
 + Optional shadow text draw
 + Buffer texts and remove overlaps (per layer/type)
 + Bucket algorithm for all plots (box plot, ...)

Common Properties
 + Fill
   + visible
   + color
   + alpha
   + pattern
 + Stroke
   + visible
   + color
   + alpha
   + width
   + dash
 + Text
   + visible
   + string
   + font
   + color
   + alpha
   + angle
   + contrast (shadow) - ToDo shadow properties
 + Box
   + cornerSize (ToDo corners)
   + sides
 + Point
   + visible
   + symbol
   + size
   + Stroke
     + visible (?)
     + color
     + alpha (?)
     + width
     + dash (?)
   + Fill
     + visible
     + color
     + alpha (?)
     + pattern (?)

Colors
 + Custom Editor
 + Contrast, Invert, B/W (Invert)

Misc
 + Cache plot pixmaps
 + Draw inside/selected to overlay pixmap
 + Use QScript/Tcl for expression evaluation
   + Plugable evaluation engine
 + Animation

Theme
 + theme config file
 + more colors

Functions
 + Improved Filter
 + Symbol better highlight

Data
 + Allow more assigned attributes to plot symbol (size, color)
 + Allow Expressions for Data/Columns
 + Use Type for more data customization

Model
 + Add extra columns/roles for extra properties
   + data labels, symbol size, ....
   + allow interchangeable role/column for data
 + Use sort filter to hide values

Axes
 + Match Y1/Y2 ticks if possible ?
 + Disable/Custom Zoom
 + Handle overlapping labels
   + Auto Hide, Resize, Rotate
 + Scale break

Title
 + Default to larger font

Plots
 + Step Line (Left/Center/Right) (Fill Under)
 + Restrict which plots can be combined
   + Bar/Column Chart NOT paired with Pie, Bar
 + Vector
 + Contour
 + Dendogram
 + Arrow at points

Pie Chart
 + Stacked (multiple categories)

Parallel Plot
 + Parallel needs x axis labels

Adjacency Plot
 + X Border Width

Box Plot
 + BoxObj master for boxes
 + Color String (palette or QColor)

Radar Plot
 + Add Key
 + Stacked
 + Percent of Total/Max

Plot
 + Context Menu
   + More options
 + Allow config overlay data
   + connect to plot
 + timer delay on mouse over feedback and probe
 + auto fit with overlay/xy
 + current plot

Mouse Over
 + Customize

Bar
 + Allow hide sets as well as individual sub set types
 + Custom Data Labels (including hiding)

Pie
 + Radial labels
 + Expand selected

Key
 + Spacing
 + Inside/Outside
 + Overlap with Axis/Title
 + Title
 + Max entries

Overlay
 + Share Key
 + Share Palette

Symbols
 + Support, stroke, fill and width
 + Support multiple symbols for multiline plot

ToolTip
 + Format value depending on type
 + Format contents (wrap, max line length, max lines, ...)

Mouse Tools
 + Zoom : Data, Region
 + Zoom/Pan
   + Meaning for different plot types
   + Allow non-square zoom
   + Mouse position update in zoom mode

Parameters
 + Description

Optimization
 + Get more data direct from model instead of cached
 + Use H Tree (V Tree) for bars and other ordered data structures
