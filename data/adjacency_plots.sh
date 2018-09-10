# Adjacency

CQChartsTest \
 -tsv data/adjacency.tsv -comment_header \
 -type adjacency -columns "node=1,connections=3,name=0,groupId=2" \
 -plot_title "adjacency"

CQChartsTest \
 -csv data/adjacency.csv -comment_header \
 -type adjacency -columns "namePair=0,count=1,groupId=2" \
 -plot_title "adjacency"

CQChartsTest -exec adjacency_plots.tcl -loop
