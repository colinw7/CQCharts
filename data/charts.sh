CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1" -first_line_header -plot_title "bar chart" -format ";integer"

CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1 2 3 4 5 6" -first_line_header -plot_title "multiple bar chart" -format ";integer"

CQChartsTest -csv ages.csv -plot pie -column "label=0" -column "data=1" -first_line_header -plot_title "pie chart" -format ";integer"

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -column "name=1" -plot_title "random xy"

CQChartsTest -csv airports.csv -plot delaunay -x 6 -y 5 -column "name=1" -plot_title "delaunay"

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -format "time:format=%Y%m%d" -bivariate -plot_title "bivariate" -comment_header

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2 -plot_title "boxplot" -first_line_header

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d" -comment_header -plot_title "xy plot"

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -format "time:format=%Y%m%d" -comment_header -plot_title "multiple xy plot"

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7" -plot_title "parallel"

CQChartsTest -tsv scatter.tsv -plot scatter -column "name=4" -x 0 -y 1 -column "size=2" -first_line_header -plot_title "scatter"

CQChartsTest -tsv stacked_area.tsv -plot xy -x 0 -y "1 2 3 4 5" -comment_header -format "time:format=%y-%b-%d" -stacked -plot_title "stacked area"

CQChartsTest -json flare.json -plot sunburst -column "name=0" -column "value=1" -plot_title "sunburst"

CQChartsTest -json flare.json -plot bubble -column "name=0" -column "value=1" -plot_title "bubble"

CQChartsTest -json flare.json -plot hierbubble -column "name=0" -column "value=1" -plot_title "hierarchical bubble"

CQChartsTest -tsv states.tsv -plot geometry -column "name=0" -column "geometry=1" -comment_header "geometry"

CQChartsTest -csv multi_bar.csv -plot bar -column "name=0" -column "value=1 2 3 4 5 6" -first_line_header -format ";integer" -and -csv ages.csv -plot pie -column "label=0" -column "data=1" -plot_title "bar chart and pie"

CQChartsTest -overlay -tsv states.tsv -plot geometry -column "name=0" -column "geometry=1" -comment_header -and -csv airports.csv -plot delaunay -x 6 -y 5 -column "name=1" -plot_title "states and airports"

CQChartsTest -tsv choropeth.tsv -plot geometry -column "name=0" -column "geometry=1" -column "value=2" -plot_title "choropeth"

CQChartsTest -tsv adjacency.tsv -plot adjacency -column "node=1" -column "connections=3" -column "name=0" -column "group=2" -plot_title "adjacency"

CQChartsTest -csv xy_10000.csv -plot xy -x 0 -y 1 -plot_title "10000 points" -first_line_header

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -format "time:format=%Y%m%d" -comment_header -plot_title "multiple y axis"

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d,oformat=%F" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -format "time:format=%Y%m%d,oformat=%F" -comment_header -plot_title "multiple y axis"

CQChartsTest -json flare.json -plot treemap -column "name=0" -column "value=1" -plot_title "tree map"

CQChartsTest -y1y2 -csv pareto.csv -plot bar -comment_header -and -csv pareto.csv -plot xy -comment_header -cumulative -xmin -0.5 -xmax 5.5 -ymin2 0 -xintegral -plot_title "pareto"

CQChartsTest -data missing.data -x 0 -y 1 -plot xy

CQChartsTest -data xy_size.tsv -x 0 -y 1 -column "size=2" -plot xy -comment_header
