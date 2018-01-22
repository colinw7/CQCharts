# Adjacency

CQChartsTest \
 -tsv adjacency.tsv -comment_header \
 -type adjacency -columns "node=1,connections=3,name=0,group=2" \
 -plot_title "adjacency"
CQChartsTest \
 -csv adjacency.csv \
 -type adjacency -columns "name=0,value=1,group=2" \
 -plot_title "adjacency"

# Bar Chart

CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "category=0,value=1" -column_type "1#integer" \
 -plot_title "bar chart"

CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "category=0,value=1 2 3 4 5 6 7" \
 -plot_title "multiple bar chart" -column_type "1#integer"
CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "category=0,value=1 2 3 4 5 6 7" \
 -plot_title "multiple bar chart" -column_type "1#integer" \
 -properties "columns.rowGrouping=1"

CQChartsTest \
 -csv group.csv -comment_header \
 -type barchart -columns "category=1,value=2,name=0"

#--

CQChartsTest \
 -csv ages.csv -first_line_header \
 -type bar -columns "category=0,value=1" \
 -plot_title "bar chart" -column_type "1#integer"

CQChartsTest \
 -csv group_ages.csv -first_line_header \
 -type bar -columns "category=0,name=1,value=2" \
 -plot_title "grouped bar chart" -column_type "2#integer"
CQChartsTest \
 -csv group_ages.csv -first_line_header -fold 0 \
 -type bar -columns "category=1,value=2" \
 -plot_title "folded bar chart" -column_type "2#integer"

# Box Plot

CQChartsTest \
 -csv boxplot.csv -first_line_header \
 -type box -columns "x=0,y=2" \
 -plot_title "boxplot"

# Bubble/Hier Bubble Plot

CQChartsTest -json flare.json \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv flare.csv \
 -type bubble -columns "name=0,value=1" -column_type "1#real" \
 -plot_title "bubble"
CQChartsTest -csv flare.csv \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv pareto.csv -comment_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"

CQChartsTest -json flare.json \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"
CQChartsTest -csv flare.csv \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"
CQChartsTest -csv hier.csv -comment_header \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"
CQChartsTest -csv hier_files.csv \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hier files"
CQChartsTest -tsv coffee.tsv -first_line_header \
 -type hierbubble -columns "name=0,color=1" \
 -plot_title "coffee characteristics"

CQChartsTest -csv hier_order.csv -comment_header \
 -type hierbubble -columns "name=0,value=1,color=2" \
 -plot_title "hierarchical bubble"

CQChartsTest -csv book_revenue.csv -first_line_header \
 -type hierbubble -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"

# Chord Plot

CQChartsTest \
 -data chord-cities.data \
 -type chord -columns "name=0,group=1" \
 -plot_title "Chord Plot"
CQChartsTest \
 -csv chord-cities.csv \
 -type chord -columns "name=0,value=1,group=2" \
 -plot_title "Chord Plot"

# Delaunay Plot

CQChartsTest \
 -csv airports.csv -comment_header \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "delaunay"

# Distribution Plot

CQChartsTest \
 -csv name_value.csv \
 -type distribution -columns "value=0"

CQChartsTest \
 -csv gaussian.txt -comment_header \
 -type distribution -columns "value=0"
CQChartsTest \
 -csv gaussian.txt -comment_header \
 -type distribution -columns "value=0" \
 -bool "autoRange=0" -real "start=1.0,delta=0.1"
CQChartsTest \
 -csv distribution_sparse.csv -first_line_header \
 -type distribution -columns "value=0,color=1" \
 -real "autoRange=0,delta=1"

CQChartsTest \
 -csv gaussian.txt -comment_header \
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
 -tsv adjacency.tsv \
 -type forcedirected -columns "node=1,connections=3,name=0,group=2"

# Geometry Plot

CQChartsTest \
 -tsv states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
 -plot_title "geometry"

CQChartsTest \
 -tsv choropeth.tsv \
 -type geometry -columns "name=0,geometry=1,value=2" \
 -plot_title "choropeth"

# Image Plot

CQChartsTest -csv mandelbrot.csv -type image

# Parallel Plot

CQChartsTest \
 -csv parallel_coords.csv -first_line_header \
 -type parallel -columns "x=0,y=1 2 3 4 5 6 7" \
 -plot_title "parallel"

# Pie Chart

CQChartsTest \
 -csv ages.csv -first_line_header \
 -type pie -columns "label=0,data=1" \
 -plot_title "pie chart" -column_type "1#integer"

CQChartsTest \
 -csv group_ages.csv -first_line_header \
 -type pie -columns "label=1,data=2,group=0" \
 -plot_title "grouped pie chart" -column_type "2#integer"
CQChartsTest \
 -csv group_ages.csv -first_line_header -fold 0 \
 -type pie -columns "label=1,data=2" \
 -plot_title "folded pie chart" -column_type "2#integer"
CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type pie -columns "label=0,data=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart"
CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type pie -columns "label=0,data=1 2 3 4 5 6 7" \
 -plot_title "multi column pie chart" \
 -properties "columns.rowGrouping=1"

# Radar Plot

CQChartsTest \
  -csv radar.csv -first_line_header \
  -type radar -columns "name=0,value=1 2 3 4 5"

# Sankey

CQChartsTest \
 -csv sankey.csv -comment_header \
 -type sankey -columns "link=0,value=1"

CQChartsTest \
 -csv sankey_energy.csv -comment_header \
 -type sankey -columns "link=0,value=1"

# Scatter Plot

CQChartsTest \
 -tsv scatter.tsv -first_line_header \
 -type scatter -columns "name=4,x=0,y=1,symbolSize=2" \
 -plot_title "scatter" \
 -properties "symbol.size=12" \
 -view_properties "selectedHighlight.fill.enabled=1"

CQChartsTest \
 -csv bubble.csv -comment_header \
 -type scatter -columns "name=0,x=1,y=2,color=3,size=4" \
 -plot_title "Scatter Plot"

CQChartsTest \
 -tsv cities.dat -comment_header -process "+column(2)/20000.0" \
 -type scatter -columns "x=4,y=3,name=0,fontSize=5" \
 -bool "textLabels=1,key=0" \
 -properties "dataLabel.position=CENTER"

CQChartsTest \
 -csv airports.csv -comment_header \
 -type scatter -columns "x=6,y=5,name=1" \
 -plot_title "airports"

# Hier Scatter Plot

CQChartsTest \
 -csv airports.csv -comment_header \
 -type hierscatter -columns "x=6,y=5,name=1,group=4 3 2" \
 -plot_title "airports"

# Sunburst Plot

CQChartsTest -json flare.json \
 -type sunburst -columns "name=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -csv flare.csv \
 -type sunburst -columns "name=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -tsv coffee.tsv -first_line_header \
 -type sunburst -columns "name=0,color=1" \
 -plot_title "coffee characteristics" \
 -properties "multiRoot=1"
CQChartsTest -csv book_revenue.csv -first_line_header \
 -type sunburst -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"

# TreeMap Plot

CQChartsTest -json flare.json \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"
CQChartsTest -csv flare.csv \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"
CQChartsTest -tsv coffee.tsv -first_line_header \
 -type treemap -columns "name=0,color=1" \
 -plot_title "coffee characteristics"

CQChartsTest -csv hier_order.csv -comment_header \
 -type treemap -columns "name=0,value=1" \
 -plot_title "tree map"

# XY Plot

CQChartsTest \
 -csv airports.csv -comment_header \
 -type xy -columns "x=6,y=5,name=1" \
 -properties "lines.visible=0" \
 -plot_title "random xy"

CQChartsTest \
 -tsv bivariate.tsv -comment_header \
 -type xy -columns "x=0,y=1 2" \
 -column_type "time:format=%Y%m%d,oformat=%F" \
 -bivariate \
 -plot_title "bivariate" \
 -properties "yaxis.includeZero=1"

CQChartsTest \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "xy plot"

CQChartsTest \
 -tsv multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3" -column_type "time:format=%Y%m%d" \
 -plot_title "multiple xy plot"

CQChartsTest \
 -tsv stacked_area.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3 4 5" -column_type "time:format=%y-%b-%d" -stacked \
 -plot_title "stacked area"

CQChartsTest \
  -data missing.data -comment_header \
  -type xy -columns "x=0,y=1" \
  -plot_title "Missing Data"

CQChartsTest \
  -data xy_size.data -comment_header \
  -type xy -columns "x=0,y=1,size=2" \
  -properties "points.symbol=circle,points.fill.visible=1"

CQChartsTest \
 -expr -num_rows 50 \
 -process "+row(-10,10)" \
 -process "+sin(x)=sin(@1)" \
 -process "+atan(x)=atan(@1)" \
 -process "+cos(atan(x))=cos(atan(@1))" \
 -type xy -columns "x=1,y=2 3 4" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.location=tl,key.flipped=1" \
 -properties "title.text.font=+8"

CQChartsTest \
 -expr -num_rows 100 \
 -process "+row(-pi/2,pi)" \
 -process "+cos(x)=cos(@1)" \
 -process "+-(sin(x)>sin(x+1)?sin(x):sin(x+1))=-(sin(@1)>sin(@1+1)?sin(@1):sin(@1+1))" \
 -type xy -columns "x=1,y=2 3" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.flipped=1,key.border.visible=0" \
 -properties "title.text.font=+8"

CQChartsTest \
 -expr -num_rows 200 \
 -process "+row(-3,5)" \
 -process "+asin(x)=asin(@1)" \
 -process "+acos(x)=acos(@1)" \
 -type xy -columns "x=1,y=2 3" \
 -xmin -3 -xmax 5 \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.location=tl,key.flipped=1" \
 -properties "title.text.font=+8"

CQChartsTest \
 -expr -num_rows 500 \
 -process "+row(-5*pi,5*pi)" \
 -process "+tan(x)/atan(x)=tan(@1)/atan(@1)" \
 -process "+1/x=1/@1" \
 -type xy -columns "x=1,y=2 3" \
 -plot_title "Simple Plots" \
 -xmin -16 -xmax 16 -ymin -5 -ymax 5 \
 -properties "points.visible=0" \
 -properties "key.location=bc,key.insideY=0"
 -properties "title.text.font=+8"

CQChartsTest \
 -expr -num_rows 800 \
 -process "+row(-30,20)" \
 -process "+sin(x*20)*atan(x)=sin(@1*20)*atan(@1)" \
 -type xy -columns "x=1,y=2" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0" \
 -properties "key.location=bl,key.insideY=0,key.flipped=1"
 -properties "title.text.font=+8"

CQChartsTest \
 -data silver.dat \
 -type xy -columns "x=0,y=1 2" -bivariate \
 -properties "fillUnder.visible=1"

CQChartsTest \
 -expr -num_rows 100 \
 -process "+x=row(-10,10)" \
 -process "+abs(x)=abs(@1)" \
 -type xy -columns "x=1,y=2" \
 -properties "points.visible=0,fillUnder.visible=1,fillUnder.position=2 5"

#----

# Multiplot

CQChartsTest \
 -csv multi_bar.csv -first_line_header \
 -type bar -columns "name=0,value=1 2 3 4 5 6" -column_type "1#integer" \
-and \
 -csv ages.csv -first_line_header \
 -type pie -columns "label=0,data=1" \
 -plot_title "bar chart and pie"

CQChartsTest \
 -csv spline_area.csv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "0#integer;1#integer" -fillunder \
 -xintegral -ymin 0 \
 -plot_title "Spline Area Chart" \
 -properties "points.symbol=circle,points.fill.visible=1,yaxis.grid.line.major.visible=1" \
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
 -properties "points.symbol=circle,points.fill.visible=1,yaxis.grid.line.major.visible=1" \
-and \
 -csv country_wise_population.csv -comment_header \
 -type bar -columns "name=2,value=1" \
 -plot_title "Column Chart"

#---

# Y1Y2 Plots

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

CQChartsTest -y1y2 \
 -csv pareto.csv -comment_header \
 -type bar \
-and \
 -csv pareto.csv -comment_header \
 -type xy -cumulative -xmin -0.5 -xmax 5.5 -ymin 2.0 -xintegral \
 -plot_title "pareto"

#---

# Overlay Plots

CQChartsTest -overlay \
 -tsv states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
-and \
 -csv airports.csv -comment_header \
 -type delaunay -columns "x=6,y=5,name=1" \
 -plot_title "states and airports"

CQChartsTest -overlay \
 -data 1.dat -type xy \
 -bool "impulse=1" \
 -properties "points.visible=0,lines.visible=0" \
 -properties "dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1,xaxis.ticks.minor.visible=0" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1,yaxis.ticks.minor.visible=0" \
 -properties "key.location=bl,key.insideY=0,key.horizontal=1" \
-and \
 -data 2.dat -type xy \
 -properties "lines.visible=0" \
-and \
 -data 3.dat -type xy \
 -properties "points.visible=0" \
 -ymin -10 -ymax 10

CQChartsTest -overlay \
 -expr -num_rows 100 \
 -process "+row(-10,10)=row(-10,10)" \
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
 -process "+row(-10,10)=row(-10,10)" \
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
 -process "+x=row(-10,10)" \
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
 -process "+x=row(0,10)" \
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
 -data silver.dat \
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
 -properties "lines.width=2,lines.color=black" \
 -plot_title "curve 1" \
-and \
 -type xy -columns "x=0,y=3" \
 -properties "lines.width=2,lines.color=cyan" \
 -plot_title "curve 2"

CQChartsTest -overlay \
 -csv group.csv -comment_header \
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
 -csv log.csv -comment_header \
 -type xy -ylog \
 -properties "lines.width=2,points.symbol=circle,points.fill.visible=1" \
 -properties "lines.color=palette:0.25,points.fill.color=palette:0.25" \
 -plot_title "Log Scale" \
-and \
 -csv log.csv -comment_header \
 -type xy \
 -properties "lines.width=2,points.symbol=circle,points.fill.visible=1" \
 -properties "lines.color=palette:0.75,points.fill.color=palette:0.75" \
 -plot_title "Linear Scale"

#----

# Performance

CQChartsTest \
 -csv xy_10000.csv -first_line_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "10000 points" \
 -properties "points.visible=0"
CQChartsTest \
 -csv xy_10000.csv -first_line_header \
 -type bar -columns "x=0,y=1" \
 -plot_title "10000 points" \
 -properties "stroke.visible=0"
CQChartsTest \
 -csv xy_10000.csv -first_line_header \
 -type scatter -columns "x=0,y=1" \
 -plot_title "10000 points"
CQChartsTest \
 -csv random_10000.csv -first_line_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "10000 points"

CQChartsTest \
 -csv xy_100000.csv -first_line_header \
 -type xy -columns "x=0,y=1" \
 -plot_title "100000 points" \
 -properties "lines.visible=0"
CQChartsTest \
 -csv xy_100000.csv -first_line_header \
 -type bar -columns "x=0,y=1" \
 -plot_title "100000 points" \
 -properties "stroke.visible=0"
CQChartsTest \
 -csv xy_100000.csv -first_line_header \
 -type scatter -columns "x=0,y=1" \
 -plot_title "100000 points"
CQChartsTest \
 -csv xy_100000_positive.csv -first_line_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "100000 points"
