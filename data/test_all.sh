#/bin/csh -f

set tests1 = ( \
annotations.sh \
arrow_annotation.sh \
arrow_style.sh \
color_bar.sh \
column_type.sh \
coverage.sh \
ellipse_annotation.sh \
expr_column.sh \
jitter.sh \
path_geom.sh \
poly_geom.sh \
population.sh \
rect_geom.sh \
split_bar_separated.sh \
text_annotation.sh \
tiger_svg.sh \
)

foreach test ($tests1)
  echo "--- $test ---"

  source $test
end

set tests2 = ( \
barchart.sh \
bar_headers.sh \
distribution.sh \
geometry_plots.sh \
piechart.sh \
xy_plots.sh \
simple.sh \
charts.sh \
big_data.sh \
)

foreach test ($tests2)
  echo "--- $test ---"

  source $test
end

set tests3 = ( \
digits.sh \
)

foreach test ($tests3)
  echo "--- $test ---"

  source $test
end

exit 0
