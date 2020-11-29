proc init { } {
  #echo "init"

  set ::tick_count            0
  set ::inject_count          0
  set ::time_step             0.01
  set ::inject_ticks          20
  set ::inject_x_velocity_min -25
  set ::inject_x_velocity_max 25
  set ::inject_y_velocity_min 20
  set ::inject_y_velocity_max 100
  set ::explode_ticks         200
  set ::explosion_ticks       200
  set ::step_delay            50

  set ::gravity 9.8

  set ::numParticles 0

  set ::active 1
}

proc add_particle { } {
  #echo "add_particle"

  set r $::numParticles

  incr ::numParticles

  return $r
}

proc rand_in { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

proc explode_particle { row } {
  #echo "explode_particle $row"

  set x $::particle_x($row)
  set y $::particle_y($row)

  set oldNumParticles $::numParticles

  for {set i 0} {$i < 8} {incr i} {
    set row1 [add_particle]

    set ::particle_dead($row1) 0

    set ::particle_mass($row1) 1
    set ::particle_age($row1)  $::explode_ticks

    set ::particle_x($row1) $x
    set ::particle_y($row1) $y

    set ::particle_xv($row1) 0
    set ::particle_yv($row1) 0

    set r [rand_in 0.5 1.0]
    set g [rand_in 0.5 1.0]
    set b [rand_in 0.5 1.0]

    #echo "explode_particle $row1 $r $g $b"

    set ::particle_color_r($row1) $r
    set ::particle_color_g($row1) $g
    set ::particle_color_b($row1) $b
    set ::particle_color_a($row1) 1.0
  }

  set row1 $oldNumParticles

  set ::particle_xv($row1) [expr { 20 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr {  0 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr { 15 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr { 15 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr {  0 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr { 20 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr {-15 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr { 15 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr {-20 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr {  0 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr {-15 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr {-15 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr {  0 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr {-20 + [rand_in -2 2]}]

  incr row1

  set ::particle_xv($row1) [expr { 15 + [rand_in -2 2]}]
  set ::particle_yv($row1) [expr {-15 + [rand_in -2 2]}]
}

proc step { } {
  #echo "step"

  incr ::tick_count

  inject_particle

  update_particles

  update_model

  if {$::active} {
    qt_timer -delay $::step_delay -proc step
  }
}

proc inject_particle { } {
  if {$::inject_count == 0} {
    #echo "inject_particle"

    set row1 [add_particle]

    set ::particle_dead($row1) 0

    set ::particle_mass($row1) 1
    set ::particle_age($row1) 0

    set ::particle_x($row1) 0.5
    set ::particle_y($row1) 0

    set ::particle_xv($row1) [rand_in $::inject_x_velocity_min $::inject_x_velocity_max]
    set ::particle_yv($row1) [rand_in $::inject_y_velocity_min $::inject_y_velocity_max]

    #echo " V: $::particle_xv($row1) $::particle_yv($row1)"

    #set ::particle_color_r($row1) [rand_in 0.5 1.0]
    #set ::particle_color_g($row1) [rand_in 0.5 1.0]
    #set ::particle_color_b($row1) [rand_in 0.5 1.0]
    set ::particle_color_r($row1) [rand_in 0.9 1.0]
    set ::particle_color_g($row1) [rand_in 0.9 1.0]
    set ::particle_color_b($row1) [rand_in 0.9 1.0]

    set ::particle_color_a($row1) 1.0
  }

  incr ::inject_count

  if {$::inject_count > $::inject_ticks} {
    set ::inject_count 0
  }
}

proc update_particles { } {
  #echo "update_particles"

  set ax 0.0
  set ay -$::gravity

  set vx 0
  set vy [expr {$ay*$::time_step}]

  for {set r 0} {$r < $::numParticles} {incr r} {
    if {$::particle_dead($r)} {
      continue
    }

    set ::particle_x($r) [expr {$::particle_x($r) + ($::particle_xv($r) + 0.5*$vx)*$::time_step}]
    set ::particle_y($r) [expr {$::particle_y($r) + ($::particle_yv($r) + 0.5*$vy)*$::time_step}]

    set ::particle_yv($r) [expr {$::particle_yv($r) + $vy}]

    if {$::particle_y($r) <= 0.0} {
      set ::particle_y($r)  0
      set ::particle_xv($r) 0
      set ::particle_yv($r) 0

      set ::particle_dead($r) 1
    }
  }

  for {set r 0} {$r < $::numParticles} {incr r} {
    if {$::particle_dead($r)} {
      continue
    }

    set ::particle_age($r) [expr {$::particle_age($r) + 1}]

    if {$::particle_age($r) == $::explode_ticks} {
      set ::particle_dead($r) 1

      explode_particle $r
    }

    if {$::particle_age($r) > $::explode_ticks + $::explosion_ticks} {
      set ::particle_dead($r) 1
    }

    if {$::particle_age($r) > $::explode_ticks} {
     #set ::particle_color_r($r) [expr {$::particle_color_r($r) - 0.01}]
     #set ::particle_color_g($r) [expr {$::particle_color_g($r) - 0.01}]
     #set ::particle_color_b($r) [expr {$::particle_color_b($r) - 0.01}]
      set ::particle_color_a($r) [expr {$::particle_color_a($r) - 0.01}]

     #if {$::particle_color_r($r) < 0.0} { set ::particle_color_r($r) 0.0 }
     #if {$::particle_color_g($r) < 0.0} { set ::particle_color_g($r) 0.0 }
     #if {$::particle_color_b($r) < 0.0} { set ::particle_color_b($r) 0.0 }
      if {$::particle_color_a($r) < 0.0} { set ::particle_color_a($r) 0.0 }

      #echo "Update Color: [encode_color $r]"
    }
  }
}

proc encode_color { row } {
  set r [expr {int($::particle_color_r($row) * 255)}]
  set g [expr {int($::particle_color_g($row) * 255)}]
  set b [expr {int($::particle_color_b($row) * 255)}]
 #set a [expr {int($::particle_color_a($row) * 255)}]

 #return [format "#%02x%02x%02x%02x" $r $g $b $a]
  return [format "#%02x%02x%02x" $r $g $b]
}

proc update_model { } {
  set nr 0

  for {set row 0} {$row < $::numParticles} {incr row} {
    if {! $::particle_dead($row)} {
      incr nr
    }
  }

  set_charts_data -model $::model -name size -value [list $nr 8]

  set nr 0

  for {set row 0} {$row < $::numParticles} {incr row} {
    if {$::particle_dead($row)} {
      continue
    }

    set x $::particle_x($row)
    set y $::particle_y($row)

    set xv $::particle_xv($row)
    set yv $::particle_yv($row)

    set age $::particle_age($row)

    set color [encode_color $row]
    set alpha $::particle_color_a($row)

    set_charts_data -model $::model -row $nr -column 0 -name value -value $x
    set_charts_data -model $::model -row $nr -column 1 -name value -value $y
    set_charts_data -model $::model -row $nr -column 2 -name value -value $xv
    set_charts_data -model $::model -row $nr -column 3 -name value -value $yv
    set_charts_data -model $::model -row $nr -column 4 -name value -value $age
    set_charts_data -model $::model -row $nr -column 5 -name value -value $color
    set_charts_data -model $::model -row $nr -column 6 -name value -value $alpha
    set_charts_data -model $::model -row $nr -column 7 -name value -value 1

    incr nr
  }
}

proc keyPress { view key } {
  echo "keyPress $key"

  if {$key == "p"} {
    set ::active [expr {! $::active}]

    if {$::active} {
      qt_timer -delay $::step_delay -proc step
    }
  }
}

init

# one row per firework, x, y, color
set model [create_charts_data_model -rows $::numParticles -columns 8]

set_charts_data -model $::model -column 0 -header -name value -value X
set_charts_data -model $::model -column 1 -header -name value -value Y
set_charts_data -model $::model -column 2 -header -name value -value XV
set_charts_data -model $::model -column 3 -header -name value -value YV
set_charts_data -model $::model -column 4 -header -name value -value Age
set_charts_data -model $::model -column 5 -header -name value -value Color
set_charts_data -model $::model -column 6 -header -name value -value Alpha
set_charts_data -model $::model -column 7 -header -name value -value Visible

#set_charts_data -model $model -column 5 -name column_type -value {color {mapped 1}}
set_charts_data -model $model -column 5 -name column_type -value color
set_charts_data -model $model -column 6 -name column_type -value real

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1} {color 5} {alpha 6}} \
 -xmin -100 -ymin 0 -xmax 100 -ymax 200]

set_charts_property -plot $plot -name dataBox.fill.visible -value 0

set_charts_property -plot $plot -name plotBox.fill.visible -value 1
#set_charts_property -plot $plot -name plotBox.fill.color   -value black
set_charts_property -plot $plot -name plotBox.fill.pattern -value "IMAGE:data/night_moon.jpg"

#set_charts_property -plot $plot -name symbol.type           -value box
set_charts_property -plot $plot -name symbol.stroke.visible -value 0
#set_charts_property -plot $plot -name symbol.fill.pattern   -value "IMAGE:data/particle.png"

set_charts_property -plot $plot -name key.visible -value 0
set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set_charts_property -plot $plot -name symbol.type -value star6
set_charts_property -plot $plot -name symbol.size -value 12px

set view [get_charts_data -plot $plot -name view]

connect_charts_signal -view $view -from keyEventPress -to keyPress

qt_timer -delay $::step_delay -proc step
