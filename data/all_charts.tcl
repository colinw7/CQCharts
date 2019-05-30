# Adjacency Plot

set model1 [load_charts_model -tsv data/adjacency.tsv -comment_header]

set plot1 [create_charts_plot -model $model1 -type adjacency \
  -columns {{name 1} {connections 3} {name 0} {groupId 2}} -title "adjacency"]

# Bar Chart

set model2 [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot2 [create_charts_plot -model $model2 -type barchart \
  -columns {{value sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}}]

# Box Plot

set model3 [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot3 [create_charts_plot -model $model3 -type boxplot -columns {{value {0 1 2 3}}}]

# Bubble/Hier Bubble Plot

set model4 [load_charts_model -csv data/hier.csv -comment_header]

set plot4 [create_charts_plot -model $model4 -type bubble \
  -columns {{group 0} {value 1}} -title "Bubble Group"]

# Chord Plot

set model5 [load_charts_model -data data/chord-cities.data]

set plot5 [create_charts_plot -model $model5 -type chord -columns {{link 0} {group 1}}]

# Delaunay Plot

set model6 [load_charts_model -csv data/airports.csv -comment_header]

set plot6 [create_charts_plot -model $model6 -type delaunay \
  -columns {{x 6} {y 5} {name 1}} -title delaunay]

# Dendrogram

set model7 [load_charts_model -csv data/flare.csv -comment_header]

set plot7 [create_charts_plot -model $model7 -type dendrogram \
  -columns {{name 0} {value 1}} -title "dendrogram"]

# Distribution Plot

set model8 [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot8 [create_charts_plot -model $model8 -type distribution \
  -columns {{value sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Sepal Length}}}]

# Force Directed Plot

set model9 [load_charts_model -tsv data/adjacency.tsv]

set plot9 [create_charts_plot -model $model9 -type forcedirected \
  -columns {{node 1} {connections 3} {name 0} {groupId 2}} -title "Force Directed"]

# Geometry Plot

set model10 [load_charts_model -tsv data/states.tsv -comment_header]

set plot10 [create_charts_plot -model $model10 -type geometry \
  -columns {{name 0} {geometry 1}} -title "geometry"]

# Image Plot

set model11 [load_charts_model -csv data/winequality-white.csv -first_line_header -separator {;}]

set corrModel11 [create_charts_correlation_model -model $model11]

set plot11 [create_charts_plot -model $corrModel11 -type image]

# Parallel Plot

set model12 [load_charts_model -csv data/parallel_coords.csv -first_line_header]

set plot12 [create_charts_plot -model $model12 -type parallel \
  -columns {{x 0} {y {1 2 3 4 5 6 7}}} -title "parallel"]

# Pie Plot

set model13 [load_charts_model -csv data/ages.csv -first_line_header -column_type {{{1 integer}}}]

set plot13 [create_charts_plot -model $model13 -type pie \
  -columns {{label 0} {value 1}} -title "pie chart"]

# Radar Plot

set model14 [load_charts_model -csv data/radar.csv -first_line_header]

set plot14 [create_charts_plot -model $model14 -type radar \
  -columns {{name 0} {value {1 2 3 4 5}}} -title "Radar Plot"]

# Sankey

set model15 [load_charts_model -csv data/sankey.csv -comment_header]

set plot15 [create_charts_plot -model $model15 -type sankey -columns {{link 0} {value 1}}]

# Scatter

set model16 [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot16 [create_charts_plot -model $model16 -type scatter \
  -columns {{x petalLength} {y sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}}]

# Hier Scatter Plot

set model17 [load_charts_model -csv data/airports.csv -comment_header]

set plot17 [create_charts_plot -model $model17 -type hierscatter \
  -columns {{x 6} {y 5} {name 1} {group {4 3 2}}} -title "airports"]

# Sunburst Plot

set model18 [load_charts_model -json data/flare.json]

set plot18 [create_charts_plot -model $model18 -type sunburst \
  -columns {{names 0} {value 1}} -title "sunburst"]

# TreeMap Plot

set model19 [load_charts_model -json data/flare.json]

set plot19 [create_charts_plot -model $model19 -type treemap \
  -columns {{names 0} {value 1}} -title "tree map"]

# XY Plot

set model20 [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot20 [create_charts_plot -model $model20 -type xy -columns {{x 0} {y 1}} -title "XY Plot"]

###---

set view [get_charts_property -plot $plot1 -name viewId]

set plots [get_charts_data -view $view -name plots]

place_charts_plots -view $view -rows 4 -columns 5 $plots
