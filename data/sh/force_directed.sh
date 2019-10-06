# Force Directed Plot

CQChartsTest \
 -tsv data/adjacency.tsv \
 -type forcedirected -columns "node=1,connections=3,name=0,groupId=2"
CQChartsTest \
 -csv data/adjacency.csv \
 -type forcedirected -columns "namePair=0,count=1,groupId=2" \
 -plot_title "force directed"
