High
 + Documentation
 + Use CQChartsModelP everywhere !!!!
 + More testing of x/y flip for axis/key/...
 + Generic Filter support (plugin)
 + Allow disable theme for fixed color plot
 + Radial Coords
 + More annotation range support (bar chart, pie ...)
 + Compose BoxObj, FillObj, TextBoxObj together
 + Buffer texts and remove overlaps (per layer/type)
 + Bucket algorithm for all plots (box plot, ...)
 + Group in more areas
 + Standard format (formats) for hierarchical and connectivity (node/edge) data
   + Support multi column to specify hierarchy
 + Multiple sets of connected data (single level tree) - csv ?
 + Grouping support
   + support different grouping per depth for push down/hier plots
   + group data for scrolled (left/right) plot series
 + Auto group in model - table -> tree
 + Common code for hier plots
   + data import
   + name column handling 
 + Set cursor on mode change

+ TOAST UI plots
  + Split Axes (Left/Right, Same Y Value Range, Split in Middle, Mirrored)
  + Check in key
  + Scatter Plot Circle Size Key (Min, Max, Mean)

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
 + Custom Editor for palette/theme/color
 + Contrast, Invert, B/W (Invert)
 + Do hierarchical plots handle fill color ?

Misc
 + Cache plot pixmaps
 + Draw inside/selected to overlay pixmap
 + Use QScript/Tcl for expression evaluation
   + Plugable evaluation engine
 + Animation
 + Custom editor for column (name or number)

Theme
 + theme config file
 + more colors
 + cube helix max/min range
 + contrast color set
 + support discrete colors

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
 + column type from header or values ?

Axes
 + Match Y1/Y2 ticks if possible ?
 + Disable/Custom Zoom
 + Handle overlapping labels
   + Auto Hide, Resize, Rotate
 + Scale break

Title
 + Default to larger font

Property View
 + Add more custom edtiors
   + Column (Number or Name)
   + Color (Name, Palette or Theme)

Plots to Add
 + Step Line (Left/Center/Right) (Fill Under)
 + Vector
 + Contour
 + Arrow at points

Plots General
 + Restrict which plots can be combined
   + Bar/Column Chart NOT paired with Pie, Bar
 + columnValueType more usage
   + Common handling of column data type determination
 + All set methods (especially column) check value change
   + This should be a common pattern
 + Context Menu
   + More options
 + Allow config overlay data
   + connect to plot
 + timer delay on mouse over feedback and probe
 + auto fit with overlay/xy
 + current plot
 + value set manager (by name) for plot
 + common menu code

Adjacency Plot
 + X Border Width
 + Better Text Placement/Sizing

Bar
 + Allow hide sets as well as individual sub set types
 + Custom Data Labels (including hiding)

Box Plot
 + BoxObj master for boxes
 + Color String (palette or QColor)

Distribution Plot
 + Push/Pop save restore ranges
 + Draw data label inside bar if fits

Hier Bubble Plot
 + Pop to top
 + Pop top is off by one level
 + Better coloring algorithm ?
 + How handle non-cumulative values (percentages)

Parallel Plot
 + Parallel needs x axis labels

Pie Chart
 + Stacked (multiple categories)
 + Radial labels (see radar)
 + Expand selected

Radar Plot
 + Add Key
 + Stacked
 + Percent of Total/Max

Sunburst
 + Flat coloring
 + Different node depth
 + Push/Pop

Tree map
 + Support Hier Value and Child Values (include in sum, total size)
 + Auto font size
 + Color Key

XY Plot
 + Use color set for color column to allow explict colors and colors from data
 + Diverging

Mouse Over
 + Customize

Key
 + Spacing
 + Inside/Outside
 + Overlap with Axis/Title
 + Title
 + Max entries
 + Check Box for hide/show

Expander
 + Better Title Bar
 + Attach Icon

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
