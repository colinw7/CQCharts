# CQCharts

Qt charting library to support displaying a interactive chart from an
QAbstractItemModel with support for cross selection using a
QItemSelectionModel.

Charts are live in that changes to the data model automatically update
the chart which allows the programmer to build interactive charts
where user can select items from the model using a table/tree view and/or
the display chart data.

Chart types supported 
  + Adjacency
    + tabular connectivity table (needs special column value syntax for connections)
    ![adjacency](adjacency.png "Adjacency Plot")
  + BarChart
    + 1d bar chart for column value
    ![barchart](barchart.png "Bar Chart")
  + Box
    + Box plot of agregated values from value and group columns
    ![boxplot](boxplot.png "Box Plot")
  + Bubble
    + Bubble plot where circle is size of column value
    ![bubble](bubbleplot.png "Bubble Plot")
  + Chord
    + Circlular connectivity chart (needs special column value syntax for connections)
    ![chord](chordplot.png "Chord Plot")
  + Delaunay
    + Delaunay/Voronoi geometric connectivity chart
    ![delaunay](delaunay.png "Delaunay Plot")
  + Distribution
    + Distribution count of range based values
    ![distribution](distribution.png "Distribution Plot")
  + ForceDirected
    + Force directed connectivity chart
    ![forcedirected](forcedirected.png "Force Directed Plot")
  + Geometry
    + General polygon geometry colored by value
    ![geometry](geometryplot.png "Geometry Plot")
  + HierBubble
    + Hierarchical bubble plot where circle is size of column value
    ![hierbubble](hierbubble.png "Hierarchical Bubble Plot")
  + Image
    + Image plot (x, y, color)
    ![image](imageplot.png "Image Plot")
  + Parallel
    + Parallel lines plots multiple overlaid value sets
    ![parallel](parallelplot.png "Parallel Plot")
  + Pie
    + Pie chart
    ![piechart](piechart.png "Pie Chart")
  + Radar
    + Radar (polygon pie chart)
    ![radar](radar.png "Radar Plot")
  + Sankey
    + Sankey plot of connected values
    ![sankey](sankey.png "Sankey Plot")
  + Scatter
    + Scatter plot of disparate values
    ![scatterplot](scatterplot.png "Scatter Plot")
  +  Sunburst
    + Hierarchical pie chart plot
    ![sunburst](sunburst.png "Sunburst Plot")
  + TreeMap
    + Hierarchical tree map
    ![treemap](treemap.png "Treemap Plot")
  + XY
    + x/y monotic value plot
    ![xychart](xychart.png "XY Chart")
