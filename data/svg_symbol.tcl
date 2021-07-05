create_charts_symbol_set -name svg

add_charts_symbol -set svg -svg data/weather_clear.svg -name weather_clear -filled 1
add_charts_symbol -set svg -svg data/weather_cloudy.svg -name weather_cloudy -filled 1
add_charts_symbol -set svg -svg data/weather_overcast.svg -name weather_overcast -filled 1
add_charts_symbol -set svg -svg data/weather_rain.svg -name weather_rain -filled 1
add_charts_symbol -set svg -svg data/weather_snow.svg -name weather_snow -filled 1

set plot [create_charts_plot -type empty -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set symbols [get_charts_data -name symbols -data svg]

set n [llength $symbols]

set ny [expr {int(sqrt($n))}]
set nx [expr {int(($n + $ny - 1)/$ny)}]

echo $ny
echo $nx

set ix 0
set iy 0

set dx [expr {100/$nx}]
set dy [expr {100/$ny}]

set ss [expr {$dx/2.0}]

foreach symbol $symbols {
echo $symbol
  set x [expr {($ix + 0.5)*$dx}]
  set y [expr {($iy + 0.5)*$dy}]

# set ann [create_charts_point_annotation -plot $plot -id $symbol \
#            -position [list $x $y] -symbol $symbol -size [list $ss P] \
#            -filled 1 -fill_color green]
  set ann [create_charts_point_annotation -plot $plot -id $symbol \
             -position [list $x $y] -symbol $symbol -size [list $ss P] \
             -fill_color green]

  incr ix

  if {$ix >= $nx} {
    incr iy

    set ix 0
  }
}
