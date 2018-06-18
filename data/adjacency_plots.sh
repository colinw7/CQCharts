# Adjacency

CQChartsTest \
 -tsv data/adjacency.tsv -comment_header \
 -type adjacency -columns "id=1,connections=3,name=0,group=2" \
 -plot_title "adjacency"

CQChartsTest \
 -csv data/adjacency.csv -comment_header \
 -type adjacency -columns "namePair=0,count=1,group=2" \
 -plot_title "adjacency"

CQChartsTest -exec adjacency_plots.tcl -loop
