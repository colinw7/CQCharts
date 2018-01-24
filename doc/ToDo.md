High
 + Documentation
 + Use CQChartsModelP everywhere !!!!
 + More testing of x/y flip for axis/key/...
 + Radial Coords/Plot
 + More annotation range support (bar chart, pie ...)
 + Compose BoxObj, FillObj, TextBoxObj together
 + Buffer texts and remove overlaps (per layer/type)
 + Standard format (formats) for hierarchical and connectivity (node/edge) data
   + Support multi column to specify hierarchy
 + Multiple sets of connected data (single level tree) - csv ?
   . Filter to row or range or auto group (extra column, hierarchy for multiple sets of csv data)
 + Common code for hier plots
   + data import
   + name column handling 
 + Set cursor on mode change
 + Allow table/tree floating inside view
 + Edit mode to edit plot positions (illustrator code ?)
 + Only move key, title, axes in edit mode ?

+ TOAST UI plots
  + Split Axes (Left/Right, Same Y Value Range, Split in Middle, Mirrored)
  + Check in key
  + Scatter Plot Circle Size Key (Min, Max, Mean)

Scripting
 + Interface language (Ceil)
 + support data from script (Ceil)

Common Properties
 + Fill
   + visible
   + color
   + alpha
   + pattern
   + gradient ?
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

Grouping
 + Switch between key for group and bucketed values
 + Support group by hier, column, ... in all plots
 + Common grouping code
 + Bucket algorithm for all plots (box plot, ...)
 + Group in more areas
 + support different grouping per depth for push down/hier plots
 + group data for scrolled (left/right) plot series (view level)
 + group data for gridded plots (view level)
 + what does grouping do (configurable)
   + overlay (multi-plot)
   + stack (bar chart)
 + grouping on:
   + hier (group per parent path)
   + name column(s) (group per bucketed value, how handle multiple ?)
   + multiple columns (group per column)
 + Auto group in model - table -> tree
   + bucket controls
 + Consolidate binning algorithms (class/config)
 + How color key for multi-color data

Coloring
 + Color Column for all plots

Misc
 + Cache plot pixmaps
 + Draw inside/selected to overlay pixmap
 + Use QScript/Tcl for expression evaluation
   + Plugable evaluation engine
 + Animation
 + Custom editor for column (name or number)
 + OrderedMap/OrderedSet
   + Quick lookup of values in insertion order
   + Option list of values for common key (in insertion order)

Theme
 + Allow disable theme for fixed color plot
 + theme config file
 + more colors
 + add fonts (other defaults)
 + cube helix max/min range
 + contrast color set
 + support discrete colors

Filter
 + Generic Filter support (plugin)

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

Bar Chart
 + Allow hide sets as well as individual sub set types
 + Custom Data Labels (including hiding)

Box Plot
 + BoxObj master for boxes
 + Color String (palette or QColor)
 + Better tooltip for connected
 + interp connected

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
 + Better expand display when grouped
 + Key by group and by value (see bar chart)
 + Group Labels 

Radar Plot
 + Add Key
 + Stacked
 + Percent of Total/Max

Scatter
 + Use FillObj for circles

Sunburst
 + Flat coloring
 + Different node depth
 + Push/Pop

Tree map
 + Support Hier Value and Child Values (include in sum, total size)
 + Auto font size
 + Color Key
 + Color fixed size box by value

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
 + Hierarchical Key when groups or switch group/value color/sel ?
 + Optional column for label in all plots
 + Hierarchical

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
 + use html version (encoded text) in more plots
 + More Key ToolTips

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

Gradient Palette
 + Probe show color value at vertical line and on gradient
 + Interp vertical graph, horizontal gradient
 + Interp show RGB values
 + Min/Max, Negate controls on non-model data ?
 + Support indexed palette (1-N colors) with different color values

Data Processing
 + Script/Data
 + Table/Tree
 + Chart
