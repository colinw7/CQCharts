CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1" -first_line_header -plot_title "bar chart" -column_type "1#integer"

CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1 2 3 4 5 6" -first_line_header -plot_title "multiple bar chart" -column_type "1#integer"

CQChartsTest -csv ages.csv -plot pie -column "label=0" -column "data=1" -first_line_header -plot_title "pie chart" -column_type "1#integer"

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -column "name=1" -plot_title "random xy"

CQChartsTest -csv airports.csv -plot delaunay -x 6 -y 5 -column "name=1" -plot_title "delaunay"

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -column_type "time:format=%Y%m%d" -bivariate -plot_title "bivariate" -comment_header

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2 -plot_title "boxplot" -first_line_header

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y 1 -column_type "time:format=%Y%m%d" -comment_header -plot_title "xy plot"

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -column_type "time:format=%Y%m%d" -comment_header -plot_title "multiple xy plot"

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7" -plot_title "parallel"

CQChartsTest -tsv scatter.tsv -plot scatter -column "name=4" -x 0 -y 1 -column "size=2" -first_line_header -plot_title "scatter"

CQChartsTest -tsv stacked_area.tsv -plot xy -x 0 -y "1 2 3 4 5" -comment_header -column_type "time:format=%y-%b-%d" -stacked -plot_title "stacked area"

CQChartsTest -json flare.json -plot sunburst -column "name=0" -column "value=1" -plot_title "sunburst"

CQChartsTest -json flare.json -plot bubble -column "name=0" -column "value=1" -plot_title "bubble"

CQChartsTest -json flare.json -plot hierbubble -column "name=0" -column "value=1" -plot_title "hierarchical bubble"

CQChartsTest -tsv states.tsv -plot geometry -column "name=0" -column "geometry=1" -comment_header "geometry"

CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1 2 3 4 5 6" -first_line_header -column_type "1#integer" -and -csv ages.csv -plot pie -column "label=0" -column "data=1" -plot_title "bar chart and pie"

CQChartsTest -overlay -tsv states.tsv -plot geometry -column "name=0" -column "geometry=1" -comment_header -and -csv airports.csv -plot delaunay -x 6 -y 5 -column "name=1" -plot_title "states and airports"

CQChartsTest -tsv choropeth.tsv -plot geometry -column "name=0" -column "geometry=1" -column "value=2" -plot_title "choropeth"

CQChartsTest -tsv adjacency.tsv -plot adjacency -column "node=1" -column "connections=3" -column "name=0" -column "group=2" -plot_title "adjacency"

CQChartsTest -csv xy_10000.csv -plot xy -x 0 -y 1 -plot_title "10000 points" -first_line_header

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -column_type "time:format=%Y%m%d" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -column_type "time:format=%Y%m%d" -comment_header -plot_title "multiple y axis"

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -column_type "time:format=%Y%m%d,oformat=%F" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -column_type "time:format=%Y%m%d,oformat=%F" -comment_header -plot_title "multiple y axis"

CQChartsTest -json flare.json -plot treemap -column "name=0" -column "value=1" -plot_title "tree map"

CQChartsTest -y1y2 -csv pareto.csv -plot bar -comment_header -and -csv pareto.csv -plot xy -comment_header -cumulative -xmin -0.5 -xmax 5.5 -ymin2 0 -xintegral -plot_title "pareto"

CQChartsTest -data missing.data -x 0 -y 1 -plot xy

CQChartsTest -data xy_size.tsv -x 0 -y 1 -column "size=2" -plot xy -comment_header

#---

CQChartsTest \
-csv spline_area.csv -comment_header \
  -plot xy -column "x=0" -column "y=1" -column_type "0#integer;1#integer" -fillunder \
  -xintegral -ymin 0 -plot_title "Spline Area Chart" \
  -properties "points.symbol=circle,points.filled=1,Y Axis.grid.displayed=1" \
-and \
-csv console.csv -comment_header \
  -plot pie -column "label=2" -column "data=0" -column "keyLabel=1" -column_type "data#integer" \
  -plot_title "Pie Chart" \
  -properties "labelRadius=1.2,startAngle=0.0" \
-and \
-csv lines.csv -comment_header \
  -plot xy -column "x=0" -column "y=1" \
  -column "pointLabel=2" -column "pointColor=3" -column "pointSymbol=4" \
  -column_type "0#time:format=%Y%m%d,oformat=%b" \
  -plot_title "Line Chart" \
  -properties "points.symbol=circle,points.filled=1,Y Axis.grid.displayed=1" \
-and \
-csv country_wise_population.csv -comment_header \
  -plot bar -column "name=2" -column "value=1" \
  -plot_title "Column Chart"
