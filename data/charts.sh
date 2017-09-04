CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y 1 -first_line_header -plot_title "bar char"

CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6" -first_line_header -plot_title "multiple bar chart"

CQChartsTest -csv ages.csv -plot pie -x 0 -y 1 -first_line_header -plot_title "pie chart"

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -z 1 -plot_title "random xy"

CQChartsTest -csv airports.csv -plot delaunay -x 6 -y 5 -z 1 -plot_title "delaunay"

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -format "time:format=%Y%m%d" -bivariate -plot_title "bivariate"

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2 -plot_title "boxplot"

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d" -comment_header -plot_title "xy plot"

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -format "time:format=%Y%m%d" -comment_header -plot_title "multiple xy plot"

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7" -plot_title "parallel"

CQChartsTest -tsv scatter.tsv -plot scatter -x 0 -y 1 -z 4 -first_line_header -plot_title "scatter"

CQChartsTest -tsv stacked_area.tsv -plot xy -x 0 -y "1 2 3 4 5" -comment_header -format "time:format=%y-%b-%d" -stacked -plot_title "stacked area"

CQChartsTest -json flare.json -plot sunburst -x 0 -y 1 -plot_title "sunburst"

CQChartsTest -json flare.json -plot bubble -x 0 -y 1 -plot_title "bubble"

CQChartsTest -json flare.json -plot hierbubble -x 0 -y 1 -plot_title "hierarchical bubble"

CQChartsTest -tsv states.tsv -plot geometry -x 0 -y 1 -comment_header "geometry"

CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6" -first_line_header -and -csv ages.csv -plot pie -x 0 -y 1 -plot_title "bar chart and pie"

CQChartsTest -tsv states.tsv -plot geometry -x 0 -y 1 -comment_header -and -csv airports.csv -plot delaunay -x 6 -y 5 -z 1 -plot_title "states and airports"

CQChartsTest -tsv choropeth.tsv -plot geometry -x 0 -y 1 -z 2 -plot_title "choropeth"

CQChartsTest -tsv adjacency.tsv -plot adjacency -arg1 1 -arg2 3 -arg3 0 -arg4 2 -plot_title "adjacency"

CQChartsTest -csv xy_10000.csv -plot xy -x 0 -y 1 -plot_title "10000 points"

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -format "time:format=%Y%m%d" -comment_header -plot_title "multiple y axis"

CQChartsTest -y1y2 -tsv multi_series.tsv -plot xy -x 0 -y 1 -format "time:format=%Y%m%d,oformat=%F" -comment_header -and -tsv multi_series.tsv -plot xy -x 0 -y 2 -format "time:format=%Y%m%d,oformat=%F" -comment_header -plot_title "multiple y axis"

CQChartsTest -json flare.json -plot treemap -x 0 -y 1 -plot_title "tree map"

CQChartsTest -y1y2 -csv pareto.csv -plot bar -and -csv pareto.csv -plot xy -cumulative -xmin -0.5 -xmax 5.5 -ymin2 0 -xintegral -plot_title "pareto"
