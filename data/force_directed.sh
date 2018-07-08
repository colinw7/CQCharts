# Force Directed Plot

CQChartsTest \
 -tsv data/adjacency.tsv \
 -type forcedirected -columns "node=1,connections=3,name=0,group=2"
CQChartsTest \
 -csv data/adjacency.csv \
 -type forcedirected -columns "name=0,value=1,group=2" \
 -plot_title "adjacency"
