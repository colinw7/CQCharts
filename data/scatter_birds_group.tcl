set model [load_charts_model -csv data/birds.csv -first_line_header]

set_charts_data -model $model -column {OBSERVATION DATE} -name column_type \
  -value {time {format {%d.%m.%Y %H:%M:%S}}}

#---

set view [create_charts_view]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x LONGITUDE} {y LATITUDE} {group COUNTY}}]
