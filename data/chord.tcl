set model [load_model -data data/chord-cities.data]

set plot [create_plot -type chord -columns "name=0,group=1"]

#set_property -plot chord1 -name visible -value 0
