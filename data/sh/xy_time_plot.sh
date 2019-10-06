CQChartsTest \
 -tsv data/multi_series.tsv -comment_header \
 -type xy -columns "x=0,y=1" -column_type "time:format=%Y%m%d,oformat=%F" \
 -plot_title "simple xy - single line" -loop
