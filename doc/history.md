## 8/6/2017 ##
 + Improve Integral Axis
 + Add Plot Objects
 + Parallel Plot
 + Add TSV support
 + Stacked XY Plot
## 8/13/2017 ##
 + Add Key
 + Add Axis Labels
 + Add ToolTip
 + Add Zoom/Pan
## 8/16/2017 ##
 + Add Geometry Plot
 + Add Filtering to Table
## 8/20/2017 ##
 + Allow multiple plots per window
 + Add Adjacency Plot
 + Add Delaunay/Veronoi Plot
 + Add Value for Geometry Plot
 + Allow Click of Key to show/hide data
## 8/27/2017 ##
 + Add objects to Adjacency Plot
 + Allow axis to be placed on left/right top/bottom
 + Add Bubble Plot/Hier Bubble Plot
 + Support two xy plots with separate y axes
 + Support output format for time type
 + Fix key init position
 + Add probe line
## 9/3/2017 ##
 + Add auto fit for margins
 + Allow key outside
 + Add line symbols for key
 + Add Tree Map
 + Add title object
 + Improve symbols
## 9/5/2017 ##
 + improve axis properties
 + allow grid draw front/back
 + fix axis column update
 + split view widgets to separate files
## 9/10/2017
 + Add types for real, integer, string
 + Key above/below
 + Support zoom on data (with overlay plot)
 + Add plot/data background border/color
 + Add plot/data clip
 + Add border individual side drawing
 + Support NaN values
 + Add objects for Sunburst plot
## 9/12/2017
 + Add more bar chart customization : fill/stroke/data label
 + Allow key of multiple bar chart sets to show sets or values
 + Clip title
 + Draw key above or below
 + Improve handling of overlapped/linked plots
## 9/13/2017
 + Add more border properties
 + Add grid fill
 + Use key background for disabled fade
 + draw faded key in xy plot
## 9/17/2017
 + Extra data label customization
 + new CQPropertyView with search/filter support
 + Add key to box plot
 + Title on all plots
## 9/24/2017
 + Add plot type manager
 + Add plot type parameters
 + Reduce dependencies
 + Add data labels for geometry plot
## 9/27/2017
 + horizontal bar chart
## 10/1/2017
 + fix memory leaks
 + improve adjacency fit
 + support rotated axis tick labels
 + support pie labels outside plot (> 1.0)
## 10/2/2017
 + add point label, symbol, color
 + support different key label
 + don't shown non-integer minor ticks for integer axis
 + add pie chart start angle
 + ensure pie chart labels are rendered on top
 + support set of property from command line args
## 10/3/2017
 + add factory to plot type
 + add create dialog
 + remove some name specific code to use plot type mgr
## 10/4/2017
 + improve create dialog
 + remove old create widgets
 + cleanup argument syntax
## 10/5/2017
 + improve handling of identical x values in bar chart of box plot
 + more options in create dialog
## 10/8/2017
 + Track views by name
 + Improve model ownership tracking
 + Allow auto hide of overlapping tick labels
 + Scatter plot support for point size (auto scale) and colors
 + All plot models derive from QSortFilterProxyModel and have base model
 + More line, point symbol config support (delaunay, parallel)
 + Improve equal scale handling
 + Improve error handling
 + Handle click signal in test to display details
## 10/15/2017
 + Add chord and force directed plots
## 10/22/2017
 + Use CQBaseModel enum for column types
 + Add custom font size support for scatter plot
 + Use common value set code for customization column value binning/scaling
 + Use CQExprModel for all input file models to allow expression support for extra column
## 10/29/2017
 + Improve auto range setting for overlay plots
 + Add fill between, file above and fill below for bivarite
 + Add pattern support for fill
 + Add inside ticks
 + Allow key flip and horizontal layout
 + Add impulse lines
 + Add fill under position
 + Reduce exported dependencies
## 11/5/2017
 + Add support for selection (select to/from view)
 + Add distibution plot
 + Update view dependencies so view uses property model and gradient palette and window uses
   property tree and gradient canvas/control
## 11/8/2017
 + Improve tree map coloring and borders
 + Improve cross selection
## 11/9/2017
 + Fix hier bubble and tree map hier selection
## 11/12/2017
 + Import and Rename some classes to reduce dependencies
 + Use common code to draw inside and selected state and add properties to customize
 + Add filter support to Csv and Tsv readers
 + Improve cross selection (view -> plot)
 + Add Data Label to distribution plot
 + Update aspect calc to work when plots tiled
 + Update gradient calc to support HSV and range limits
## 11/16/2017
 + custom plot probe
 + calc tree and table details in view
 + add context menu
 + add scroll left/right for multiple horizontal plots outside view
 + add more commands to scripting as well as control command (if, while, continue)
## 11/21/2017
 + performance improvements for large numbers of points
   + calc plot object id on demand
   + build quad tree on separate thread
 + update plot on model change
 + add radar and image plot
## 11/24/2017
 + Add theme colors
 + Add margin to adjacency ploy
 + Add invert for coordinate system (x/y)
 + Add label column for bar chart
## 11/26/2017
 + Add more color configuration
 + Support Log axes in xy plot
## 11/29/2017
 + Remove all non custom tick labels in distribution plot
 + Add max value for exp
 + Allow antialias to be disabled
 + Update context menu to show current key and theme state
## 12/03/2017
 + Add minor grid lines to grid
 + Update custom Line Dash class to use Pen and own property editor
 + Allow drag key and axes
 + Support custom plot menu items
 + Add push/pop for distribution plot to filter to selected bar
 + Support auto range calc in distribution plot
 + Add tab through objects under mouse
 + Add dark/light icons
 + Separate position and status labels on status line
 + Add impulse color/width to xy plot
## 12/04/2017
 + Add abstract renderer to allow replace QPainter
 + Add SVG print support
 + Improve object init in paint to avoid high CPU when no range/objects
 + Add no data object when no objects
 + Update column set routines to update all objects
## 12/10/2017
 + Add point or rect select support and add/remove/replace with key modifiers
 + Fix some renderering issues with invert x/y in axis/key
 + More control of axis tick label placement 
 + Support filter or selection in plot table
 + Improve filter/selection to support general expression syntax
 + Use axis tick placement in distribution plot to display start of range on left side of bar
 + Improve select/inside control to allow specify color, width and dash
 + Manual place key using view coords to keep stable placement when plot range changes
 + Update plot on filter change
 + Support set of view properties from input args
 + Add selected object count to status bar
 + Better tip id for scatter plot
 + Add table of current plot data to window
 + Change current window mode to combo and add controls per mode
## 12/11/2017
 + Revert back to QPainter from abstract renderer as not needed (yet)
 + Add includeZero option to axis
 + ensure start coord of distribution is snapped to delta
 + Update expander widget to allow detach
## 12/12/2017
 + Support override of distribution plot model data using virtuals
## 12/15/2017
 + Better bounding boxes for axis tick labels
 + Use Qt::EditRole for raw model data
## 12/16/2017
 + Update documentation with screen shots
 + Fix some memory leaks
 + Add pan mode
## 12/17/2017
 + Add annotation bbox to distribution plot
 + More fixes for axis label bounding boxes
 + Data label box
 + Improve title placement
 + Context menus for title, axis, key placementment/visibility
 + Drag move title
## 12/18/2017
 + Draw line when distribution bar too thin
 + Don't calc key size when not displayed
 + Add Grid to Axes context menu
## 12/30/2017
 + Add Sankey plot
 + More consistent property names and config of fill stroke
 + Delay plot update on property change to speed up multiple changes
 + Add Trie to ValueSet to auto generate string groups
 + Support multiple selection on distribution plot push
## 1/1/2018
 + Support name based connections in model for adjacency plot 
 + Support name based connections in model for chord plot 
 + Add annotation bbox to chord plot for better fit
 + Add dendrogram plot type
 + More style config for delaunay plot
 + Fix zoom for overlay plots
 + Add pos feedback in zoom mode and fix rubber band point order
## 1/7/2018
 + Support color column for hierarchical plots
 + Improve auto coloring of hierarchical and bubble plot
 + Add Hierarchical Scatter Plot
 + Add cycle back (Shift Tab)
 + Add push/pop and multiple root support to sunburst plot
 + Remove click zoom - always use push/pop
 + Add Hier Plot base type with support for multiple name columns
## 1/8/2018
 + Handle zero/negative size and sizes in hierarchical plots
 + Handle sizes on hierarchical nodes in hierarchical plots
 + Fix bad colorValueSet range color
 + Order inside objects by area
## 1/10/2018
 + Improve tip text for hier plots
 + Add more symbol display properties for scatter plot
 + Fix data label alignment when box is horizontal
 + Save current root as string in hier plots so model reload does not lose state
 + Improve check for integer/real columns
 + Add theme class and allow reset to default theme
 + Add theme palette to view settings
## 1/14/2018
 + Add Folded Model
 + Support model visitor to better support hierarchical input data
 + Call addParameters from plot type register
 + Add simple filter support to CQChartsModelFilter (multi column wildcards)
 + Tree map has control for header max area usage before hidden
 + Test proc supports folded model and table/tree switching
# 1/21/2018
 + Support grouping by hierarchy in bar chart and pie chart
 + Cleanup duplicate code in bar chart
 + Move common model data method to CQChartsModelFilter
 + Add named value sets
 + Add more palettes to gradient and allow selection
# 1/23/2018
 + Add common code for radial text
 + Add auto format text to fit rect
 + Add scatter plot symbol size key (wip)
 + Ensure skip empty root when drawing tree map
# 1/28/2018
 + Add support for view key (wip)
 + Fix margin on horizontal bar chart
 + Support angle extent in pie chart
 + Add edit mode for move of plot objects (key, title axis) and support move/resize plot (wip)
 + Fix bad equal scale in pie chart
 + Add support for ceil as interface language in test exec
 + support row headers in csv loader
 + Move filter for csv/tsv loaded to base class
# 2/4/2018
 + Use vistor class for more model data processing for common row handling
 + Improve axis bbox calc for invert x/y
 + Add percent and range (min, max value) to bar chart
 + Remove old equal scale calc code
 + Fix data label on horizontal bar chart
 + Support single direction zoom and pan - use in distribution plot
 + Fix bad range auto calc for distribution plot and use auto value as default for non-auto
 + Improve view key drawing and config
 + Improve Qt custom property type support and use for units in bar chart margin
 + Add support for every (start,stop,step) when processing model data
 + Fix bad auto fit for invert x/y
 + Support custom menu items for xy plot
 + Support more details in tree view
 + Support thousands format for numbers
 + Add context menu to view position text for value type
 + Cleanup duplicate code in xy plot
# 2/4/2018
 + Add Bar Chart Menu Items
 + More Visitor usage
 + Support From/To format for Force Directed nodes
 + Support Vertical Probe Lines
# 2/7/2018
 + Use visitor in more plot types
# 2/11/2018
 + Add drag handles for plot, title, key and axis
 + More visitor usage
 + vector support in xy plot
 + ensure parent used in model index calls for hier data
 + Add text and arrow annotations
 + Support export CSV/TSV from table
 + Edit uses left mouse instead of middle mouse
 + Allow generate model with variables as columns
# 2/18/2018
 + rename CQChartsPaletteColor to CQChartsColor
 + Add rect, ellipse, poly, text and point annotation
 + Remove FillObj, LineObj, PointObj. Use data struectures
 + Use CQChartsLength for more property widths
 + Support object cross select from property view
# 2/25/2018
 + Use CQChartsColumn class for all column values to support model columns, expression columns
   and vertical header columns
 + Improve usage of Column Type so columns with custom value types (rect, polygon, color)
   are handled using type not manually
 + Support QRectF and QPolygonF type columns in geometry plot.
 + More type support in base model
 + Support color column in geometry plot
 + Cleanup more object usage for style data
 + Use CQChartsPosition for more point values
 + Add some missing color alpha customization in plot colors
 + Rounded rect support x and y corner sizes with units
 + Support scaled font in hier bubble plot
 + Support first column header in CSV reader
 + Support polyline annotation
# 3/4/2018
 + Use testAndSet in more code
 + Add Path and Style columbn types to support SVG geometry
 + Update default palette
# 4/21/2018
 + make plot model data access virtual
 + add edit move using cursor keys
# 5/6/2018
 + support annotations on view
 + support bar chart log value
 + support theme number in color spec
 + use variant to string code in more places
 + add min/max scaled font size
 + allow combine y1/y2 and overlay and add x1/x2
 + add signals for all object selections
 + add id column support to more plots
 + improve print support (auto size and set size)
 + better size limits on expander
 + support xy impulse as box as well as line
 + rename commands for clearer usage and add more get/set support
# 5/9/2018
 + support align for text annotation
 + cache models from commands in charts instance
 + allow key to be optionally clipped
 + add aster (variable height) support in pie chart
 + add grid to pie chart
 + make grid in radar plot optional
 + update command names and support model id
# 5/13/2018
 + allow looking model details from model
 + add remap function for model process
 + add num_unique and map support to model column details
 + support color type in model details 
 + support color column real value for scatter plot
 + add name to role and role support on cli commands
 + make gui display optional when getting model details
# 5/19/2018
 + Add export to CSV/TSV
 + Support Tcl in CLI and process expression
 + More standard command arg processing
# 5/20/2018
 + Cleanup tcl support code
 + Fix some memory leaks
# 5/21/2018
 + export to file
 + remove get_view/set_view commands
 + add measure_text command
 + use tcl::mathfunc for model expressions
# 5/28/2018
 + add attributes to column parameters and use to improve plot dialog input
 + move interface palette to separate structure and separate tab in view settings
 + init group supports auto group by row number
 + add plots list and place, group and modify functions to view settings tab
 + support tcl in gradient palette defined functions
 + add preview plot to plot dialog
 + add simple validation to plot parameters
 + don't build object tree for single no data object
 + support optional double and color values in value set
 + signals on view plots changed for view settings update
 + add text and icon for title bar in expander
 + move mapped and map min/max options into column parameter definition
 + preview file and analyze contents in load dialog for easier loading
 + combine set_theme/set_palette and get_theme/get_palette commands
 + improve expression model operators to allow header type and type to be set
# 5/29/2018
 + add tips to some parameters
 + support auto set of format from model in plot dialog
 + move some plot properties in hierarchy
 + rename range property to rangeBar in bar chart (duplicate name)
 + move file load and analyze to separate file
# 5/30/2018
 + more CQChartsHierPlot to separate file
 + add max rows for preview plot
# 6/4/2018
 + add support for current model data and current view to charts class
 + allow disable auto range for distribution plot for string keys
 + move load model code to Loader class
 + Add tcl support for CQChartsModelExprMatch
 + Add missing object names for widgets
# 6/10/2018
 + use single getSelectIndices API for plot object for cross selection
 + fix invert x/y axis drawing for x1x2 y1y1
 + add alpha for box plot lines and update property names
 + allow box plot labels to be skipped
 + support expression columns in plot dialog input fields
 + add x/y and cell labels to image plot
 + add generation of correlation model from model
 + support list or lists for tcl var in var model and header rows/columns
 + add median calculation into column details and get data command
 + add unique value and unique value counts to column details
 + auto select plot when plot object selected
# 6/10/2018
 + move fold code into CQChartsModelData
 + add model dialog (to replace main window one)
 + add column editor
 + add more locks to obj tree
# 6/11/2018
 + distribution plot use range label for middle axis label placement
 + add annotation bbox for image plot
 + update filter model class names
 + more cleanup of model list/control class/dialog
 + Add random indices to summary model
# 6/12/2018
 + add sorting to summary model
