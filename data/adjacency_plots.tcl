# Adjacency Plot

set model1 [load_model -tsv data/adjacency.tsv -comment_header]

set plot1 [create_plot -model $model1 -type adjacency -columns "id=1,connections=3,name=0,group=2" -title "adjacency"]

#set model2 [load_model -csv data/adjacency.csv -comment_header]

#set plot2 [create_plot -model $model2 -type adjacency -columns "namePair=0,count=1,group=2" -title "adjacency"]

#place_plots -horizontal $plot1 $plot2
