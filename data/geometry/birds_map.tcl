set model1 [load_charts_model -csv data/birds.csv -first_line_header]

set_charts_data -model $model1 -column {OBSERVATION DATE} -name column_type \
  -value {time {format {%d.%m.%Y %H:%M:%S}}}

set model2 [load_charts_model -csv data/world.csv -comment_header \
  -column_type {{{1 polygon_list}}}]

#---

set view [create_charts_view]

set plot1 [create_charts_plot -model $model2 -type geometry \
  -columns {{name location} {geometry geometry}}]
set plot2 [create_charts_plot -model $model1 -type scatter \
  -columns {{x LONGITUDE} {y LATITUDE} {controls {{SUBNATIONAL2_CODE} {LOCALITY TYPE} {LATITUDE} {LONGITUDE} {OBSERVATION DATE} {Month} {COUNTY} {LOCALITY} {Family}}}}]

set plot3 [group_charts_plots -composite [list $plot1 $plot2]]

set_charts_property -plot $plot3 -name commonXRange -value 0
set_charts_property -plot $plot3 -name commonYRange -value 0

set_charts_property -plot $plot3 -name currentPlot -value 1
