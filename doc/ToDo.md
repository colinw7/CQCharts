Abstract Painter/Javascript
 + Support Javascript callback in generated code (select)
 + JS Polygon List inside support
 + JS Library with Tests

TODO:
 + Scalable object to allow fit multiple (scalable) objects in rect
 + Use default symbol set (name) for all plots (scatter)
 + Summary Plot
   + Parallel Plot
   + Correlation Plot
   + Customization
   + Hide sub plots
   + Push in support all plots
   + fit box
   + convex hull on scatter
 + Custom Controls interface
   init(), addWidgets() all virtual
 + Summary plot. Input is multiple columns output is
   + Grid of scatter/barchart
   + Parallel Plot ?
   + Others (use existing correlation plot)
 + Shape edit swatch and string
 + Load CSV from line number
 + easy red/green distribution split
 + distribution plot custom controls include group
 + allow columns property to be edited/simplified as if single column
 + column property support name (associated plot ?)
 + jitter box plot with group/set
 + simple color editor (only palette or color)
 + tcl command to auto place texts
 + all placement algorithms support hier data
 + sankey annotation plot, arc start/end size the same
 + hier name support for connection plots (sankey) to provide extra level for grouping
   + display of hier name in adjacency
 + time based display of multiple values (weather ?)
 + unnormalize modelInd before use in plots (test/assert)
 + color column in tip
 + sankey hier name -> depth
 + trie to unique abbreviations
 + chord max count per depth/relative to parent for normalize
   + scale to max ...
   + mouse over names
   + chord show in/out (allow hide ?)
 + chord fit with hier (long) labels ?
 + text placement
   + pie text overlap removal
   + text place on line (used in axis, reuse axis code ?)
 + support model/columns for annotation data
 + more support for module roles in charts objects (when no columns ?)
 + meta data in more data file formats
 + more separated/gridded plots for grouped (grid column ?)
   + treemap
 + support columns in image plot (+ color column) (+ controls)
 + ensure new color handling code handles color column
   + better testing of color combinations
 + icon splitter group (support one of)
 + pie chart variants:
   https://www.tableau.com/about/blog/5-unusual-alternatives-pie-charts
 + pie chart double group (generic support) with split
 + better pie chart group colors and indicator/header
 + Remove ItemState from property editors (use or remove dirty)
 + sort xy x values before line connect (handle duplicate y per x)
 + make model..(int row, const Column &column, ...) API's in CQChartsPlot private
 + all plots with model real should also support details->uniqueId
 + Improve javascript treemap
 + pie plot groups into multiple circles
   + pie chart uses separate palette for each group ? (consistency)
   + pie chart key tip
 + edit move map key (jumps)
 + support anchor point (left/right colors) - general ?
   + bucket fixed stops
 + all plot keys handle ctrl/shift same as xy/scatter
 + numeric auto range should be continuous even if no values in group range
 + bucketer shared not copied (reuse column bucketer)
 + redo grouping
   + use column details
   + allow column to define range and other grouping parameters
   + when set group column, auto init depending on type (real, integer, string) and value range
   + add to custom controls (separate tab) and use range slider ...
   + underflow/overflow in CQBucketer <min >max
 + numUnique use bucket as alternative when numeric (customize)
 + disable custom controls when invalid for current state
 + grouping in separate tab of custom controls with extra options
 + scatter handling of columns when group matches map columns (color, symbol size, symbol type)
 + key auto scale symbol size when widget. better draw circles. curve ?
 + fix test_all.sh crashes
 + symbol and symbol type confusion
   + symbol type needs to be combined with fill/stroke to get symbol
   + symbol is complete defintion (type + fill/stroke, char, path, ...)
 + resources for files map name to file system or add file search path ?
 + Font Size type (reuse Font)
 + symbol control has fixed/interpolated switch, stacked widgets and key widget
   + map key widget
   + font size widget
 + range slider handles on top ? draw lines
 + Force symbol set to non-null value ("all"/"filled")
   + how handle filled/stroked for points
 + application options like snps to set global font ?
 + smaller font for range sizes (auto update ?)
 + color key handle discreet
 + auto show key when first set column
 + splitter custom controls. Less tabs, option to skip group/name/labels
 + no static data (must be part of charts)
 + Palette Name Edit should support all palette names not just current (or allow add)
 + parameter color palette (2 -> N colors with connecting curves to interp)
 + symbol editor
   + support lines/curves
   + support SVG (syntax)
   + all symbols have path
 + symbol characters inconsistent fit
 + quick filters. If number of visible unique or ranges less than tolerance show filter ?
 + tcl commands to create and add to symbol sets
   + use for utf 8 symbols
 + utf 8 symbols need name ?
 + Access to keys for mapped data (color, symbol size, symbol type)
 + Composite Plot Key (multiple plot items in single key)
 + Composite Plot for all plots
   + Fix issues with select / tooltip and crashes
 + Improve Symbol Set in View Settings
 + Custom Controls for more (all) plots
   + Include all 'major' parameters
   + test with scatter plot for symbol size, ...
   + need start/end slider widget for data/pixel ranges 
 + Symbol Set editor
 + Optional clipping for objects
 + overlapping obj/annotation select with cycle needs better code and consistent order
 + allow row select which object match optimization
 + auto analyze data, pick best plot type by weight, display plots in tabbed group
 + all overlay plots forward to first overlay plot and collect data for all plots
   + e.g. insideInd_, insideObjs_, sizeInsideObjs_ (assert if wrong plot)
   + better and consistent naming convention for overlay and specific plot routines
 + selection update model table when overlay plot and multiple models
   + display model of plot of first selected object ?
 + OpenGL model view ?
 + named image and name color editors
 + palette editor (all palette names ?) update when new palettes added
 + consistent uses of bbox_ , rect_ , ... in ChartsObj (see Title use of bbox)
   + careful with invalidate
 + consistent usage or orientation/horizontal/vertical
 + support currentColor ? Last used color
 + draw html text in box does not work when box larger than text
 + set model column_type data should not lose existing data
 + chord directional arrow (triangle end)
 + better mutex handling in model/column details
 + game of life
 + CQFactor (placement and animation)
 + sankey
   + remove overlaps keeps path id order (especially if single value)
   + track space below and reuse if possible
     + spread by smallest and use groups
   + adjust bezier to reduce overlaps
 + better title bar renderer for tip
 + annotations for all plot draw types
   + contour
   + arc
   + all code through draw util ?
 + object reference for path annotation
 + probe
   + no snap to object
   + snap to coord (axis minor tick)
   + add value lable
   + fixed placement/height
 + annotations have 'fit' property to be included in plot fit algorithm
 + zoom in should zoom plot background ?
 + symbol size key as separate class and allow as annotation
 + test write properties with enum values
 + R in Action
 + errors in more plots
 + html support for all text if possible.
   + Remove all calls to QFontMetricsF::width()
   + Only if user generated ?
 + all annotation mouse over
   + contrast color, better line handling
 + allow chaining. get indices, process ...
 + more common expression/filter evaluation code (remove old wildcard/regexp ?)
 + crashes in preview plot
 + add fit for preview (better auto fit)
 + tip
   + signal on show
   + show in separate area
   + set position (tl, br, ...)
   + show at mouse
 + more objects in scatter plot (point plot) overlays
   + use objects ?? mouse over tip ....
 + Completion
   + more command value completion (all ?)
   + add completion support for all args (and values)
 + column header names in tip
   + barchart, all plots ?
 + polygon data label (text) does not set bbox. Check if inside ?
   + rotated text box draw (correct ?)
 + region tool mgr activate/deactivate view region tool
 + null, bad values special draw in table delegate
   + per column null value
 + unset column from editor is -1 ?
 + simplify column expressions (no back references, all calc on demand)
   + update test case with running sum
 + context menu for key/axis/... under mouse
   + editing controls in menu (property to action(s)
 + opengl renderer (more work)
 + surface plot
 + othello game
   + improve controls
 + view scrolled (view x/y pages)
 + processing like event loop (callback) for tcl code
 + widgets for property change (associated property and type widget + optional label)
 + tcl widget signal handling
 + control plot (controls in plot with associated plot/current plot ?)
 + animation
 + auto update (use start/end update signals to optimize)
 + draw non-selected dimmed
   + can't use select layer ?
   + requires all object draw faded
   + select draw non-faded
 + double click support (key)
   + show group
 + only show selected indices/data in table ? (filter selected)
 + Inline dialog with max/pop out like gmail
   + use for control widgets ?
 + region tool
   + signal, point+size, rect. widget tracks
   + status bar
   + sync widget state on/off
 + hier bubble plot labels
 + size hint for init size
 + multiple axes for overlay plots
   + stacked axes
 + better bubble plot margin with title ?
   + signal on first calc/redraw
 + assert right model index (normalized)
 + hier bubble min value/group column (bucket name column for integer/real (control))
 + rect/position edit values from plot (region edit)
 + image edit/widget edit
 + drag label
 + key tip includes text (could be clipped) - all keys
 + no recalc updates (skip empty, fit, ...)
 + xy overlay key position outside plot
 + consistent model indices in tcl (-model and object indices)
 + hide tip columns if not supported
 + more common code in connection plot
 + skip empty for distribution plot filter range
 + support notip/tip columns in all plots
 + arrow relative position to other object (start/end)
 + arrow position is shape dependent
 + text overlap graph/remover/clipper (text movable, clippable, priority)
 + cleanup edit interface (move to base class)
 + Better pattern fills (pattern shape as fill)
 + Recalc margin from current value on resize as may have changed due to font change
 + Auto size title/axis to outer margin
 + Better handling of scaled (icon) images like SVG
 + tcl proc to run command with current location ? tcl state machine to add annotation ?
 + filterObjs for all connection types (chord, ...)
 + add from/to for all connection types
 + fit
   + not stable ? different results when zoomed in
 + View drawLayerType
   + might not be set correctly when accessed (reset to NONE and assert ?)
 + Connection plots
   + propagate up optional and custom (sum, average, ...)
   + Add depth and min value filters (all connection data sources)
   + how handle different src/dest values (bi-directional connection)
   + more common code
   + object for chord plot connection
   + Connected Scatter Plot
 + Improve auto fit (tolerance) for xy plot
   + single value or both directions
 + Bar Plot auto inside/outside for data label
 + Depth for all hier plots (connection plot)
 + Palettes need to be more regular (red/green/blue mixes)
 + Handdrawn
   + Better paths
 + remove device->setBrush/setPen calls
 + Use PlotType/Plot class hierarchy for create plot dialog menu
 + Model
   + Bucket and Fold by Hier
 + Real iformat for % values ?
 + Tip role for column to replace header name with custom value (all plots)
 + Factory classes for plots and plot objects
 + 3D plots
   + 3D force directed
   + pixel to point (unmap)
     + tooltip
   + nearest 3d point
   + text use box points to rotate to axis
   + break box into multiple normal rects
   + scatter labels using text obj
   + update object style without recreate
 + Hand drawn shapes
   + https://shihn.ca/posts/2020/roughjs-algorithms/
 + Bezier through points
   + https://exploringswift.com/blog/Drawing-Smooth-Cubic-Bezier-Curve-through-prescribed-points-using-Swift
   + https://github.com/Onaeem26/CubicBezierCurveSwift

High
  Variant
   + remove QVariant::fromValue calls outside CQChartsVariant and add all types to BaseModule/ColumnType
   + all variant cast goes through CQChartsVariant and is consistent
     + handle all possible types
     + converted flag ?
     + string fallback ?
   + Check plots handle all possible data types from model
 + pie fan plot (R In Action pg 124)
 + hex grid for scatter density
   + support shapes for key (circle, square, hex, ...)
 + filter
   + data or object
   + column only
 + key
   + key grouping item (with layout)
   + more testing of key annotation and allow add items
   + Move to thread crash for key with scrollbar ?
   + Auto key placement (quad tree, all plots)
   + Should be able to create fully functional key from annotations
     + press callback with enough information to show/hide data
   + more customization of key line item (others)
     + symbol/line style. format
   + more customization of color box item
     + shape, rounded, ...
 + expand plot with click (edit mode ?)
 + tabbed
   + tab change callback
   + test nested tab plots
   + tab through plots on view (like exitsting next/prev)
 + table plot
   + column color style
   + More menu editors (see table plot sort column)
   + support all variant types in table plot (image, color, ...)
 + use tcl expression for load_charts_model filter
 + column default/fallback value
 + stablize auto fit call with threads !
 + custom title fit (directional, allow formatted/scaled)
 + handle overlay, x1x2, y1y2, when plot hidden
   + use firstPlot data if overlay (don't propagate values)
 + monthly_sales.tcl (overlay) broken range
 + fill under broken ?
 + fit with bad margins
 + Nominal, Ordinal, Continuous Variables
 + Mapping of Values (Name->Number, Number->Name, Value->Color/Symbol Type/Size)
 + Filter as prompt in plot (common expression handling)
 + Header bar to switch data ?
 + Better javascript font rendering (device scaling, alignment ?)
 + Box Shape (circle, path, ....)
 + Output or string list result does not match echo !
 + Grouped Axis Ranges
   + Multiple Ranges, tick label per group.
 + Statistical Painter - use in tests
 + Test write/reload and compare same
 + Region tool to fill in coords
 + Custom editors for all properties (CQRealSpin, CQAlignEdit)
 + test <desc> <body> <result> tcl proc ?
 + Restrictions on column for editor (no vertical, no expression, ...)
 + Tcl Parser for all string conversions to/from
 + Single line edit for property editors (and focus proxy)
 + Testing (all values)
 + Replace QRectF, QPolygonF, QSizeF, QPointF
 + Auto fit text scaling
   + only shrink, common factor (across related objects)
   + Auto scale all axis tick labels (constant scale)
 + SQL sort/filter model ?
 + Load Model edit all meta data (only CSV)
 + Auto determine HTML for Text (Qt code ?)
 + Plot type description pass in advanced (change help)
 + Analyze skip bad numeric values (test case)
 + Basic Parameters (simpler create plot interface)
   + Include fold model creation and simplify grouping
 + Better filter model (new model)
 + Improve coloring with follow view
 + Better renderer for text used as buttons
 + No threading needed in plots for local data
 + values annotation : units, use width instead of rectangle
 + view annotation change doesn't redraw (rect fill/stroke visible)
 + const everywhere ?
 + SVG interactive output
 + Arrow polyline (mid points and mid symbols)
 + Generate table in JS output
 + access to model data in JS output (JSON)
 + Write JSON (test, different formats)
 + Combine CQChartsModelDetails and CQModelDetails
 + Same coloring in table and plot (if possible)
   + same palette
   + auto palette for single color
   + stops
 + Export model (META data and TIME (converted) value handling)
 + Export to HTML
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
 + Axis annotation
 + more code in point plot (base class for scatter/xy)
 + plot selection behavior -> select out, select in, none, both
 + More help content
 + Scaled rotated text
 + Improve edit arrow annotation preview
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
 + Filter by Zoom
 + Key/Title should be positioned relative to plot
 + Use Quad Tree to limit number of objects draw (min rect size)
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
 + Update Annotation Dialogs to Handle All Types
 + Axis Annotation (independent coords ?, angle)
 + Annotation connectors (arc like sankey, lines)
 + Edit View Annotation with mouse
 + Annotation tip, select, ..., widget (float widget in plot)
 + more reference objects for annotations
   + handle invalidation and re-create to same object (by name)
 + annotation select in rect (overlay)
 + key annotation (custom key)
   + key column ?
 + axis annotations for all 2d plots
 + powerpoint like text in box for annotation (with edit support)
   + mouse tool ?
 + update of visible for widget annotation (create and set property)
 + widget annotation edit
 + support shapes/sides for annotation (which ?)
 + Arrow annotation new style
 + More annotation range support (bar chart, pie ...)

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

Plots General
 + Restrict which plots can be combined (overlay)
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

Connection Plots
 + Use derived column data type to improve interface to columns
 + Support table format from image plot (rows/columns as from/to and cells as connection count
 + Adjacency plot becomes same as image plot ?

Adjacency Plot
 + Title not visible
 + Invert X/Y does not work
 + More controls
 + Elide should trim border text (?) or fail ?
 + Move label text control (fixed size)
 + Hide more properties
 + X Border Width
 + Better Text Placement/Sizing (Iterative calc fit)
 + Hierarchical Adjacency (H/V headers are hierarchical trees)
 + Labels in Box (count)
 + Allow turn off row/column labels

Bar Chart
 + Allow hide sets as well as individual sub set types
 + Custom Data Labels (including hiding)
 + Whisker/Box for bar values
 + Multiple columns with summary value (range, min, max) fails

Box Plot
 + BoxObj master for boxes
 + Color String (palette or QColor)
 + Better tooltip for connected
 + interp connected
 + extra column for x label
 + Sort by median
 + Connected Box Plot setting X axis wrong (box_connected.tcl) - also slow

Bubble Plot
 + Combine with Hier Bubble Plot
 + Text Scale Factor (reduce bbox size by factor to fit circle)
 + Color quick controls

Chord Plot
 + Control gap between sections
 + Separate segment and arc properties
 + Selectable arc
 + Color column ?

Contour Plot
 + support x,y,z columns ?

Delaunay Plot
 + Faster delaunay algorithm

Distribution Plot
 + Push/Pop save restore ranges
 + Draw data label inside bar if fits
 + Cross select slow with large number of values (allow disable ?)
 + Line
 + Log Scale
 + Axis Labels deault should be based on continutity (real)

Force Directed Plot
 + Crash on Preview Resize

Geometry Plot
 + Color Key
 + Font Size Column Support
 + Label Scale to Fit ...

Hier Bubble Plot
 + Pop to top
 + Pop top is off by one level
 + Better coloring algorithm ?
 + How handle non-cumulative values (percentages)
 + Value labels include color column.
 + Tip for color column should be column name

Image Plot
 + Allow sub set of model ?
 + Search and only matching (like tree map)
 + Same/similar to table for connection plot ?
 + Extra columns ?
 + Support other connection formats ?

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
 + Mouse over center label
 + Color Column Quick Control
 + Color included in tip

Radar Plot
 + Add Key
 + Stacked
 + Percent of Total/Max

Sankey
 + placement
   + don't center single item at src/dest when align src/dest
   + center max X vertically and let align handle rest
 + sankey remove edge overlaps (limit move up/down)
   + edit constraints
 + sankey_node.tcl bad placement (single x) -> rand ?
 + Move dot/graphviz plot to separate plot type (out of sankey)
 + Sankey editing
   + Better feedback for selected/over object
 + Dot language (Sankey)
   + More support
   + Better arrow connectors
   + Better node shapes
   + Manually move/place nodes

Scatter
 + Use FillObj for circles
 + Combine points before create objects
 + Cache objects as pixmaps/images

Sunburst
 + Flat coloring
 + Different node depth
 + Push/Pop

Table Plot
 + Locked rows
 + Total (summary) row (visible/filtered or all)
 + Multiple Header Lines - Column Header Group
 + More Interactive features
 + Hier Model
 + Selection
 + Header/Number Colors
 + Controls (Filter)
 + Resizable columns
 + Mouse Over
 + Mouse Resize
 + Scaled or Fixed Size Font
 + Scrollbars in Plot
 + Custom objsAtPoint or improve quad tree
 + Mouse wheel scroll
 + Vertical (no row number column) locked
 + Cell/Cross/None mouse over highlight

Tree map
 + Rotate text if text aspect > box aspect
 + Filter to visible (keep placement, don't show) 
 + Tree map performance for large data (file tree) (small object opt)
 + Tree Map title (max) depth
 + Push/Pop controls on title
 + Support Hier Value and Child Values (include in sum, total size)
 + Auto font size
 + Color Key
 + Color fixed size box by value
 + Html scaled not working
 + hierarchical key ? (tree map)
 + Test for extra nodes in tree map (hier with size and child with size)
 + Handle push/pop in tree map follow view
 + Hide children from header control

XY Plot
 + Fill under with no lines and rolling average should fill under rolling average
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
