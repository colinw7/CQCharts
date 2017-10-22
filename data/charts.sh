CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "name=0,value=1" -column_type "1#integer" \
 -plot_title "bar chart"

CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "name=0,value=1 2 3 4 5 6" \
 -plot_title "multiple bar chart" -column_type "1#integer"

CQChartsTest \
 -csv ages.csv -first_line_header \
 -type pie -columns "label=0,data=1" \
 -plot_title "pie chart" -column_type "1#integer"

CQChartsTest \
 -csv airports.csv \
 -type xy -columns "x=6,y=5,name=1" \
 -plot_title "random xy"

CQChartsTest \
 -csv airports.csv \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "delaunay"

CQChartsTest \
 -tsv bivariate.tsv -comment_header \
 -type xy -columns "x=0,y=1 2" \
 -column_type "time:format=%Y%m%d,oformat=%F" \
 -bivariate \
 -plot_title "bivariate"

CQChartsTest \
 -csv boxplot.csv -first_line_header \
 -type box -columns "x=0,y=2" \
 -plot_title "boxplot"

CQChartsTest \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "xy plot"

CQChartsTest \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3" -column_type "time:format=%Y%m%d" \
 -plot_title "multiple xy plot"

CQChartsTest \
 -csv parallel_coords.csv -first_line_header \
 -type parallel -columns "x=0,y=1 2 3 4 5 6 7" \
 -plot_title "parallel"

CQChartsTest \
 -tsv scatter.tsv -first_line_header \
 -type scatter -columns "name=4,x=0,y=1,size=2" \
 -plot_title "scatter"

CQChartsTest \
 -tsv stacked_area.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3 4 5" -column_type "time:format=%y-%b-%d" -stacked \
 -plot_title "stacked area"

CQChartsTest -json flare.json \
 -type sunburst -columns "name=0,value=1" \
 -plot_title "sunburst"

CQChartsTest -json flare.json \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"

CQChartsTest -json flare.json \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"

CQChartsTest \
 -tsv states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
 -plot_title "geometry"

CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "name=0,value=1 2 3 4 5 6" -column_type "1#integer" -and \
 -csv ages.csv \
 -type pie -columns "label=0,data=1" \
 -plot_title "bar chart and pie"

CQChartsTest -overlay \
 -tsv states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
-and \
 -csv airports.csv \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "states and airports"

CQChartsTest \
 -tsv choropeth.tsv \
 -type geometry -columns "name=0,geometry=1,value=2" \
 -plot_title "choropeth"

CQChartsTest \
 -tsv adjacency.tsv \
 -type adjacency -columns "node=1,connections=3,name=0,group=2" \
 -plot_title "adjacency"

CQChartsTest \
 -csv xy_10000.csv -first_line_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "10000 points"

CQChartsTest -y1y2 \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d" \
-and \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=2" -column_type "time:format=%Y%m%d" \
 -plot_title "multiple y axis"

CQChartsTest -y1y2 \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
-and \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=2" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "multiple y axis"

CQChartsTest -json flare.json \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"

CQChartsTest -y1y2 \
 -csv pareto.csv -comment_header \
 -type bar \
-and \
 -csv pareto.csv -comment_header \
 -type xy -cumulative -xmin -0.5 -xmax 5.5 -ymin2 0 -xintegral \
 -plot_title "pareto"

CQChartsTest \
  -data missing.data -comment_header \
  -type xy -columns "x=0,y=1" \
  -plot_title "Missing Data"

CQChartsTest \
  -data xy_size.data -comment_header \
  -type xy -columns "x=0,y=1,size=2" \
  -properties "points.symbol=circle,points.filled=1"

#---

CQChartsTest \
 -csv spline_area.csv -comment_header \
  -type xy -columns "x=0,y=1" -column_type "0#integer;1#integer" -fillunder \
  -xintegral -ymin 0 \
  -plot_title "Spline Area Chart" \
  -properties "points.symbol=circle,points.filled=1,Y Axis.grid.displayed=1" \
-and \
 -csv console.csv -comment_header \
  -type pie -columns "label=2,data=0,keyLabel=1" -column_type "0#integer" \
  -plot_title "Pie Chart" \
  -properties "labelRadius=1.2,startAngle=0.0" \
-and \
 -csv lines.csv -comment_header \
  -type xy -columns "x=0,y=1,pointLabel=2,pointColor=3,pointSymbol=4" \
  -column_type "0#time:format=%Y%m%d,oformat=%b" \
  -plot_title "Line Chart" \
  -properties "points.symbol=circle,points.filled=1,Y Axis.grid.displayed=1" \
-and \
 -csv country_wise_population.csv -comment_header \
  -type bar -columns "name=2,value=1" \
  -plot_title "Column Chart"

CQChartsTest \
 -csv group.csv -comment_header \
 -type barchart -columns "category=1,value=2,name=0"

#---

CQChartsTest \
 -csv bubble.csv -comment_header \
 -type scatter -columns "name=0,x=1,y=2,color=3,size=4" \
 -plot_title "Scatter Plot"

#---

CQChartsTest \
  -data chord-cities.data \
  -type chord -columns "name=0,group=1" \
  -plot_title "Chord Plot"

#---

CQChartsTest \
 -tsv adjacency.tsv \
 -type forcedirected -columns "node=1,connections=3,name=0,group=2"

#---

CQChartsTest \
 -tsv cities.dat -comment_header -process "+column(2)/20000.0" \
 -type scatter -columns "x=4,y=3,name=0,fontSize=5" -bool "textLabels=1,key=0" \
 -properties "dataLabel.position=CENTER"
