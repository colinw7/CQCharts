CQChartsTest \
 -tsv digits.tab -first_line_header \
 -type boxplot -columns "x=0,y=2,group=1" \
 -properties "skipOutliers=1,connected=1" \
 -plot_title "boxplot"
