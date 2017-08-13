CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6"

CQChartsTest -csv ages.csv -plot pie -x 0 -y 1

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -z 1

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -format "time:format=%Y%m%d" -bivariate

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -format "time:format=%Y%m%d" -comment_header

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7"

CQChartsTest -tsv scatter.tsv -plot scatter -x 0 -y 1 -z 4 -first_line_header

CQChartsTest -tsv stacked_area.tsv -plot xy -x 0 -y "1 2 3 4 5" -comment_header -format "time:format=%y-%b-%d" -stacked
