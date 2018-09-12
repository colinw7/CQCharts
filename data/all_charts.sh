# Bar Chart

CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type barchart -columns "group=0,value=1" -column_type "1#integer" \
 -plot_title "bar chart"

CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3 4 5 6 7" \
 -plot_title "multiple bar chart" -column_type "1#integer"
CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3 4 5 6 7" \
 -plot_title "multiple bar chart" -column_type "1#integer" \
 -properties "grouping.rowGrouping=1"

CQChartsTest \
 -csv data/group.csv -comment_header \
 -type barchart -columns "group=1,value=2,name=0"

#--

CQChartsTest \
 -csv data/ages.csv -first_line_header \
 -type barchart -columns "group=0,value=1" \
 -plot_title "bar chart" -column_type "1#integer"

CQChartsTest \
 -csv data/group_ages.csv -first_line_header \
 -type barchart -columns "group=0,name=1,value=2" \
 -plot_title "grouped bar chart" -column_type "2#integer"
CQChartsTest \
 -csv data/group_ages.csv -first_line_header -fold 0 \
 -type barchart -columns "group=1,value=2" \
 -plot_title "folded bar chart" -column_type "2#integer"

# Box Plot

CQChartsTest \
 -csv data/boxplot.csv -first_line_header \
 -type box -columns "group=0,value=2" \
 -plot_title "boxplot"

# Bubble/Hier Bubble Plot

CQChartsTest -json data/flare.json \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv data/flare.csv -comment_header \
 -type bubble -columns "name=0,value=1" -column_type "1#real" \
 -plot_title "bubble"
CQChartsTest -csv data/flare.csv -comment_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv data/pareto.csv -comment_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"

CQChartsTest -json data/flare.json \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble (hier data)"
CQChartsTest -csv data/flare.csv -comment_header \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble (flat data)"
CQChartsTest -csv data/hier.csv -comment_header \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"
CQChartsTest -csv data/hier_files.csv \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hier files"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type hierbubble -columns "name=0,color=1" \
 -plot_title "coffee characteristics"

CQChartsTest -csv data/hier_order.csv -comment_header \
 -type hierbubble -columns "name=0,value=1,color=2" \
 -plot_title "hierarchical bubble"

CQChartsTest -csv data/book_revenue.csv -first_line_header \
 -type hierbubble -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"

# Chord Plot

CQChartsTest \
 -data data/chord-cities.data \
 -type chord -columns "link=0,group=1" \
 -plot_title "Chord Plot"
CQChartsTest \
 -csv data/chord-cities.csv \
 -type chord -columns "link=0,value=1,group=2" \
 -plot_title "Chord Plot"

# Delaunay Plot

CQChartsTest \
 -csv data/airports.csv -comment_header \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "delaunay"

# Dendrogram

CQChartsTest -csv data/flare.csv -comment_header \
 -type dendrogram -columns "name=0,value=1" \
 -plot_title "dendrogram"
CQChartsTest -json data/flare.json \
 -type dendrogram -columns "name=0,value=1" \
 -plot_title "dendrogram"

# Distribution Plot

CQChartsTest \
 -csv data/name_value.csv \
 -type distribution -columns "value=0"

CQChartsTest \
 -csv data/gaussian.txt -comment_header \
 -type distribution -columns "value=0"
CQChartsTest \
 -csv data/gaussian.txt -comment_header \
 -type distribution -columns "value=0" \
 -bool "autoRange=0" -real "start=1.0,delta=0.1"
CQChartsTest \
 -csv data/distribution_sparse.csv -first_line_header \
 -type distribution -columns "value=0,color=1" \
 -real "autoRange=0,delta=1"

CQChartsTest \
 -csv data/gaussian.txt -comment_header \
 -process-add "Color=@0 > 0.0 ? 'green' : 'red'" \
 -type distribution -columns "value=0,color=1" \
 -plot_title "distribution chart" \
 -view_properties "selectedHighlight.fill.enabled=1" \
 -properties "xaxis.ticks.label.placement=BETWEEN" \
 -properties "yaxis.ticks.label.placement=TOP_RIGHT" \
 -properties "key.visible=0" \
 -properties "dataLabel.visible=1,dataLabel.position=TOP_OUTSIDE"
  
# Force Directed Plot

CQChartsTest \
 -tsv data/adjacency.tsv \
 -type forcedirected -columns "node=1,connections=3,name=0,groupId=2"
CQChartsTest \
 -csv data/adjacency.csv \
 -type forcedirected -columns "namePair=0,count=1,groupId=2" \
 -plot_title "adjacency"

# Geometry Plot

CQChartsTest \
 -tsv data/states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
 -plot_title "geometry"

CQChartsTest \
 -tsv data/choropeth.tsv \
 -type geometry -columns "name=0,geometry=1,value=2" \
 -plot_title "choropeth"

# Image Plot

CQChartsTest -csv data/mandelbrot.csv -type image

# Parallel Plot

CQChartsTest \
 -csv data/parallel_coords.csv -first_line_header \
 -type parallel -columns "x=0,y=1 2 3 4 5 6 7" \
 -plot_title "parallel"

# Radar Plot

CQChartsTest \
 -csv data/radar.csv -first_line_header \
 -type radar -columns "name=0,value=1 2 3 4 5"

# Sankey

CQChartsTest \
 -csv data/sankey.csv -comment_header \
 -type sankey -columns "link=0,value=1"

CQChartsTest \
 -csv data/sankey_energy.csv -comment_header \
 -type sankey -columns "link=0,value=1"

# Hier Scatter Plot

CQChartsTest \
 -csv data/airports.csv -comment_header \
 -type hierscatter -columns "x=6,y=5,name=1,group=4 3 2" \
 -plot_title "airports"

# Sunburst Plot

CQChartsTest -json data/flare.json \
 -type sunburst -columns "name=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -csv data/flare.csv -comment_header \
 -type sunburst -columns "name=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type sunburst -columns "name=0,color=1" \
 -plot_title "coffee characteristics" \
 -properties "multiRoot=1"
CQChartsTest -csv data/book_revenue.csv -first_line_header \
 -type sunburst -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"

# TreeMap Plot

CQChartsTest -json data/flare.json \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"
CQChartsTest -csv data/flare.csv -comment_header \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type treemap -columns "name=0,color=1" \
 -plot_title "coffee characteristics"

CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type treemap -columns "name=0,color=1" \
 -plot_title "coffee characteristics" \
 -column_type "1#color"

CQChartsTest -csv data/hier_order.csv -comment_header \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"

# XY Plot

CQChartsTest \
 -csv data/airports.csv -comment_header \
 -type xy -columns "x=6,y=5,name=1" \
 -properties "lines.visible=0" \
 -plot_title "random xy"

CQChartsTest \
 -tsv data/bivariate.tsv -comment_header \
 -type xy -columns "x=0,y=1 2" -column_type "time:format=%Y%m%d,oformat=%F" \
 -bivariate \
 -plot_title "bivariate" \
 -properties "yaxis.includeZero=1"

CQChartsTest \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "xy plot"

CQChartsTest \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3" -column_type "time:format=%Y%m%d" \
 -plot_title "multiple xy plot"

CQChartsTest \
 -tsv data/stacked_area.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3 4 5" -column_type "time:format=%y-%b-%d" -stacked \
 -plot_title "stacked area"

CQChartsTest \
 -data data/missing.data -comment_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "Missing Data"

CQChartsTest \
 -data data/xy_size.data -comment_header \
 -type xy -columns "x=0,y=1,size=2" \
 -properties "points.symbol=circle,points.fill.visible=1"

CQChartsTest \
 -data data/silver.dat \
 -type xy -columns "x=0,y=1 2" -bivariate \
 -properties "fillUnder.visible=1"

CQChartsTest \
 -expr -num_rows 100 \
 -process "+x=map(-10,10)" \
 -process "+abs(x)=abs(@1)" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=2 5"

#----

# Multiplot

CQChartsTest \
 -csv data/multi_bar.csv -first_line_header \
 -type barchart -columns "name=0,value=1 2 3 4 5 6" -column_type "1#integer" \
-and \
 -csv data/ages.csv -first_line_header \
 -type pie -columns "label=0,data=1" \
 -plot_title "bar chart and pie"

CQChartsTest \
 -csv data/spline_area.csv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "0#integer;1#integer" -fillunder \
 -xintegral -ymin 0 \
 -plot_title "Spline Area Chart" \
 -properties "points.symbol=circle,points.fill.visible=1,yaxis.grid.line.major.visible=1" \
-and \
 -csv data/console.csv -comment_header \
 -type pie -columns "label=2,data=0,keyLabel=1" -column_type "0#integer" \
 -plot_title "Pie Chart" \
 -properties "label.radius=1.2,startAngle=0.0" \
-and \
 -csv data/lines.csv -comment_header \
 -type xy -columns "x=0,y=1,pointLabel=2,pointColor=3,pointSymbol=4" \
 -column_type "0#time:format=%Y%m%d,oformat=%b" \
 -plot_title "Line Chart" \
 -properties "points.symbol=circle,points.fill.visible=1,yaxis.grid.line.major.visible=1" \
-and \
 -csv data/country_wise_population.csv -comment_header \
 -type barchart -columns "name=2,value=1" \
 -plot_title "Column Chart"

#---

# Y1Y2 Plots

CQChartsTest -y1y2 \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d" \
-and \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=2" -column_type "time:format=%Y%m%d" \
 -plot_title "multiple y axis"

CQChartsTest -y1y2 \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
-and \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=2" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "multiple y axis"

CQChartsTest -y1y2 \
 -csv data/pareto.csv -comment_header \
 -type barchart \
-and \
 -csv data/pareto.csv -comment_header \
 -type xy -cumulative -xmin -0.5 -xmax 5.5 -ymin 2.0 -xintegral \
 -plot_title "pareto"

#---

# Overlay Plots

CQChartsTest -overlay \
 -tsv data/states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
-and \
 -csv data/airports.csv -comment_header \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "states and airports"

CQChartsTest -overlay \
 -data data/1.dat -type xy \
 -bool "impulse=1" \
 -properties "points.visible=0,lines.visible=0" \
 -properties "dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1,xaxis.ticks.minor.visible=0" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1,yaxis.ticks.minor.visible=0" \
 -properties "key.location=bl,key.insideY=0,key.horizontal=1" \
-and \
 -data data/2.dat -type xy \
 -properties "lines.visible=0" \
-and \
 -data data/3.dat -type xy \
 -properties "points.visible=0" \
 -ymin -10 -ymax 10

CQChartsTest -overlay \
 -expr -num_rows 100 \
 -process "+map(-10,10)=map(-10,10)" \
 -process "+1.5+sin(x)/x=1.5+sin(@1)/@1" \
 -process "+sin(x)/x=sin(@1)/@1" \
 -process "+1+sin(x)/x=1+sin(@1)/@1" \
 -process "+-1+sin(x)/x=-1+sin(@1)/@1" \
 -process "+-2.5+sin(x)/x=-2.5+sin(@1)/@1" \
 -process "+-4.3+sin(x)/x=-4.3+sin(@1)/@1" \
 -process "+(x>3.5 ? x/3-3 : 1/0)=(@1>3.5 ? @1/3-3 : 1/0)" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=max" \
 -xmin -10 -xmax 10 -ymin -5 -ymax 3 \
-and \
 -type xy -columns "x=1,y=3" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=0.0" \
-and \
 -type xy -columns "x=1,y=4" \
 -properties "points.visible=0" \
-and \
 -type xy -columns "x=1,y=5" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=-2" \
-and \
 -type xy -columns "x=1,y=6" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=-5 -4" \
-and \
 -type xy -columns "x=1,y=7" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=min" \
-and \
 -type xy -columns "x=1,y=8" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=-1.8333"

CQChartsTest -overlay \
 -expr -num_rows 100 \
 -process "+map(-10,10)=map(-10,10)" \
 -process "+x*x=@1*@1" \
 -process "+50-x*x=50-@1*@1" \
 -process "+x*x=@1*@1" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=max" \
-and \
 -type xy -columns "x=1,y=3" \
 -properties "points.visible=0,fillUnder.visible=1" \
-and \
 -type xy -columns "x=1,y=4" \
 -properties "points.visible=0"

CQChartsTest -overlay \
 -expr -num_rows 100 \
 -process "+x=map(-10,10)" \
 -process "+2+sin(x)**2=2+sin(@1)**2" \
 -process "+cos(x)**2=cos(@1)**2" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=min" \
 -properties "yaxis.grid.line.major.visible=1" \
-and \
 -type xy -columns "x=1,y=3" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=min"

CQChartsTest -overlay \
 -expr -num_rows 100 \
 -process "+x=map(0,10)" \
 -process "+-8=-8" \
 -process "+sqrt(x)=sqrt(@1)" \
 -process "+sqrt(10-x)-4.5=sqrt(10-@1)-4.5" \
 -plot_title "Some sqrt stripes on filled graph background" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=max" \
-and \
 -type xy -columns "x=1,y=3" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=-0.5" \
-and \
 -type xy -columns "x=1,y=4" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=-5.5" \
 -ymin -8 -ymax 6

CQChartsTest -overlay \
 -view_title "Fill area between two curves (above/below)" \
 -data data/silver.dat \
 -process "+@2+@0/50.0" \
 -type xy -columns "x=0,y=1 3" -bivariate \
 -properties "fillUnder.visible=1,fillUnder.side=above,lines.visible=0" \
 -plot_title "Above" \
 -xmin 250 -xmax 500 -ymin 5 -ymax 30 \
-and \
 -type xy -columns "x=0,y=1 3" -bivariate \
 -properties "fillUnder.visible=1,fillUnder.side=below,lines.visible=0" \
 -plot_title "Below" \
-and \
 -type xy -columns "x=0,y=1" \
 -properties "lines.width=2px,lines.color=black" \
 -plot_title "curve 1" \
-and \
 -type xy -columns "x=0,y=3" \
 -properties "lines.width=2px,lines.color=cyan" \
 -plot_title "curve 2"

CQChartsTest -overlay \
 -csv data/group.csv -comment_header \
 -type barchart -where "layer:METAL1" -columns "value=2" \
-and \
 -type barchart -where "layer:METAL2" -columns "value=2" \
-and \
 -type barchart -where "layer:METAL3" -columns "value=2" \
-and \
 -type barchart -where "layer:METAL4" -columns "value=2"

##---

# Log Plot

CQChartsTest -y1y2 \
 -view_title "Growth in Internet Users Globally" \
 -csv data/log.csv -comment_header \
 -type xy -ylog \
 -properties "lines.width=2px,points.symbol=circle,points.fill.visible=1" \
 -properties "lines.color=palette:0.25,points.fill.color=palette:0.25" \
 -plot_title "Log Scale" \
-and \
 -csv data/log.csv -comment_header \
 -type xy \
 -properties "lines.width=2px,points.symbol=circle,points.fill.visible=1" \
 -properties "lines.color=palette:0.75,points.fill.color=palette:0.75" \
 -plot_title "Linear Scale"

#----

CQChartsTest \
 -csv data/money_bar.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3 4" \
 -bool "horizontal=1"
CQChartsTest \
 -csv data/money_bar_neg.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3" \
 -bool "horizontal=1" \
 -properties "dataLabel.visible=1,dataLabel.position=TOP_OUTSIDE" \
 -properties "xaxis.format=real:format=%T"
CQChartsTest \
 -csv data/money_bar_stack.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3 4" \
 -bool "horizontal=1,stacked=1"

CQChartsTest \
 -csv data/temp_bar_range.csv -first_line_header \
 -type barchart -columns "group=0,value=2 3" \
 -bool "rangeBar=1,horizontal=1" \
 -properties "dataLabel.visible=1,dataLabel.position=TOP_OUTSIDE" \
 -properties "key.visible=0,options.barMargin=12px,fill.color=palette:0.2"

#----

CQChartsTest \
 -csv data/money_bar_stack_neg.csv -first_line_header \
 -type barchart -columns "group=0,value=1 2 3 4" \
 -bool "stacked=1,percent=1"
