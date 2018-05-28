# XY Plot

# Tests:
#   Simple    (Single Line)
#   Simple    (Multi Line)
#   Bivariate (Single Pair)
#   Bivariate (Multiple Pairs)
#   Stacked   (Single Line)
#   Stacked   (Multiple Lines)

CQChartsTest \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "simple xy - single line" \
-and \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3" -column_type "time:format=%Y%m%d" \
 -plot_title "simple xy - multi line" \
-and \
 -tsv data/bivariate.tsv -comment_header \
 -type xy -columns "x=0,y=1 2" -column_type "time:format=%Y%m%d,oformat=%F" \
 -bivariate \
 -plot_title "bivariate - single line" \
 -properties "yaxis.includeZero=1" \
-and \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3" -column_type "time:format=%Y%m%d,oformat=%F" \
 -bivariate \
 -plot_title "bivariate - multi line" \
-and \
 -tsv data/stacked_area.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%y-%b-%d" -stacked \
 -plot_title "stacked xy - single line" \
-and \
 -tsv data/stacked_area.tsv -comment_header \
 -type xy -columns "x=0,y=1 2 3 4 5" -column_type "time:format=%y-%b-%d" -stacked \
 -plot_title "stacked xy - multi line" \
-close_app
