High
 + Divided Area allow drag resize
 + Analyze, set time to X axis (scatter, xy, ...)
 + Tag grouped column for analyze
 + Remove expr proces that can easily be replace by tcl proc (map, rand, norm)
 + Arrow - empty needed ? Use rotated rect for line
 + offscreen display for print
 + aggregate function for distribution plot
 + auto deduction of plot data from data
 + better filtering of plot values
 + auto split data into list of plots (left/right buttons)
 + document expression handling (process, filter)
 + density axis plot scaling for multiple plots ?
 + Common Code for Data->Pen/Brush
 + Tree Map title (max) depth
 + Allow write to model value (tcl ?) not extra (extra option ?)
 + Consistent property names and hieracrchy
 + More Variants in Model (QImage/QIcon could supply custom symbol !)
 + Sort for scatter distribution
 + Layer only include image for object bbox
 + Column details in model data or model ?
 + Summary plot support in plot (not just dialog)
 + allow get property names of view, plot, obj
 + Qtcl class belongs to CQCharts not Cmds
 + Notifications on mouse enter/leave (others)
 + More share model details
 + More common grouping column handling
 + equal scale should be quick adjust of dataRange (cached original)
 + More variant properties to remove string conversion
 + More variant editors
 + More annotations (symbol, box, ...)
 + Annotation tip, select, ..., widget (float widget in plot)
 + Filter by Zoom
 + Key/Title should be positioned relative to plot
 + Use Quad Tree to limit number of objects draw (min rect size)
 + Position class for point position on view, plot, ... Use for annotations
 + text format, flow or split, format to aspect and then scale
 + use plot percent for key, title ? unzoomed ?
 + custom editors for column, columns, position, length
   + custom edit, use to/from string API's and custom contols
 + more standard code for column details (type, min, max, ..., bucket) and use in all plots
 + jitter (peturb move overlapping points)
 + add column details to model summary data
 + more usage of CQChartsUtil::testAndSet
 + add version to plot data, bump count when changed and updated cached values on version change
 + Remove color set. Store colors in palette ?
 + More column details in type: min/max ...
 + better default colors
 + select plot updates current embedded table model
 + select of plot in view with multiple plots deselects others
 + remove big model data
 + use model index for values so when model changes object can update without reload (if possible) ?

Medium
 + Scroll by Single Large Tick Mark ?
 + Documentation
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
 + Use Data not Obj to store data in objects (axis -> line obj ...)
 + rounded corner control
 + overview window (timeline)

View Settings
 + More signals in view settings to reduce dependencies
 + Support expand as well as overlay

+ TOAST UI plots (tui)
  + Split Axes (Left/Right, Same Y Value Range, Split in Middle, Mirrored)
  + Check in key
  + Scatter Plot Circle Size Key (Min, Max, Mean)

Scripting
 + Other interface languages (QtScript, Javascript, ...)

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

Data Analysis
 + Hierarchical Name (type support for separator)
 + Number of data (integer/real/time/...) columns
 + Column value grouping to generate hierarchy
   + distribution
 + 1D
   + Single data column and single value column -> bar chart, pie chart, ...
 + Allow combine/split columns
   + multiple name columns -> hier
   + two columns -> point (x,y)
 + Common strings in columns -> hier
 + Duplicate values in column -> grouping

Fit Text
 + Support bubble fit
 + Test prog
 + Test punctuation (::, ..., etc)
 + Cache

Misc
 + Cache plot pixmaps
 + Draw inside/selected to overlay pixmap
 + Use QScript for expression evaluation
   + Plugable evaluation engine
 + Animation
 + Custom editor for column (name or number)
 + OrderedMap/OrderedSet
   + Quick lookup of values in insertion order
   + Option list of values for common key (in insertion order)
 + plot stacking order

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

Annotations
 + Annotation Layer
 + Delete all
 + Animate

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
 + limit number of ticks (bad increment)

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
 + bucket coloring with filter (See HairEyeColorScatter.tcl)

Combined Plots
 + Better builtin support for split plots

Adjacency Plot
 + X Border Width
 + Better Text Placement/Sizing
 + Hierarchical Adjacency (H/V headers are hierarchical trees)
 + Labels in Box (count)
 + Allow turn off row/column labels

Bar Chart
 + Allow hide sets as well as individual sub set types
 + Custom Data Labels (including hiding)

Box Plot
 + BoxObj master for boxes
 + Color String (palette or QColor)
 + Better tooltip for connected
 + interp connected
 + extra column for x label
 + Sort by median
 + Connected Box Plot setting X axis wrong (box_connected.tcl) - also slow

Distribution Plot
 + Push/Pop save restore ranges
 + Draw data label inside bar if fits
 + Cross select slow with large number of values (allow disable ?)
 + Line
 + Log Scale

Hier Bubble Plot
 + Pop to top
 + Pop top is off by one level
 + Better coloring algorithm ?
 + How handle non-cumulative values (percentages)

Image Plot
 + separate x/y and cell text styles

Parallel Plot
 + Parallel needs x axis labels
 + Optional label on axis or title

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
 + Show size with label

XY Plot
 + Use color set for color column to allow explict colors and colors from data
 + Diverging
 + Smooth fit
 + Label Peaks/Valleys
 + Mean

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
 + Compress when all values are same color (bar chart)

Expander
 + Better Title Bar
 + Attach Icon

Auto fit
 + Not always called (init ?)
 + set title string triggers update
 + delayed (timer), dirty flag

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

Interactivity
 + Callbacks

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
   + Interp or index select with wrap

Data Processing
 + Sources
   + Script/Data
   + Model
   + Table/Tree
   + Chart
 + Operations
  + Add, Remove, Delete
    + Expression
  + Restricted View
  + Flip/SubSet
  + Show/Hide
  + Find
  + Bucket/Fold
