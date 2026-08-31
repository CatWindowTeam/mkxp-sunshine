# =======================  COLORS  =======================
#  i DO NOT know how these colors will look on other
#  controllers, but on PS4 DualShock 4 controller these
#  colors look somewhat close to the original colors.
#  all of these colors are defined in RGB format.
# --------------------------------------------------------
# start house/tower/world machine = (75, 30, 255)
# sun                             = (255, 150, 30)
# barrens/mineshaft               = (1, 1, 15)
# glen                            = (5, 15, 10)
# glen ruins                      = (1, 5, 1)
# glen village                    = (0, 6, 6)
# refuge                          = (255, 36, 89)
# refuge ground                   = (25, 2, 9)
# ========================================================

# each map ID corresponds to color
module GamepadMapColors
  WORLD_MACHINE = Color.new(75, 30, 255)
  SUN = Color.new(255, 150, 30)
  BARRENS = Color.new(1, 1, 15)
  GLEN = Color.new(5, 15, 10)
  GLEN_RUINS = Color.new(1, 5, 1)
  GLEN_VILLAGE = Color.new(0, 6, 6)
  REFUGE = Color.new(255, 36, 89)
  REFUGE_GROUND = Color.new(25, 2, 9)

  COLORS = {
    # Start House
    2 => WORLD_MACHINE,
    3 => WORLD_MACHINE,
    4 => WORLD_MACHINE,
    5 => WORLD_MACHINE,
    20 => WORLD_MACHINE,

    # Barrens
    7 => BARRENS,
    12 => BARRENS,
    13 => BARRENS,
    14 => BARRENS,
    15 => BARRENS,
    16 => BARRENS,
    17 => BARRENS,
    18 => BARRENS,
    19 => BARRENS,
    21 => BARRENS,
    23 => BARRENS,
    24 => BARRENS,
    69 => BARRENS,
    70 => BARRENS,
    71 => BARRENS,
    73 => BARRENS,
    75 => BARRENS,
    76 => BARRENS,
    78 => BARRENS,
    79 => BARRENS,
    80 => BARRENS,
    81 => BARRENS,
    82 => BARRENS,
    92 => BARRENS,
    93 => BARRENS,
    94 => BARRENS,
    101 => BARRENS,
    102 => BARRENS,
    194 => BARRENS,
    195 => BARRENS,
    196 => BARRENS,
    197 => BARRENS,
    210 => BARRENS,

    # Glen
    8 => GLEN,
    27 => GLEN,
    28 => GLEN,
    29 => GLEN_VILLAGE,
    30 => GLEN_VILLAGE,
    31 => GLEN_VILLAGE,
    32 => GLEN_VILLAGE,
    33 => GLEN_VILLAGE,
    34 => GLEN_VILLAGE,
    35 => GLEN_VILLAGE,
    36 => GLEN_VILLAGE,
    37 => GLEN,
    38 => GLEN_RUINS,
    39 => GLEN_RUINS,
    40 => GLEN_RUINS,
    41 => GLEN_RUINS,
    42 => GLEN_RUINS,
    44 => GLEN,
    45 => GLEN,
    46 => GLEN,
    57 => GLEN_RUINS,
    65 => GLEN_RUINS,
    66 => GLEN,
    107 => GLEN,
    126 => GLEN_RUINS,
    128 => GLEN,
    133 => GLEN,
    136 => GLEN,
    177 => GLEN,
    178 => GLEN,
    191 => GLEN,
    192 => GLEN,
    193 => GLEN,

    # Refuge Sky
    9 => REFUGE,
    22 => REFUGE,
    25 => REFUGE,
    47 => REFUGE,
    48 => REFUGE,
    49 => REFUGE,
    52 => REFUGE,
    53 => REFUGE,
    54 => REFUGE,
    59 => REFUGE,
    74 => REFUGE,
    95 => REFUGE,
    108 => REFUGE,
    109 => REFUGE,
    111 => REFUGE,
    116 => REFUGE,
    117 => REFUGE,
    119 => REFUGE,
    120 => REFUGE,
    121 => REFUGE,
    122 => REFUGE,
    124 => REFUGE,
    125 => REFUGE,
    129 => REFUGE,
    130 => REFUGE,
    132 => REFUGE,
    186 => REFUGE,
    260 => REFUGE,

    # Refuge Ground
    50 => REFUGE_GROUND,
    51 => REFUGE_GROUND,
    72 => REFUGE_GROUND,
    83 => REFUGE_GROUND,
    84 => REFUGE_GROUND,
    85 => REFUGE_GROUND,
    86 => REFUGE_GROUND,
    87 => REFUGE_GROUND,
    90 => REFUGE_GROUND,
    91 => REFUGE_GROUND,
    97 => WORLD_MACHINE,
    98 => REFUGE_GROUND,
    99 => REFUGE_GROUND,
    104 => REFUGE_GROUND,
    105 => REFUGE_GROUND,
    106 => REFUGE_GROUND,
    112 => REFUGE_GROUND,
    113 => REFUGE_GROUND,
    114 => REFUGE_GROUND,
    115 => REFUGE_GROUND,
    134 => REFUGE_GROUND,
    135 => REFUGE_GROUND,
    137 => REFUGE_GROUND,
    176 => REFUGE_GROUND,
    187 => REFUGE_GROUND,

    # Tower
    10 => WORLD_MACHINE,
    155 => WORLD_MACHINE,
    167 => WORLD_MACHINE,
    173 => WORLD_MACHINE,
    174 => WORLD_MACHINE,
    175 => WORLD_MACHINE,
    55 => WORLD_MACHINE,
    164 => WORLD_MACHINE,
    166 => WORLD_MACHINE,
    168 => WORLD_MACHINE,
    169 => WORLD_MACHINE,
    170 => WORLD_MACHINE,
    171 => WORLD_MACHINE,
    172 => WORLD_MACHINE,
    56 => WORLD_MACHINE,
    139 => WORLD_MACHINE,
    140 => WORLD_MACHINE,
    141 => WORLD_MACHINE,
    142 => WORLD_MACHINE,
    143 => WORLD_MACHINE,
    144 => WORLD_MACHINE,
    145 => WORLD_MACHINE,
    146 => WORLD_MACHINE,
    147 => WORLD_MACHINE,
    148 => WORLD_MACHINE,
    149 => WORLD_MACHINE,
    150 => WORLD_MACHINE,
    151 => WORLD_MACHINE,
    152 => WORLD_MACHINE,
    153 => WORLD_MACHINE,
    154 => WORLD_MACHINE,
    63 => WORLD_MACHINE,
    179 => WORLD_MACHINE,
    156 => WORLD_MACHINE,
    180 => WORLD_MACHINE,
    181 => WORLD_MACHINE,
    182 => WORLD_MACHINE,
    183 => WORLD_MACHINE,
    184 => WORLD_MACHINE,
    185 => WORLD_MACHINE,
    190 => WORLD_MACHINE,
    60 => WORLD_MACHINE,
    61 => WORLD_MACHINE,
    62 => WORLD_MACHINE,

    # Finale (Solstice)
    224 => WORLD_MACHINE,
    226 => WORLD_MACHINE,
    241 => WORLD_MACHINE,
    242 => WORLD_MACHINE,
    243 => WORLD_MACHINE,
    245 => WORLD_MACHINE,
    249 => WORLD_MACHINE,
    255 => WORLD_MACHINE,
    258 => WORLD_MACHINE,
    259 => WORLD_MACHINE,
  }
end

module GamepadIcons
  GAMEPADS = [nil,
    Input::GamepadType::XBOX360,
    Input::GamepadType::XBOXONE,
    :series_x,
    Input::GamepadType::PS3,
    Input::GamepadType::PS4,
    Input::GamepadType::PS5,
    Input::GamepadType::SWITCH_PRO,
    Input::GamepadType::JOYCON_PAIR,
    :luna,
    :ouya,
    :stadia,
    :steam_controller,
    :steam_deck,
    Input::GamepadType::GAMECUBE
  ]

  ICONS = {
    # SDL supported types, can be detected automatically
    # SDL_GAMEPAD_TYPE_UNKNOWN                      => UNKNOWN
    # SDL_GAMEPAD_TYPE_STANDARD                     => STANDART
    # SDL_GAMEPAD_TYPE_XBOX360                      => XBOX360
    # SDL_GAMEPAD_TYPE_XBOXONE                      => XBOXONE
    # SDL_GAMEPAD_TYPE_PS3                          => PS3
    # SDL_GAMEPAD_TYPE_PS4                          => PS4
    # SDL_GAMEPAD_TYPE_PS5                          => PS5
    # SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO          => SWITCH_PRO
    # SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT  => JOYCON_LEFT
    # SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT => JOYCON_RIGHT
    # SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR  => JOYCON_PAIR
    # SDL_GAMEPAD_TYPE_GAMECUBE                     => GAMECUBE

    Input::GamepadType::UNKNOWN => {
      :extends => Input::GamepadType::STANDART,
      :icon => [8, 16],
    },
    Input::GamepadType::STANDART => {
      :icon => [10, 13],
      :face_skinnable => true,
      :buttons => {
        Input::GamepadButton::INVALID => [9, 16],
        Input::GamepadButton::SOUTH => [0, 0],
        Input::GamepadButton::EAST => [3, 0],
        Input::GamepadButton::WEST => [1, 0],
        Input::GamepadButton::NORTH => [2, 0],
        Input::GamepadButton::BACK => [4, 5],
        Input::GamepadButton::GUIDE => [6, 13],
        Input::GamepadButton::START => [5, 5],
        Input::GamepadButton::LEFT_STICK => [4, 3],
        Input::GamepadButton::RIGHT_STICK => [5, 3],
        Input::GamepadButton::LEFT_SHOULDER => [6, 3],
        Input::GamepadButton::RIGHT_SHOULDER => [7, 3],
        Input::GamepadButton::DPAD_UP => [7, 0],
        Input::GamepadButton::DPAD_DOWN => [4, 0],
        Input::GamepadButton::DPAD_LEFT => [5, 0],
        Input::GamepadButton::DPAD_RIGHT => [6, 0],
        Input::GamepadButton::MISC1 => [0, 18],
        Input::GamepadButton::RIGHT_PADDLE1 => [7, 6],
        Input::GamepadButton::LEFT_PADDLE1 => [6, 6],
        Input::GamepadButton::RIGHT_PADDLE2 => [7, 7],
        Input::GamepadButton::LEFT_PADDLE2 => [6, 7],
        Input::GamepadButton::TOUCHPAD => [8, 16],
        Input::GamepadButton::MISC2 => [1, 18],
        Input::GamepadButton::MISC3 => [2, 18],
        Input::GamepadButton::MISC4 => [3, 18],
        Input::GamepadButton::MISC5 => [4, 18],
        Input::GamepadButton::MISC6 => [5, 18],
      },
      :axes => {
        Input::GamepadAxis::INVALID => [[9, 16], [9, 16]],
        Input::GamepadAxis::LEFTX => [[6, 1], [5, 1]],
        Input::GamepadAxis::LEFTY => [[4, 1], [7, 1]],
        Input::GamepadAxis::RIGHTX => [[6, 2], [5, 2]],
        Input::GamepadAxis::RIGHTY => [[4, 2], [7, 2]],
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 4], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[7, 4], [9, 16]],
      }
    },
    Input::GamepadType::XBOX360 => {
      :extends => Input::GamepadType::STANDART,
      :icon => [8, 14],
      :buttons => {
        Input::GamepadButton::GUIDE => [1, 17],
      }
    },
    Input::GamepadType::XBOXONE => {
      :extends => Input::GamepadType::XBOX360,
      :icon => [10, 14],
      :buttons => {
        Input::GamepadButton::BACK => [4, 4],
        Input::GamepadButton::START => [5, 4],
        Input::GamepadButton::MISC1 => [5, 6],
      }
    },
    Input::GamepadType::PS3 => {
      :extends => Input::GamepadType::STANDART,
      :icon => [8, 12],
      :buttons => {
        Input::GamepadButton::SOUTH => [0, 5],
        Input::GamepadButton::EAST => [3, 5],
        Input::GamepadButton::WEST => [1, 5],
        Input::GamepadButton::NORTH => [2, 5],
        Input::GamepadButton::BACK => [5, 12],
        Input::GamepadButton::GUIDE => [6, 18],
        Input::GamepadButton::START => [6, 12],
        Input::GamepadButton::LEFT_SHOULDER => [6, 9],
        Input::GamepadButton::RIGHT_SHOULDER => [7, 9],
      },
      :axes => {
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 10], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[7, 10], [9, 16]],
      }
    },
    Input::GamepadType::PS4 => {
      :extends => Input::GamepadType::PS3,
      :icon => [9, 12],
      :buttons => {
        Input::GamepadButton::BACK => [6, 11],
        Input::GamepadButton::START => [5, 11],
        Input::GamepadButton::TOUCHPAD => [4, 11],
      }
    },
    Input::GamepadType::PS5 => {
      :extends => Input::GamepadType::PS4,
      :icon => [10, 12],
      :buttons => {
        Input::GamepadButton::BACK => [4, 9],
        Input::GamepadButton::GUIDE => [11, 17],
        Input::GamepadButton::START => [5, 9],
        Input::GamepadButton::TOUCHPAD => [4, 10],
        Input::GamepadButton::MISC1 => [7, 11],
      }
    },
    Input::GamepadType::SWITCH_PRO => {
      :extends => Input::GamepadType::JOYCON_PAIR,
      :icon => [10, 13],
      :buttons => {
        Input::GamepadButton::DPAD_UP => [7, 0],
        Input::GamepadButton::DPAD_DOWN => [4, 0],
        Input::GamepadButton::DPAD_LEFT => [5, 0],
        Input::GamepadButton::DPAD_RIGHT => [6, 0],
      }
    },
    Input::GamepadType::JOYCON_PAIR => {
      :extends => Input::GamepadType::STANDART,
      :icon => [9, 11],
      :buttons => {
        Input::GamepadButton::SOUTH => [3, 0],
        Input::GamepadButton::EAST => [0, 0],
        Input::GamepadButton::WEST => [2, 0],
        Input::GamepadButton::NORTH => [1, 0],
        Input::GamepadButton::BACK => [5, 13],
        Input::GamepadButton::START => [4, 13],
        Input::GamepadButton::DPAD_UP => [3, 15],
        Input::GamepadButton::DPAD_DOWN => [0, 15],
        Input::GamepadButton::DPAD_LEFT => [1, 15],
        Input::GamepadButton::DPAD_RIGHT => [2, 15],
        Input::GamepadButton::LEFT_SHOULDER => [6, 8],
        Input::GamepadButton::RIGHT_SHOULDER => [7, 8],
        Input::GamepadButton::MISC1 => [4, 14],
      },
      :axes => {
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 14], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[7, 14], [9, 16]],
      }
    },
    Input::GamepadType::JOYCON_LEFT => {
      :extends => Input::GamepadType::JOYCON_PAIR,
      :icon => [10, 16],
    },
    Input::GamepadType::JOYCON_RIGHT => {
      :extends => Input::GamepadType::JOYCON_PAIR,
      :icon => [11, 16],
    },
    Input::GamepadType::GAMECUBE => {
      :extends => Input::GamepadType::STANDART,
      :face_skinnable => false,
      :icon => [8, 11],
      :buttons => {
        Input::GamepadButton::SOUTH => [8, 10],
        Input::GamepadButton::EAST => [7, 15],
        Input::GamepadButton::WEST => [9, 10],
        Input::GamepadButton::NORTH => [5, 15],
        Input::GamepadButton::START => [5, 14],
        Input::GamepadButton::LEFT_STICK => [10, 11],
        Input::GamepadButton::RIGHT_STICK => [10, 10],
        Input::GamepadButton::RIGHT_SHOULDER => [4, 15],
      },
      :axes => {
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 15], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[0, 17], [9, 16]],
        Input::GamepadAxis::LEFTX => [[10, 9], [9, 9]],
        Input::GamepadAxis::LEFTY => [[8, 9], [11, 9]],
        Input::GamepadAxis::RIGHTX => [[10, 8], [9, 8]],
        Input::GamepadAxis::RIGHTY => [[8, 8], [11, 8]],
      }
    },

    # GUID gamepads
    :series_x => {
      :extends => Input::GamepadType::XBOXONE,
      :icon => [9, 14],
      :buttons => {
        Input::GamepadButton::MISC1 => [5, 6],
      }
    },
    :luna => {
      :extends => Input::GamepadType::XBOXONE,
      :icon => [10, 11],
      :buttons => {
        Input::GamepadButton::BACK => [7, 13],
        Input::GamepadButton::GUIDE => [7, 12],
        Input::GamepadButton::START => [6, 12],
      }
    },
    :ouya => {
      :extends => Input::GamepadType::PS3,
      :icon => [11, 11],
      :buttons => {
        Input::GamepadButton::SOUTH => [0, 10],
        Input::GamepadButton::EAST => [3, 10],
        Input::GamepadButton::WEST => [1, 10],
        Input::GamepadButton::NORTH => [2, 10],
        Input::GamepadButton::GUIDE => [2, 17],
        Input::GamepadButton::TOUCHPAD => [4, 12],
      }
    },
    :steam_controller => {
      :extends => Input::GamepadType::XBOXONE,
      :icon => [8, 13],
      :buttons => {
        Input::GamepadButton::GUIDE => [5, 10],
        Input::GamepadButton::DPAD_UP => [7, 17],
        Input::GamepadButton::DPAD_DOWN => [4, 17],
        Input::GamepadButton::DPAD_LEFT => [5, 17],
        Input::GamepadButton::DPAD_RIGHT => [6, 17],
        Input::GamepadButton::RIGHT_PADDLE1 => [7, 8],
        Input::GamepadButton::LEFT_PADDLE1 => [6, 8],
        Input::GamepadButton::RIGHT_STICK => [3, 17],
      },
      :axes => {
        Input::GamepadAxis::RIGHTX => [[6, 16], [5, 16]],
        Input::GamepadAxis::RIGHTY => [[4, 16], [7, 16]],
      }
    },
    :steam_deck => {
      :extends => Input::GamepadType::XBOXONE,
      :icon => [9, 13],
      :buttons => {
        Input::GamepadButton::BACK => [4, 4],
        Input::GamepadButton::GUIDE => [4, 8],
        Input::GamepadButton::MISC1 => [5, 8],
        Input::GamepadButton::START => [5, 4],
        Input::GamepadButton::LEFT_SHOULDER => [6, 9],
        Input::GamepadButton::RIGHT_SHOULDER => [7, 9],
      },
      :axes => {
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 10], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[7, 10], [9, 16]],
      }
    },
    :stadia => {
      :extends => Input::GamepadType::XBOXONE,
      :icon => [10, 13],
      :buttons => {
        Input::GamepadButton::BACK => [5, 8],
        Input::GamepadButton::GUIDE => [8, 17],
        Input::GamepadButton::LEFT_SHOULDER => [6, 9],
        Input::GamepadButton::RIGHT_SHOULDER => [7, 9],
      },
      :axes => {
        Input::GamepadAxis::LEFT_TRIGGER => [[6, 10], [9, 16]],
        Input::GamepadAxis::RIGHT_TRIGGER => [[7, 10], [9, 16]],
      }
    }
  }

  GUIDS = {
    "030000006f0e00001301000000000000" => :series_x,
    "030000006f0e00001304000000000000" => :series_x,
    "030000006f0e00001302000000000000" => :series_x,
    "030000006f0e00003901000000000000" => :series_x,
    "030000006f0e00001413000000000000" => :series_x,
    "03000000ab1200000103000000000000" => :series_x,
    "03000000ad1b000000f9000000000000" => :series_x,
    "030000005e040000130b000000000000" => :series_x,
    "03000000373500000411000023000000" => :series_x,
    "030000005e040000050b000003090000" => :series_x,
    "030000005e040000130b000001050000" => :series_x,
    "030000005e040000130b000013050000" => :series_x,
    "030000005e040000130b000015050000" => :series_x,
    "030000005e040000130b000007050000" => :series_x,
    "030000005e040000130b000017050000" => :series_x,
    "030000005e040000130b000022050000" => :series_x,
    "030000005e040000220b000017050000" => :series_x,
    "030000005e040000220b000021050000" => :series_x,
    "03000000c82d00000a20000000020000" => :series_x,
    "03000000c82d00000020000000000000" => :series_x,
    "06000000c82d00000020000006010000" => :series_x,
    "030000005e040000120b00000b050000" => :series_x,
    "030000005e040000120b000016050000" => :series_x,
    "030000005e040000120b000017050000" => :series_x,
    "060000005e040000120b000001050000" => :series_x,
    "030000006f0e0000d702000006640000" => :series_x,
    "030000006f0e0000d802000006640000" => :series_x,
    "030000006f0e0000ef02000007640000" => :series_x,
    "03000000d62000000540000001010000" => :series_x,
    "03000000d62000000520000050010000" => :series_x,
    "03000000d62000000b20000001010000" => :series_x,
    "03000000d62000000f20000001010000" => :series_x,
    "03000000d62000006520000002010000" => :series_x,
    "030000004b2900000430000011000000" => :series_x,
    "030000005e040000120b000001050000" => :series_x,
    "030000005e040000120b000005050000" => :series_x,
    "030000005e040000120b000007050000" => :series_x,
    "030000005e040000120b000009050000" => :series_x,
    "030000005e040000120b00000d050000" => :series_x,
    "030000005e040000120b00000f050000" => :series_x,
    "030000005e040000120b000011050000" => :series_x,
    "030000005e040000120b000014050000" => :series_x,
    "030000005e040000120b000015050000" => :series_x,
    "030000005e040000130b000005050000" => :series_x,
    "050000005e040000130b000001050000" => :series_x,
    "050000005e040000130b000005050000" => :series_x,
    "050000005e040000130b000007050000" => :series_x,
    "050000005e040000130b000009050000" => :series_x,
    "050000005e040000130b000011050000" => :series_x,
    "050000005e040000130b000013050000" => :series_x,
    "050000005e040000130b000015050000" => :series_x,
    "050000005e040000130b000017050000" => :series_x,
    "060000005e040000120b000007050000" => :series_x,
    "060000005e040000120b00000b050000" => :series_x,
    "060000005e040000120b00000d050000" => :series_x,
    "060000005e040000120b00000f050000" => :series_x,
    "050000005e040000130b000022050000" => :series_x,
    "060000005e040000120b000011050000" => :series_x,
    "32386235353630393033393135613831" => :series_x,
    "050000005e040000120b000000783f00" => :series_x,
    "050000005e040000120b000000783f80" => :series_x,
    "050000005e040000130b0000ffff3f00" => :series_x,
    "65633038363832353634653836396239" => :series_x,
    "050000005e040000130b0000df870001" => :series_x,
    "050000005e040000130b0000ff870001" => :series_x,
    "0300fa675e040000ff02000000007801" => :series_x, # this is my gamepad, but for some reason it is not in the gamecontrollerdb.txt
    "03000000491900001904000000000000" => :luna,
    "03000000710100001904000000000000" => :luna,
    "03000000491900001904000001010000" => :luna,
    "03000000710100001904000000010000" => :luna,
    "03000000491900001904000011010000" => :luna,
    "05000000710100001904000000010000" => :luna,
    "32333634613735616163326165323731" => :luna,
    "416d617a6f6e2047616d6520436f6e74" => :luna,
    "4c756e612047616d6570616400000000" => :luna,
    "03000000362800000100000000000000" => :ouya,
    "05000000362800000100000002010000" => :ouya,
    "05000000362800000100000003010000" => :ouya,
    "05000000362800000100000004010000" => :ouya,
    "39383335313438623439373538343266" => :ouya,
    "4f5559412047616d6520436f6e74726f" => :ouya,
    "03000000de2800000112000001000000" => :steam_controller,
    "03000000de2800000112000011010000" => :steam_controller,
    "03000000de2800000211000001000000" => :steam_controller,
    "03000000de2800000211000011010000" => :steam_controller,
    "03000000de2800004211000001000000" => :steam_controller,
    "03000000de2800004211000011010000" => :steam_controller,
    "03000000de280000fc11000001000000" => :steam_controller,
    "05000000de2800000212000001000000" => :steam_controller,
    "05000000de2800000511000001000000" => :steam_controller,
    "05000000de2800000611000001000000" => :steam_controller,
    "30623739343039643830333266346439" => :steam_controller,
    "31643365666432386133346639383937" => :steam_controller,
    "03000000de2800000512000010010000" => :steam_deck,
    "03000000de2800000512000011010000" => :steam_deck,
    "03000000d11800000094000000000000" => :stadia,
    "03000000d11800000094000000010000" => :stadia,
    "03000000d11800000094000011010000" => :stadia,
    "05000000d11800000094000000010000" => :stadia,
    "35383633353935396534393230616564" => :stadia,
    "476f6f676c65204c4c43205374616469" => :stadia,
    "5374616469614e3848532d6532633400" => :stadia,
  }

  class << self
    def icon(gamepad_type = nil)
      current_gamepad = get_gamepad_type(gamepad_type)

      debug_string = ""
      iterations = 0

      result = nil
      while !result
        if !ICONS[current_gamepad].has_key?(:icon)
          current_gamepad = ICONS[current_gamepad][:extends]
          debug_string << "gamepad " << current_gamepad << "\n"
          iterations += 1
          if iterations > 10
            Logger.Debug debug_string
            result = [8, 16]
          end
          next
        end
        result = ICONS[current_gamepad][:icon].clone
        current_gamepad = ICONS[current_gamepad][:extends]
      end
      result
    end

    def face_skinnable(gamepad_type = nil)
      current_gamepad = get_gamepad_type(gamepad_type)

      debug_string = ""
      iterations = 0

      result = nil
      while result == nil
        if !ICONS[current_gamepad].has_key?(:face_skinnable)
          current_gamepad = ICONS[current_gamepad][:extends]
          debug_string << "gamepad " << current_gamepad << "\n"
          iterations += 1
          if iterations > 10
            Logger.Error debug_string
            result = false
          end
          next
        end
        result = ICONS[current_gamepad][:face_skinnable]
        current_gamepad = ICONS[current_gamepad][:extends]
      end
      result
    end

    def button(id, skinned = true, gamepad_type = nil)
      current_gamepad = get_gamepad_type(gamepad_type)

      debug_string = ""
      iterations = 0

      result = nil
      while !result
        if !ICONS[current_gamepad].has_key?(:buttons)
          current_gamepad = ICONS[current_gamepad][:extends]
          debug_string << current_gamepad << "\n"
          iterations += 1
          if iterations > 10
            Logger.Error debug_string
            result = [8, 16]
          end
          next
        end
        result = ICONS[current_gamepad][:buttons][id].clone
        current_gamepad = ICONS[current_gamepad][:extends]
      end
      if skinned
        if id == Input::GamepadButton::SOUTH ||
           id == Input::GamepadButton::EAST ||
           id == Input::GamepadButton::WEST ||
           id == Input::GamepadButton::NORTH
          result[1] += Settings[:gamepad_face_style]
        end
      end
      result
    end
    
    def axis(id, dir, gamepad_type = nil)
      current_gamepad = get_gamepad_type(gamepad_type)

      debug_string = ""
      iterations = 0

      result = nil
      while !result
        if !ICONS[current_gamepad].has_key?(:axes)
          current_gamepad = ICONS[current_gamepad][:extends]
          debug_string << "gamepad " << current_gamepad << "\n"
          iterations += 1
          if iterations > 10
            Logger.Error debug_string
            result = [8, 16]
          end
          next
        end
        axis = ICONS[current_gamepad][:axes][id]
        result = axis[1 - dir].clone if axis
        current_gamepad = ICONS[current_gamepad][:extends]
      end
      result
    end

    def get_gamepad_type(gamepad_override = nil)
      current_gamepad = gamepad_override || GamepadIcons::GAMEPADS[Settings[:gamepad_type]]
      if !ICONS.has_key?(current_gamepad)
        current_gamepad = GUIDS[Input::GamepadType.current_guid]
        if !ICONS.has_key?(current_gamepad)
          current_gamepad = Input::GamepadType.current_type
          if !ICONS.has_key?(current_gamepad)
            current_gamepad = Input::GamepadType::UNKNOWN
          end
        end
      end
      current_gamepad
    end
  end
end
