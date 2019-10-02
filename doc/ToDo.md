Abstract Painter/Javascript
 + JS Painter Clip
 + JS complex inside test
 + Support javascript callback in generated code (select)

High
 + Combine CQChartsModelDetails and CQModelDetails
 + Same coloring in table and plot (if possible)
   + same palette
   + auto palette for single color
   + stops
 + Export model (META data and TIME (converted) value handling)
 + Scroll to selected
 + Better model property editors
 + Pivot
   + multiple labels (x and y).
     + QStringList or newline separator.
     + Combine common first/last strings ?
     + Multi axis ?
   + Skip missing
   + More fill variation
   + Key on/off for groups
   + Group on key ?
 + Should be able to create fully functional key from annotations
 + Axis annotation
 + more code in point plot (base class for scatter/xy)
 + plot selection behavior -> select out, select in, none, both
 + table per column coloring
 + More help content
 + Add base class for shared code between xy/scatter
 + Scaled rotated text
 + Improve edit arrow annotation preview
 + Create View or Plot Annotation
 + Edit View Annotation with mouse
 + Multiline text in box formatting (tree map/bubble text + value)
   + Standard library for this
 + Move Layer painter to standard library
   + Use in CQSchem ...
 + Missing tooltips in dialogs
 + probe supports x/y strings as well as reals (variant)
 + If overlay of same type apply changes to all plots ?
 + More html documentation update
 + visible property at all levels (box, fill, stroke) or single value
 + Better tcl list support for command option data
 + Summarize properties using box,shape,fill,stroke abstractions
 + Hide fill pattern by default
 + hasSet/hasGroup for plot type
 + Distinct is property of color def not palette
 + Remove draw Simple Text calls
 + Support View Key Header (or remove)
 + Hier colorInd. Color by Hier. virtual for ig/is to use parent color index
 + Only send color changed from editor on mouse release (or use different signal)
 + Bar Chart Key using new color code, all key items connected to objects
 + Pos for distribution plot (x for vertical) is meaningless for strings so turn off or remap
   + get pos from axis data or separate mapping ?
 + Interrupt and wait for thread kill
 + More plot object properties
 + Inconsistent key click behavior
 + Variable bin size for distribution
 + Common text drawing to support all options
 + Font Editor
 + Use QThread
 + Numeric Sort for strings (split into numeber/non-number blocks)
 + better handling of update notifications
 + Better/more stable algorithm for fit
 + generate plot on thread (redraw when finished)
 + view annotations in offscreen pixmap/image
 + Allow color names/symbol names in color map
 + Copy model (filtered, new columns, ...) like export_model
   + Ensure skip filtered and includes filtered
 + Serialize State
 + Use new types (color, symbol, font) to auto fill scatter plot
 + cleanup theme interface (allow view, plot customization)
 + Group Plot grouping and Distrib Plot Group should share common code
 + Update plot helps
 + Column Type parameter types
 + Single Model/Expr Wrapper
 + Analyze, set time to X axis (scatter, xy, ...)
 + Remove expr process that can easily be replace by tcl proc (map, rand, norm)
 + Arrow - empty needed ? Use rotated rect for line
 + offscreen display for print
 + aggregate function for distribution plot
 + auto deduction of plot data from data
 + better filtering of plot values
 + auto split data into list of plots (left/right buttons)
 + document expression handling (process, filter)
 + density axis plot scaling for multiple plots ?
 + Common Code for Data->Pen/Brush
 + Allow write to model value (tcl ?) not extra (extra option ?)
 + Consistent property names and hierarchy
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
 + Adjacency Plot bad draw on resize (not all layers drawn)
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

Chord Plot
 + Control gap between sections
 + Separate segment and arc properties

Distribution Plot
 + Push/Pop save restore ranges
 + Draw data label inside bar if fits
 + Cross select slow with large number of values (allow disable ?)
 + Line
 + Log Scale
 + Axis Labels deault should be based on continutity (real)

Hier Bubble Plot
 + Pop to top
 + Pop top is off by one level
 + Better coloring algorithm ?
 + How handle non-cumulative values (percentages)

Image Plot
 + Allow sub set of model ?
 + Search and only matching (like tree map)

Parallel Plot
 + Parallel needs x axis labels
 + Optional label on axis or title

Pie Chart
 + Stacked (multiple categories)
 + Radial labels (see radar)
 + Better expand display when grouped
 + Key by group and by value (see bar chart)
 + Group Labels 
 + Add gap between sections

Radar Plot
 + Add Key
 + Stacked
 + Percent of Total/Max

Scatter
 + Use FillObj for circles
 + Combine points before create objects
 + Cache objects as pixmaps/images

Sunburst
 + Flat coloring
 + Different node depth
 + Push/Pop

Tree map
 + Filter to visible (keep placement, don't show) 
 + Tree map performance for large data (file tree) (small object opt)
 + Tree Map title (max) depth
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
 + Link to property

Optimization
 + Get more data direct from model instead of cached
 + Use H Tree (V Tree) for bars and other ordered data structures

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
