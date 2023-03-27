proc createWidgets { } {
  global colorLabel
  global scoreLabel

  set ::scoreText ""

  #----

  set rightFrame  [qt_create_widget -type QFrame -name right]
  set rightLayout [qt_create_layout -type QVBoxLayout -parent $rightFrame]

  set colorLabel [qt_create_widget -type QLabel -name color]
  qt_set_property -object $colorLabel -property text -value "White to Move"

  # TODO
  #  + White/Black Players
  #  + Level Control

  set scoreLabel [qt_create_widget -type QLabel -name score]
  qt_set_property -object $scoreLabel -property text -value "White 2 Black 2"

  set newGameButton [qt_create_widget -type QPushButton -name newGame]
  qt_set_property -object $newGameButton -property text -value "New Game"

  qt_connect_widget -name $newGameButton -signal "clicked()" -proc newGame

  set colorLabel [qt_add_child_widget -parent $rightFrame -child $colorLabel]
  set scoreLabel [qt_add_child_widget -parent $rightFrame -child $scoreLabel]

  qt_add_stretch -parent $rightFrame

  set newGameButton [qt_add_child_widget -parent $rightFrame -child $newGameButton]

  add_custom_widget -plot $::plot -widget $rightFrame
}

proc newGame { } {
  global moveColor

  initBoard

  setMoveColor "White"

  setScore
}

proc setWhitePlayer { player } {
  set ::whitePlayer $player
}

proc setBlackPlayer { player } {
  set ::blackPlayer $player
}

proc getCurrentPlayer { } {
  global moveColor whitePlayer blackPlayer

  if       {$moveColor == "White"} {
    return $whitePlayer
  } elseif {$moveColor == "Black"} {
    return $blackPlayer
  } else {
    return "None"
  }
}

proc setComputerLevel { level } {
  set ::computerLevel $level
}

proc setMoveColor { color } {
  #echo "setMoveColor $color"

  global colorLabel

  set parent [get_charts_data -view $::view -name custom_widgets_parent]

  set colorLabel1 "$parent|$colorLabel"

  set ::moveColor $color

  set ::moveColorText "$color to Move"

  qt_set_property -object $colorLabel1 -property text -value $::moveColorText
}

proc setCurrentPlayer { color } {
  global currentPlayer

  set currentPlayer $color
}

proc setScore { } {
  global scoreText
  global scoreLabel

  set parent [get_charts_data -view $::view -name custom_widgets_parent]

  set scoreLabel1 "$parent|$scoreLabel"

  set numWhite [getNumWhite]
  set numBlack [getNumBlack]

  set scoreText "White $numWhite Black $numBlack"

  qt_set_property -object $scoreLabel1 -property text -value $scoreText
}

proc canMove { x y color } {
  set rc [modelCanMove $::valueModel $x $y $color]
  #echo "canMove $x $y $color $rc"
  return $rc
}

proc modelCanMove { model x y color } {
  if {! [isModelNoPiece $model $x $y]} {
    return 0
  }

  set color1 [otherColor $color]

  if {[modelCanMoveDirection $model $x $y  1  0 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y  1  1 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y  0  1 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y -1  1 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y -1  0 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y -1 -1 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y  0 -1 $color $color1]} { return 1 }
  if {[modelCanMoveDirection $model $x $y  1 -1 $color $color1]} { return 1 }

  return 0
}

proc canMoveDirection { x y dx dy color otherColor } {
  #echo "canMoveDirection $x $y $dx $dy $color $otherColor"

  return [modelCanMoveDirection $::valueModel $x $y $dx $dy $color $otherColor]
}

proc modelCanMoveDirection { model x y dx dy color otherColor } {
  set x1 [expr {$x + $dx}]
  set y1 [expr {$y + $dy}]

  set value [getModelPieceColor $model $x1 $y1]

  if {$value != $otherColor} {
    return 0
  }

  set x1 [expr {$x1 + $dx}]
  set y1 [expr {$y1 + $dy}]

  set value [getModelPieceColor $model $x1 $y1]

  while {$value == $otherColor} {
    set x1 [expr {$x1 + $dx}]
    set y1 [expr {$y1 + $dy}]

    set value [getModelPieceColor $model $x1 $y1]
  }

  set value [getModelPieceColor $model $x1 $y1]

  if {$value != $color} {
    return 0
  }

  return 1
}

proc doMove { x y color } {
  #echo "doMove $x $y $color"

  modelDoMove $::valueModel $x $y $color
}

proc modelDoMove { model x y color } {
  set color1 [otherColor $color]

  if {[modelCanMoveDirection $model $x $y  1  0 $color $color1]} {
    modelDoMoveDirection $model $x $y  1  0 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y  1  1 $color $color1]} {
    modelDoMoveDirection $model $x $y  1  1 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y  0  1 $color $color1]} {
    modelDoMoveDirection $model $x $y  0  1 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y -1  1 $color $color1]} {
    modelDoMoveDirection $model $x $y -1  1 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y -1  0 $color $color1]} {
    modelDoMoveDirection $model $x $y -1  0 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y -1 -1 $color $color1]} {
    modelDoMoveDirection $model $x $y -1 -1 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y  0 -1 $color $color1]} {
    modelDoMoveDirection $model $x $y  0 -1 $color $color1
  }
  if {[modelCanMoveDirection $model $x $y  1 -1 $color $color1]} {
    modelDoMoveDirection $model $x $y  1 -1 $color $color1
  }

  setModelPieceColor $model $x $y $color
}

proc doMoveDirection { x y dx dy color otherColor } {
  #echo "doMoveDirection $x $y $dx $dy $color $otherColor"

  return [modelDoMoveDirection $::valueModel $x $y $dx $dy $color $otherColor]
}

proc modelDoMoveDirection { model x y dx dy color otherColor } {
  set x1 [expr {$x + $dx}]
  set y1 [expr {$y + $dy}]

  set value [getModelPieceColor $model $x1 $y1]

  if {$value != $otherColor} {
    return
  }

  setModelPieceColor $model $x1 $y1 $color

  set x1 [expr {$x1 + $dx}]
  set y1 [expr {$y1 + $dy}]

  set value [getModelPieceColor $model $x1 $y1]

  while {$value == $otherColor} {
    setModelPieceColor $model $x1 $y1 $color

    set x1 [expr {$x1 + $dx}]
    set y1 [expr {$y1 + $dy}]

    set value [getModelPieceColor $model $x1 $y1]
  }
}

proc getBestMove { value maxDepth } {
  return [modelGetBestMove $::valueModel $value $maxDepth]
}

proc modelGetBestMove { model value maxDepth } {
  return [modelGetBestMove1 $model $value $maxDepth 0]
}

proc modelGetBestMove1 { model value maxDepth depth } {
  #echo "modelGetBestMove1 $model $value $maxDepth $depth"

  if {$maxDepth <= 0} {
    set score 0

    return {}
  }

  set color [valueColor $value]

  set move {}

  set score -9999

  set board1 [getDepthBoard $model $depth]

  set board_score [modelBoardScore $model $value]

  for {set x1 0} {$x1 < 8} {incr x1} {
    for {set y1 0} {$y1 < 8} {incr y1} {
      set rc [modelCanMove $board1 $x1 $y1 $color]
      #echo "modelCanMove $board1 $x1 $y1 $color $rc"

      if {! $rc} {
        continue
      }

      modelDoMove $board1 $x1 $y1 $value

      set score1 [expr {[modelBoardScore $board1 $value] - $board_score}]

      set value1    [otherValue $value]
      set maxDepth1 [expr {$maxDepth - 1}]
      set depth1    [expr {$maxDepth + 1}]

      set move2 [modelGetBestMove1 $board1 $value1 $maxDepth1 $depth1]
      #echo "modelGetBestMove1 $board1 $value1 $maxDepth1 $depth1 $move2"

      if {[llength $move2]} {
        #echo "move2: $move2"

        set score2 [lindex $move2 2]

        set score1 [expr {$score1 - $score2}]
      }

      if {! [llength $move] || $score1 > $score || ($score1 == $score && [randBool])} {
        set move [list $x1 $y1 $score1]

        set score $score1
      }

      set board1 [getDepthBoard $model $depth]
    }
  }

  return $move
}

proc randBool { } {
  set r [expr {rand()}]

  return [expr {$r < 0.5}]
}

proc copyModel { fromModel toModel } {
  for {set y 0} {$y < 8} {incr y} {
    for {set x 0} {$x < 8} {incr x} {
      set value [get_charts_data -model $fromModel -row $y -column $x -name value]

      set_charts_data -model $toModel -row $y -column $x -name value -value $value
    }
  }
}

proc createTempModel { } {
  set model [create_charts_data_model -rows 8 -columns 8]

  return $model
}

proc getDepthBoard { model depth } {
  #echo "getDepthBoard $model $depth"

  global depthModels

  set numDepthModels [llength $depthModels]

  set depth1 [expr {$depth + 1}]

  while {$depth1 > $numDepthModels } {
    set model1 [createTempModel]

    lappend depthModels $model1

    incr numDepthModels
  }

  set model1 [lindex $depthModels $depth]

  copyModel $model $model1

  return $model1
}

proc computerMove { } {
  global moveColor computerLevel

  set player [getCurrentPlayer]

  while {$player == "Computer"} {
    after 250

    set moveValue [colorValue $moveColor]

    set bestMove [getBestMove $moveValue $computerLevel]

    if {! [llength $bestMove]} {
      #echo "No computer Move"
      break
    }

    #echo "computerBestMove $bestMove"

    global best_x
    global best_y

    set best_x [lindex $bestMove 0]
    set best_y [lindex $bestMove 1]

    doMove $best_x $best_y $moveColor

    setScore

    nextMoveColor

    set player [getCurrentPlayer]
  }
}

proc isNoPiece { x y } {
  return [isModelNoPiece $::valueModel $x $y]
}

proc isModelNoPiece { model x y } {
  set value [getModelPiece $model $x $y]

  if {$value == -1} {
    return 1
  } else {
    return 0
  }
}

proc isWhitePiece { x y } {
  return [isModeWhitePiece $::valueModel $x $y]
}

proc isModelWhitePiece { model x y } {
  set value [getModelPiece $model $x $y]

  if {$value == 1} {
    return 1
  } else {
    return 0
  }
}

proc isBlackPiece { x y } {
  return [isModelBlackPiece $::valueModel $x $y]
}

proc isModelBlackPiece { model x y } {
  set value [getModelPiece $model $x $y]
  
  if {$value == 0} {
    return 1 
  } else {
    return 0
  }
}

proc getPiece { x y } {
  return [getModelPiece $::valueModel $x $y]
}

proc getModelPiece { model x y } {
  if {$x < 0 || $x >= 8 || $y < 0 || $y >= 8} {
    return -1
  }

  return [get_charts_data -model $model -row $y -column $x -name value]
}

proc getPieceColor { x y } {
  return getModelPieceColor $::valueModel $x $y]
}

proc getModelPieceColor { model x y } {
  set value [getModelPiece $model $x $y]

  return [valueColor $value]
}

proc setPiece { x y value } {
  return [setModelPiece $::valueModel $x $y $value]
}

proc setModelPiece { model x y value } {
  if {$x < 0 || $x >= 8 || $y < 0 || $y >= 8} {
    return -1
  }

  set_charts_data -model $model -row $y -column $x -name value -value $value

  updateImageModel
}

proc setPieceColor { x y color } {
  setModelPieceColor $::valueModel $x $y $color
}

proc setModelPieceColor { model x y color } {
  #echo "setModelPieceColor $model $x $y $color"

  set value [colorValue $color]

  setModelPiece $model $x $y $value
}

proc getNumWhite { } {
  return [getModelNumWhite $::valueModel]
}

proc getModelNumWhite { model } {
  return [getModelNumOfValue $model 1]
}

proc getNumBlack { } {
  return [getModelNumBlack $::valueModel]
}

proc getModelNumBlack { model } {
  return [getModelNumOfValue $model 0]
}

proc getNum { } {
  return [getModelNum $::valueModel]
}

proc getModelNum { model } {
  return [expr {[getModelNumWhite $model] + [getModelNumBlack $model]}]
}

proc getNumOfValue { value } {
  return [getModelNumOfValue $::valueModel $value]
}

proc getModelNumOfValue { model value } { 
  set n 0

  for {set y 0} {$y < 8} {incr y} {
    for {set x 0} {$x < 8} {incr x} {
      set value1 [getModelPiece $model $x $y]

      if {$value1 == $value} {
        incr n
      }
    }
  }

  return $n
}

proc getMovesTaken { } {
  return [getModelMovesTaken $::valueModel]
}

proc getModelMovesTaken { model } {
  return [expr {64 - [getModelNumOfValue $model -1]}]
}

proc canMoveAnywhere { color } {
  return [modelCanMoveAnywhere $::valueModel $color]
}

proc modelCanMoveAnywhere { model color } {
  for {set x 0} {$x < 8} {incr x} {
    for {set y 0} {$y < 8} {incr y} {
      set rc [modelCanMove $model $x $y $color]
      #echo "modelCanMove $model $x $y $color $rc"

      if {$rc} {
        return 1
      }
    }
  }

  return 0
}

proc nextMoveColor { } {
  global moveColor

  setMoveColor [otherColor $moveColor]

  if {! [canMoveAnywhere $moveColor]} {
    setMoveColor [otherColor $moveColor]

    if {! [canMoveAnywhere $moveColor]} {
      gameOver
    }
  }
}

proc boardScore { value } {
  return [modelBoardScore $::valueModel $value]
}

proc modelBoardScore { model value } {
  #echo "modelBoardScore $model $value"

  set other_value [otherValue $value]

  set score 0

  for {set x 0} {$x < 8} {incr x} {
    for {set y 0} {$y < 8} {incr y} {
      set value1 [get_charts_data -model $model        -row $y -column $x -name value]
      set score  [get_charts_data -model $::scoreModel -row $y -column $x -name value]

      if       {$value1 == $value} {
        set score [expr {$score + $value}]
      } elseif {$value1 == $other_value} {
        set score [expr {$score - $value}]
      }
    }
  }

  return $score
}

proc gameOver { } {
  global scoreText

  set numWhite [getNumWhite]
  set numBlack [getNumBlack]

  if       {$numWhite > $numBlack } {
    set scoreText "White Wins"
  } elseif {$numBlack > $numWhite } {
    set scoreText "Black Wins"
  } else {
    set scoreText "Draw"
  }
}

proc otherPlayer { } {
  global currentPlayer

  set currentPlayer [otherColor $currentPlayer]
}

proc otherColor { color } {
  if {$color == "White"} {
    return "Black"
  } else {
    return "White"
  }
}

proc otherPlayer { player } {
  if {$player == "Human"} {
    return "Computer"
  } else {
    return "Human"
  }
}

proc otherValue { value } {
  if {$value == 0} { return 1 }
  if {$value == 1} { return 0 }

  return -1
}

proc colorValue { color } {
  if {$color == "Black"} { return 0 }
  if {$color == "White"} { return 1 }

  return -1
}

proc valueColor { value } {
  if {$value == 0} { return "Black" }
  if {$value == 1} { return "White" }

  return "None"
}

#---

proc current_player_cb { } {
  set num [getNum]

  if {$num == 4} {
    global moveColor

    setMoveColor [otherColor $moveColor]

    computerMove
  }
}

proc white_player_cb { } {
  global whitePlayer

  set whitePlayer [otherPlayer $whitePlayer]

  drawPlayers

  computerMove
}

proc black_player_cb { } {
  global blackPlayer

  set blackPlayer [otherPlayer $blackPlayer]

  drawPlayers

  computerMove
}

proc level_down_cb { } {
  global computerLevel

  if {$computerLevel > 1} {
    incr computerLevel -1
  }

  drawLevel
}

proc level_up_cb { } {
  global computerLevel

  if {$computerLevel < 10} {
    incr computerLevel
  }

  drawLevel
}

proc new_game_button_cb { } {
  newGame
}

proc quit_cb { } {
  exit
}

#----

proc createModels { } {
  global scoreModel

  set scoreModel [create_charts_data_model -rows 8 -columns 8]

  set scoreValues {
    { 128   1  64   8   8  64   1 128 }
    {   1   0   4   2   2   4   0   1 }
    {  64   4  32  16  16  32   4  64 }
    {   8   2  16   0   0  16   2   8 }
    {   8   2  16   0   0  16   2   8 }
    {  64   4  32  16  16  32   4  64 }
    {   1   0   4   2   2   4   0   1 }
    { 128   1  64   8   8  64   1 128 }
  }

  for {set y 0} {$y < 8} {incr y} {
    for {set x 0} {$x < 8} {incr x} {
      set value [lindex [lindex $scoreValues $y] $x]

      set_charts_data -model $scoreModel -row $y -column $x -name value -value $value
    }
  }

  #---

  global valueModel

  set valueModel [create_charts_data_model -rows 8 -columns 8]

  #---

  global imageModel

  set imageModel [create_charts_data_model -rows 8 -columns 8]

  for {set c 0} {$c < 8} {incr c} {
    set_charts_data -model $imageModel -column $c -name column_type -value {{image}}
  }

  #---

  initBoard

  #---

  global depthModels

  set depthModels {}
}

proc initBoard { } {
  #echo "initBoard"

  for {set y 0} {$y < 8} {incr y} {
    for {set x 0} {$x < 8} {incr x} {
      set_charts_data -model $::valueModel -row $y -column $x -name value -value -1
    }
  }

  set_charts_data -model $::valueModel -row 3 -column 3 -name value -value 1
  set_charts_data -model $::valueModel -row 3 -column 4 -name value -value 0
  set_charts_data -model $::valueModel -row 4 -column 3 -name value -value 0
  set_charts_data -model $::valueModel -row 4 -column 4 -name value -value 1

  updateImageModel
}

proc updateImageModel { } {
  #echo "updateImageModel"

  set white_image pics/white_piece.svg
  set black_image pics/black_piece.svg
  set no_image ""

  for {set y 0} {$y < 8} {incr y} {
    for {set x 0} {$x < 8} {incr x} {
      set value [get_charts_data -model $::valueModel -row $y -column $x -name value]

      if       {$value == 0} {
        set image $black_image
      } elseif {$value == 1} {
        set image $white_image
      } else {
        set image $no_image
      }

      set_charts_data -model $::imageModel -row $y -column $x -name value -value $image
    }
  }
}

#---

proc objPressed { view plot id } {
  set player [getCurrentPlayer]

  if {$player == "Computer"} {
    return
  }

  global moveColor

  set parts [split $id ":"]

  set y [lindex $parts 1]
  set x [lindex $parts 2]

  #echo "Press $x $y"

  if {[canMove $x $y $moveColor]} {
    doMove $x $y $moveColor

    setScore

    nextMoveColor

    computerMove
  }
}

createModels

set plot [create_charts_plot -model $imageModel -type image]
#set plot [create_charts_plot -model $valueModel -type image]
#set plot [create_charts_plot -model $scoreModel -type image]

set_charts_property -plot $plot -name margins.outer.left   -value 0px
set_charts_property -plot $plot -name margins.outer.right  -value 196px
set_charts_property -plot $plot -name margins.outer.bottom -value 0px
set_charts_property -plot $plot -name margins.outer.top    -value 0px

set_charts_property -plot $plot -name cell.fill.visible   -value 0
set_charts_property -plot $plot -name cell.stroke.visible -value 1
set_charts_property -plot $plot -name cell.stroke.alpha   -value 0.3

set_charts_property -plot $plot -name cell.style -value BALLOON

connect_charts_signal -plot $plot -from objIdPressed -to objPressed

set view [get_charts_data -plot $::plot -name view]
#set view_path [get_charts_data -plot $::plot -name view_path]

set_charts_property -view $view -name options.settingsTabs -value "CONTROLS|WIDGETS|PROPERTIES"

#---

createWidgets

#---

setWhitePlayer "Human"
setBlackPlayer "Computer"

setComputerLevel 3

setMoveColor "White"

newGame
