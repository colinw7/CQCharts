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
# 6/13/2018
 + Remove CQChartsModelP
 + support idColumn cross selecton for all plots
 + Fix create plot dialog column check to include column arrays
 + Speed up random row selection in summary model
# 6/17/2018
 + Add more plot type parameters for axes, key, title support, log support, description
 + Improve value lookup for group value in hier column
 + Better box plot when single value in range
 + Add more functions to expression model
 + Support fold and collapse commands
 + Use CQBucketer class in distributon plot and bucket model function
 + Always store time values as doubles in model (convert values when format set)
# 6/19/2018
 + Add raw values for box plot
 + Add support for rol in column definition
 + Allow transpose of list order in tcl loader
 + Add parameter groups
 + Cache converted column type in mode
# 6/20/2018
 + add plot method to get model index and cleanup unused model pointer variables
 + update plot dialog to layout widgets in parameter groups
 + improve box plot with precalculated values in rows
# 6/21/2018
 + cleanup handling of column role
 + clear data model cache when type changes
# 6/24/2018
 + Add more plot type descriptions
 + Order plot types in dialog using new dimension and hierarchical properties
 + Add pan by bar width support for bar and distribution plots
 + Support full customization of outlier symbol in box plot
 + Cache formatted model values
 + Add pages to summary model and support in plot dialog
 + Make symbol a variant and use in plot properties
 + Support cached converted and formatted values in data model
# 7/01/2018
 + draw grid lines between ticks for bar chart groups
 + make new plot base class for grouping of row/column data
 + support horizontal box plot bars
 + support grouping in bubble plot
 + support edit of palette defined colors
 + handle less than three values sets in radar plot
 + add integer plot parameter types and improved editor for required values
 + add more parameter groups
 + skip unsupported items from plot context menu
# 7/08/2018
 + support length units for symbols
 + improve auto fit so pixel sized objects too big to fit are ignored
 + add bar plot base class for bar type plots
 + improve tips for scatter and parallel plots
 + support any symbol type for scatter
 + improve grouping for distribution plot
 + support overlay and range bar for grouped distribution plot
 + support query of plot object properties and get associated inds from tcl
 + add x/y axis label user overrides
 + add more descriptions
# 7/09/2018
 + enable plot model filter
 + support column name in filter expression
 + improve grouping for scatter plot
 + add best fit for scatter plot points
# 7/14/2018
 + add plot layer buffering for faster repaint
 + remove cexpr/ceil (support only tcl)
# 7/15/2018
 + add density plot
 + support associated group for column
 + support custom symbol type in scatter plot
# 7/16/2018
 + add mean line to density
 + buffer plots to actual pixel size
 + reduce redraws on drag
# 7/18/2018
 + add optional normalized and jitter points to box plot
 + don't store value ref in CQChartsBarChartObj
 + add bar chart and distribution plot object properties 
 + update current plot in view setting when changed in view
# 7/19/2018
 + Merge CQChartsExprModel and CQExprModel classes
# 7/22/2018
 + Add indices for box plot jitter symbols
 + Add dot lines and rug support for distribution plot. Improve mean line
 + Add rug support for scatter plot
 + Support value count in pie chart
 + add rnorm function to expr model
 + fix issue in Tcl_Obj real value to variant
 + Improve layer handling for mouse over and selection
 + Add layers to view settings
# 7/25/2018
 + Add dot lines to bar chart
 + Add support for sub values of column e.g. month from time value
 + Calc density range and use for better draw. Support denisty group offset
 + Support gradient fill for density
 + Add lines/point to xy context menu
# 7/29/2018
 + All symbol sizes use length units
 + Add stacked, side-by side for group distribution bars
 + Add scatter for group distribution values
 + Cleanup up duplicate code in expression filters
 + Speed up access to tcl column variables
 + Allow remove of palette defined color
 + Speed up isHierarchical test (limit rows checked)
 + Add Inside X/Y to Key menu
# 7/31/2018
 + improve draw order or overlay plots with buffered layers
 + fix inside symbol outline
# 8/05/2018
 + emit signal on plot object load to allow annotations to be build from existing objects
 + support plot object load signal in tcl
 + improve inside test for annotations
 + allow annotations to be deleted from command line
 + support selection highlight for annotations and draw to correct layers
 + Add violin, notch and errorbar to box plot
 + Improve scatter points for box plot (use distribution and allow stacked)
 + Improve set handling for box plot
 + Add standard deviation to model details
 + get_model_data command returns list of values when no column specified
 + support row, column and header names for get_model_data to return whole row, column or header
 + Improve density curve points (add tails and normalize)
 + support named column values in expression model processing
 + add scatter best fit deviation bar
 + add scatter convext hull
# 8/06/2018
 + mouse over for annotations
# 8/09/2018
 + add density and whisker x/y axis annotations
# 8/12/2018
 + add optional bars to distribution density plot
 + cleanup pen/brush set code
# 8/19/2018
 + Support units for annotation position/size
 + Add custom expr tcl procs
 + Add annotations to pie plot box
 + Fix scale/offset for overlay plots
 + Add grid to scatter plot
 + Add best fit to xy plot
# 8/26/2018
 + Use CRTP classes to factor out graphics customization duplicated code
 + Use CInterval for axis major ticks and bucketer
 + Move fill pattern into class
 + Add density map for scatter plot
 + Fix parallel plot mouse over
# 9/2/2018
 + Support auto font scaling based on plot pixel size
 + Cleanup more pen/brush handling
 + Add interface interp color support
 + Add fit bbox fill support
 + Improve parallel plot annotation bbox
 + Support group column for xy plot
 + Add Sub Layout support to minimize number of image layers
 + Support pixmap or image for layer
# 9/2/2018
 + fix bad tooltip for first bar of grouped barchart
 + show color value in barchart tooltip
 + add scale function to expr model
 + remove key/concat functions from expr model
 + support first column header in gnu data file type
 + add support for scaled cell labels in image plot
 + support balloon for image plot
 + fix invert x/y for image plot labels
 + add flip for correlation model
 + use table for column details dialog to improve format
 + use unique id/name as fallback for scatter plot with non-numeric columns
 + support #{name} to column number in expression
 + add subset_model and transpose_model commands
# 9/3/2018
 + add value row number to model visitor
# 9/10/2018
 + Add column analyzer to select plots and associated columns drom data
 + Add polygon list, connection list and name pair types with variant and column type support
 + Support generic QObject in style macros and use in axes code for styling
 + Fix annotation BBox for radar plot
 + Use CQSwitch widget for table filter edit options
 + Add tooltip to filter edit
 + Support And/Or for multiple filters
 + Support general tipColumn to supply tip string in model
# 9/11/2018
 + Improve parameter details
 + Ensure required parameters are set in analyze model
 + Support discriminator parameters (one or another column required)
 + Improve fade of hidden key items
 + Support click key to show single value set
 + Add style macros to key
# 9/12/2018
 + Fix tooltip for data values in scatter
# 9/15/2018
 + Use single CRTP header for all object styling (view, plot, ...)
 + Use CQChartsColumns for all column arrays and properties
 + Add CQChartsKeyLocation meta type
 + Add CQChartsSides meta type
 + CQChartsFillUnderSide and CQChartsFillUnderPos meta types
# 9/16/2018
 + Add inner margin support
# 9/23/2018
 + add enum parameter
 + distribution plot use plot type and value type
 + add min/max for distribution value type
 + support extra data for distribution plot values
 + add query of plot types and parameters
 + Add theme type and dark options on view
 + Use style template for view background 
# 9/30/2018
 + fix bar chart margins when stacked
 + add fold delta and count in model control
 + add fold clear in model control
 + use column type for default fold bucketer
 + support column name for fold columns
 + use currentModel to get model from model data
 + send currentModelChange signal when model data current model changes
 + add models button to view toolbar
# 10/01/2018
 + remove defaults for column parameters
# 10/14/2018
 + Add key role to base model and use to auto fill group column
 + Update barchart plot plot type/value type to match distribution plot
 + Add visible column support to show/hide individual rows
 + Add object name to models
 + Auto init columns in plot dialog
 + Better date axis ranges
 + Support sub groups for parameters
# 10/18/2018
 + Better parameter names
 + Use root value for hierarchical group column
 + Speed up model details for non-summary data
 + Improve plot dialog
# 10/22/2018
 + Speed up model details for monotonic, numeric columns
 + Add more performance trace checkpoints
 + Allow disable plot update timeout using environment variable
 + Allow display layer buffereing using environment variable
 + optional load of details in view settings
# 10/28/2018
 + Handle comma in format (time)
 + column type allows enumerate of parameters and model dialog adds prompts to match
 + store column parameters min, max, key in model
 + Add percent support to bar chart
 + Fix bad x label for bar chart
 + Allow data model original values to be changed (need -force)
 + Cleanup per file type filter models 
 + Track model sort to update model details monotonic
 + Fix bug in interval for fixed major increment
# 11/04/2018
 + split initObjs proc into createObjs to better check performance timing
 + Add dataColumn to distribution y axis
 + Reduce dependencies in CQChartsModelControl/CQChartsModelList
 + Support per column duplicates
 + Support regexp/wildcard in filter
 + allow filter type to be specified in command
 + Fix bug in pixel unit handling of outer margin
 + improve auto fit
 + add style properties to scattter grid cell
 + add x/y overview to view (single plot)
 + allow table and view settings to be hidden
 + add generic qt set/get property in tcl
 + replace -int/-real/-enum/-bool/-string parameter option in create_plot with -parameter
# 11/11/2018
 + allow write model, plot and annotation commands for current plot
 + Support symbol type, symbol size and font size as column types
 + Draw symbols in model using delegate
 + add settings tab for annotations
 + move theme to charts class
# 11/21/2018
 + Support write plot, annotations to file
 + Use EkCharts for all interpColor calls
 + Remove value sets from plots and use model details.
 + Improve color, symbol type, symbol size and font size mapping
 + Add annotation create dialog
 + Improve command names
# 11/25/2018
 + Use CQChartsRect for rect annotation
 + Use CQChartsPolygon for polygon/polyline annotation
 + Improve layout of annotation dialog
 + Add more custom editors for types
 + Add copy_model command
 + Improve select notifications
# 11/28/2018
 + Move CQBaseModel enums outside class to reduce dependencies
 + Add fixed size rect for text annotation
 + Use Qt::Orieintation for Direction enum
 + Move model util routines to CQChartsModelUtil
 + Support symbol image buffer using environment variable
# 12/29/2018
 + improve const correctness
# 1/6/2019
 + add thread support for calc range, create objs and draw objs
 + create box plot outliers as separate interactive objects
 + support gap between pie chart segments
 + improve customization of pie segment fill/stroke
 + allow change gap in chord plot
 + support role and unset value in column edit
 + cache column type per model
 + allow disable status bar widgets
 + more const correctness
 + fix bivariare xy line customization
# 1/13/2019
 + Ensure column type cache works for non registered QAbstractModel data
 + Improve view key rendering and add to context menu
 + Fix redraw of dragged edited objects when threads active
# 1/20/2019
 + Support line ends and connecting line width for xy vector/annotation arrow
 + improve handling of inside object/selected object in overlap plots
 + support fixed size view
 + smart rect select to do point select if no drag
 + support rect inside/touching
 + support cycle select with F1 key
 + separate out cmd processing classes
 + resize property view on show
# 2/3/2019
 + Add property editors for fill, stroke data types
 + Use shape type in updateObjPenBrushState
 + Add base class for line edit with widget menu
 + Add name values class and use string to/from data conversion in data editors
 + Support palette coloring of selected objects
 + Allow show/select operation for key click
 + Support delta/count to draw selection of points in xy plot
 + Improve group handling for key in xy plot
# 2/9/2019
 + add support for scrolling the plot key
 + support discrete palettes
 + improve support for optional types in properties
 + support modifiers in key show/select
 + add support for global title and column title in model
# 2/10/2019
 + support meta data for text data structure
 + fix some issues for plot types with no data
 + fit no data object draw for multiple plots
 + make tcl required to build
 + add distinct palettes and update gradient palette to display
 + make lines, points and color bar draw in gradient palette optional
 + improve handling of unset data range
 + use doxygen style for comments 
 + improve rect select when select triggers extra selection
 + add max rows and specified columns for CSV
# 2/17/2019
 + Cleanup data line edit widgets
 + Support hidden property view items
 + Use CQChartsUtil::toInt, toReal and CQChartsVariant::toInt, toReal in all conversions
 + Support tcl list in commands
 + Add more doxygen comments
# 2/24/2019
 + Add contrast text drawing for html text
 + Add Edit Dialog for Annotation, Title, Key and Axis
 + More Enum Editors
# 3/2/2019
 + support html text in title
 + add edit context menu item in tree to show edit dialog
 + clean up edit dialogs to support tabs
# 3/3/2019
 + Use separate property models for plots
# 3/4/2019
 + add tcl selection callbacks, selection query and plot selection signals
# 3/5/2019
 + add filter for plot properties
 + use CQLineEdit
# 3/9/2019
 + Add shortcut key to show/hide property tree filter
 + Add dialog to edit single model
 + add horizontal scroll support to key
 + fix issues whe scrolled key has title
 + make pixel calculations for margins more stable
# 3/17/2019
 + Add descriptions to properties
 + Support sub coloring of distribution bar with summed values
 + Fix box plot bar positioning with explicit x values
 + Add commands to create folded model, collapsed model, bucket model and stat model
# 3/24/2019
 + cleanup pixelToWindow and windowToPixel conversions
# 4/6/2019
 + Add more property descriptions
 + Get property list for view, plot, annotation in get_charts_data command
# 4/7/2019
 + Add CQChartsFont to support inheritance
# 4/15/2019
 + Add stats lines for xy and distribution plot
 + Add under/over flow buckets for distribution plot
 + Allow plot objects to be added to property tree
# 4/21/2019
 + Cleanup description text
 + rename plot queue routines
# 4/23/2019
 + Improve description text
 + Add better mouse feedback for distribution plot
# 4/24/2019
 + Support defaultPalette for plot and view when palette number not specified
 + Fix edit of palette points
# 4/25/2019
 + Add theme palette list editor
# 4/28/2019
 + Support different color interpolation types (set, group, index, x/y value)
 + Use view/plot interpColor where appropriate
 + Improve view settings theme editor
 + Support auto maximize/restore using scrolled plots
 + Update get_charts_palette and set_charts_palette commands
# 4/29/2019
 + Add Maximize/Restore for multiple plots
 + Add color type support to xy plot
 + Update select/inside fill color on theme change
# 5/2/2019
 + Allow forced label tick positions
 + Cleanup distribution plot type/value/empty/percent combinations
# 5/5/2019
 + Use ColorInd data for more plot objects and keys
 + Support plot x/y color stops to interpolate color from x/y value
# 5/6/2019
 + ColorInd cleanup
 + Key Header and Text support more text properties
# 5/12/2019
 + More ColorInd cleanup
 + Add more property description test
 + Rename some properties
 + Add user name for types and add to property view
# 5/19/2019
 + Improve plot tip descriptions using new HTML builder class
 + Improve tip widget to allow expand/collapse
 + Add -properties to create_charts_annotation commands
 + Update create_charts_annotation command options to use consistent option names (fill/stroke)
 + Swap axis data when plot orientation changed so style options are retained
 + Use more optional types for user customization properties
 + Add axis tick and grid line enums to replace booleans
 + Add probe support to scatter and geometry plot
 + Add separator scatter points style options
 + Add log support to scatter and barchart plots
 + Add more support for tip columns in plot object tooltip
 + Cleanup image style and options
 + More plot help descriptions
# 5/28/2019
 + Fix missing axis swap code
 + Use tcl format for command arguments (column_type and properties)
 + Improve descriptions
# 5/30/2019
 + Use tcl format for more column type for more code
 + Fix format for axes
# 6/2/2019
 + Move gradient/theme classes and editors into new CQColors/CQColorsEdit classes
 + Support color value by interpolation and by index
 + Improve view/plot/annotation write
 + Support marking properties as style and allow show/hide style properties
 + Tcl parsing support for color definition
# 6/8/2019
 + Update write code for mode input data and output annotation properties
 + Add safe user format from variant
 + Make some command options hidden
# 6/10/2019
 + Fix interp Hsv issue with gray color
 + Show colors in table delegate
# 6/17/2019
 + Improve write code
 + add mutex in tcl eval
# 6/23/2019
 + add help dialog
 + improve clipping and handling of small text in hier bubble plot and sunburst plot
# 6/30/2019
 + rename rect to rectangle
 + support size in tree map plot label
 + improve consistency between symbol fill and stroke shapes
# 7/7/2019
 + Support select and edit of view annotations and key
 + Fix border sides drawing in box object
 + Remove unused CQChartsDisplayTransform
# 7/16/2019
 + Add hidden parameters
# 7/18/2019
 + Fix missing data in write plot
 + Tree map text rendering improvements
 + XY show vector even if no points
# 7/21/2019
 + Improve conistency between xy and scatter plots
 + Support interp on xy smoothed line
 + Support images for xy and scatter symbols
# 7/24/2019
 + improve draw of color, symbol, font and column editors
# 7/28/2019
 + Add pivot model and plot
 + Support custom color/palette for coloring column background or barchart
# 7/30/2019
 + Common code for CQChartsPlotObj model indices
# 8/4/2019
 + Common code for drawing thin plot bars
 + Add image annotation and cleanup common code
 + Support checkable/checked for annotation
 + Move selection model tracking from plot to model data and share across all model views
 + Add match tcl function for process/search
 + Add auto placement support for key using quad tree
 + Improve pivot plot support when no y column
 + Move move common scatter/xy plot to point plot base class
 + Support full mapping for symbol size, symbol type and font in xy plot (from base class)
 + Use tab/splitter widget to replace some splitters
 + Add view layers to layers tab
 + Add tcl access to mouse data
 + Add tcl support for set hidden
# 8/11/2019
 + Move model code to CQBaseModel/CQModelUtil
# 8/12/2019
 + use CQTabSplit instead of CQDividedArea in plot/model dialog
 + add title to csv meta data
# 8/15/2019
 + fix issues when making model writable
 + add access to model properties from tcl
# 8/18/2019
 + support META comments in analyze file
 + support ENUM column type params
 + Add palette name meta type and editor
 + Add export model to csv with meta data
# 8/22/2019
 + adjust const for no update
# 8/25/2019
 + add rgb model support for CQChartsColor
 + add stops for table heatmap
# 9/22/2019
 + add support for abstract painter
 + use painter to support writing plot to javascript
# 9/24/2019
 + add support for different front/tail arrows
 + more javascript painter support
# 9/28/2019
 + improve arrow rendering and inside test
# 9/29/2019
 + add arrow type and update tcl command
