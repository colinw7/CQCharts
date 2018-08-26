# Force Directed Plot

set model1 [load_model -tsv data/adjacency.tsv]

set plot1 [create_plot -model $model1 -type forcedirected -columns "node=1,connections=3,name=0,group=2" -title "adjacency"]

#set model2 [load_model -csv data/adjacency.csv]

#set plot2 [create_plot -model $model2 -type forcedirected -columns "name=0,value=1,group=2" -title adjacency]
