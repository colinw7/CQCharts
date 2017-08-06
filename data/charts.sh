CQChartsTest -csv multi_bar.csv -plot bar -x 0 -y "1 2 3 4 5 6"

CQChartsTest -csv ages.csv -plot pie -x 0 -y 1

CQChartsTest -csv airports.csv -plot xy -x 6 -y 5 -z 1

CQChartsTest -data bivariate.tsv -plot xy -x 0 -y "1 2" -format "time:format=%Y%m%d"

CQChartsTest -csv boxplot.csv -plot box -x 0 -y 2

CQChartsTest -tsv multi_series.tsv -plot xy -x 0 -y "1 2 3" -format "time:format=%Y%m%d"

CQChartsTest -csv parallel_coords.csv -plot parallel -x 0 -y "1 2 3 4 5 6 7"
