# Name,Party,Province,Age,Gender
set model [load_charts_model -csv data/mps.csv -first_line_header]

#set plot [create_charts_plot -type pivot -columns {{x 4} {y 2} {value 3}} -title "Pivot Plot"]
#set plot [create_charts_plot -type pivot -columns {{x 4} {value 3}} -title "Pivot Plot"]
set plot [create_charts_plot -type pivot -columns {{x 2} {y 1} {value 3}} -title "Pivot Plot"]
