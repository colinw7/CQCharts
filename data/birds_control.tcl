set model [load_charts_model -csv data/birds.csv -first_line_header]

set_charts_data -model $model -column {OBSERVATION DATE} -name column_type \
  -value {time {format {%d.%m.%Y %H:%M:%S}}}

#---

# columns
#  Num SCIENTIFIC_NAME SUBNATIONAL2_CODE LOCALITY ID LOCALITY_TYPE OBSERVATION_COUNT LATITUDE LONGITUDE OBSERVATION_DATE Month COUNTY LOCALITY Family COMMON_NAME

set view [create_charts_view]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x LONGITUDE} {y LATITUDE} {controls {{SUBNATIONAL2_CODE} {LOCALITY TYPE} {LATITUDE} {LONGITUDE} {OBSERVATION DATE} {Month} {COUNTY} {LOCALITY} {Family}}}}]
