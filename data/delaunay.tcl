set model [load_model -csv data/airports.csv -comment_header]

set plot [create_plot -type delaunay -columns "x=6,y=5,name=1" -title delaunay]
