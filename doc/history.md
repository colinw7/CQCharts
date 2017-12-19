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
