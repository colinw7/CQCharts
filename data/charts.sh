CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6"

CQChartsTest -csv ages.csv -plot pie -x 0 -y 1

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -z 1

CQChartsTest -csv airports.csv -plot delaunay -x 6 -y 5 -z 1

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -format "time:format=%Y%m%d" -bivariate

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -format "time:format=%Y%m%d" -comment_header

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7"

CQChartsTest -tsv scatter.tsv -plot scatter -x 0 -y 1 -z 4 -first_line_header

CQChartsTest -tsv stacked_area.tsv -plot xy -x 0 -y "1 2 3 4 5" -comment_header -format "time:format=%y-%b-%d" -stacked

CQChartsTest -json flare.json -plot sunburst -x 0 -y 1

CQChartsTest -tsv states.tsv -plot geometry -x 0 -y 1 -comment_header

CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6" -and -csv ages.csv -plot pie -x 0 -y 1

CQChartsTest -tsv states.tsv -plot geometry -x 0 -y 1 -comment_header -and -csv airports.csv -plot delaunay -x 6 -y 5 -z 1

CQChartsTest -tsv choropeth.tsv -plot geometry -x 0 -y 1 -z 2

CQChartsTest -tsv adjacency.tsv -plot adjacency -arg1 1 -arg2 3 -arg3 0 -arg4 2
