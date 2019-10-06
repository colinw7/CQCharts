CQChartsTest \
 -tsv data/digits_1.tsv -first_line_header \
 -type boxplot -columns "set=0,group=1,value=2" \
 -properties "outlier.visible=0,options.connected=0" \
 -plot_title "boxplot"

CQChartsTest \
 -tsv data/digits_1.tsv -first_line_header \
 -type boxplot -columns "set=0,group=1,value=2" \
 -properties "outlier.visible=0,options.connected=1" \
 -plot_title "boxplot"
