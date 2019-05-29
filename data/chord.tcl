set model [load_charts_model -data data/chord-cities.data]

set plot [create_charts_plot -type chord -columns {{link 0} {group 1}}]

#set_charts_property -plot chord1 -name visible -value 0
