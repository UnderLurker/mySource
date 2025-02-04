//
// Created by XW on 2024/3/16.
//

#ifndef _MUSIC_CONST_H
#define _MUSIC_CONST_H

#include <map>

namespace myUtil {

enum MusicStatus {
    SUCCESS = 0,
    ERROR_FILE_FORMAT,
    ERROR_UNKNOWN,
    ERROR_FILE_PATH
};

enum MusicVersion {
    MPEG2_5 = 0,
    MPEG2   = 2,
    MPEG1   = 3
};

enum MusicLayer {
    LAYER3 = 1,
    LAYER2 = 2,
    LAYER1 = 3
};

typedef std::pair<std::pair<MusicVersion, MusicLayer>, uint8_t> BitRateKey;
const static std::map<BitRateKey, int16_t> bitRateMap {
  // {{{MusicVersion, MusicLayer}, bitrateIndex}, value}
    {{{MPEG1, LAYER1}, 0},    0  },
    {{{MPEG1, LAYER1}, 1},    32 },
    {{{MPEG1, LAYER1}, 2},    64 },
    {{{MPEG1, LAYER1}, 3},    96 },
    {{{MPEG1, LAYER1}, 4},    128},
    {{{MPEG1, LAYER1}, 5},    160},
    {{{MPEG1, LAYER1}, 6},    192},
    {{{MPEG1, LAYER1}, 7},    224},
    {{{MPEG1, LAYER1}, 8},    256},
    {{{MPEG1, LAYER1}, 9},    288},
    {{{MPEG1, LAYER1}, 10},   320},
    {{{MPEG1, LAYER1}, 11},   352},
    {{{MPEG1, LAYER1}, 12},   384},
    {{{MPEG1, LAYER1}, 13},   416},
    {{{MPEG1, LAYER1}, 14},   448},
    {{{MPEG1, LAYER1}, 15},   -1 },

    {{{MPEG1, LAYER2}, 0},    0  },
    {{{MPEG1, LAYER2}, 1},    32 },
    {{{MPEG1, LAYER2}, 2},    48 },
    {{{MPEG1, LAYER2}, 3},    56 },
    {{{MPEG1, LAYER2}, 4},    64 },
    {{{MPEG1, LAYER2}, 5},    80 },
    {{{MPEG1, LAYER2}, 6},    96 },
    {{{MPEG1, LAYER2}, 7},    112},
    {{{MPEG1, LAYER2}, 8},    128},
    {{{MPEG1, LAYER2}, 9},    160},
    {{{MPEG1, LAYER2}, 10},   192},
    {{{MPEG1, LAYER2}, 11},   224},
    {{{MPEG1, LAYER2}, 12},   256},
    {{{MPEG1, LAYER2}, 13},   320},
    {{{MPEG1, LAYER2}, 14},   384},
    {{{MPEG1, LAYER2}, 15},   -1 },

    {{{MPEG1, LAYER3}, 0},    0  },
    {{{MPEG1, LAYER3}, 1},    32 },
    {{{MPEG1, LAYER3}, 2},    40 },
    {{{MPEG1, LAYER3}, 3},    48 },
    {{{MPEG1, LAYER3}, 4},    56 },
    {{{MPEG1, LAYER3}, 5},    64 },
    {{{MPEG1, LAYER3}, 6},    80 },
    {{{MPEG1, LAYER3}, 7},    96 },
    {{{MPEG1, LAYER3}, 8},    112},
    {{{MPEG1, LAYER3}, 9},    128},
    {{{MPEG1, LAYER3}, 10},   160},
    {{{MPEG1, LAYER3}, 11},   192},
    {{{MPEG1, LAYER3}, 12},   224},
    {{{MPEG1, LAYER3}, 13},   256},
    {{{MPEG1, LAYER3}, 14},   320},
    {{{MPEG1, LAYER3}, 15},   -1 },

    {{{MPEG2, LAYER1}, 0},    0  },
    {{{MPEG2, LAYER1}, 1},    32 },
    {{{MPEG2, LAYER1}, 2},    64 },
    {{{MPEG2, LAYER1}, 3},    96 },
    {{{MPEG2, LAYER1}, 4},    128},
    {{{MPEG2, LAYER1}, 5},    160},
    {{{MPEG2, LAYER1}, 6},    192},
    {{{MPEG2, LAYER1}, 7},    224},
    {{{MPEG2, LAYER1}, 8},    256},
    {{{MPEG2, LAYER1}, 9},    288},
    {{{MPEG2, LAYER1}, 10},   320},
    {{{MPEG2, LAYER1}, 11},   352},
    {{{MPEG2, LAYER1}, 12},   384},
    {{{MPEG2, LAYER1}, 13},   416},
    {{{MPEG2, LAYER1}, 14},   448},
    {{{MPEG2, LAYER1}, 15},   -1 },

    {{{MPEG2_5, LAYER1}, 0},  0  },
    {{{MPEG2_5, LAYER1}, 1},  32 },
    {{{MPEG2_5, LAYER1}, 2},  48 },
    {{{MPEG2_5, LAYER1}, 3},  56 },
    {{{MPEG2_5, LAYER1}, 4},  64 },
    {{{MPEG2_5, LAYER1}, 5},  80 },
    {{{MPEG2_5, LAYER1}, 6},  96 },
    {{{MPEG2_5, LAYER1}, 7},  112},
    {{{MPEG2_5, LAYER1}, 8},  128},
    {{{MPEG2_5, LAYER1}, 9},  144},
    {{{MPEG2_5, LAYER1}, 10}, 160},
    {{{MPEG2_5, LAYER1}, 11}, 176},
    {{{MPEG2_5, LAYER1}, 12}, 192},
    {{{MPEG2_5, LAYER1}, 13}, 224},
    {{{MPEG2_5, LAYER1}, 14}, 256},
    {{{MPEG2_5, LAYER1}, 15}, -1 },

    {{{MPEG2, LAYER2}, 0},    0  },
    {{{MPEG2, LAYER2}, 1},    32 },
    {{{MPEG2, LAYER2}, 2},    48 },
    {{{MPEG2, LAYER2}, 3},    56 },
    {{{MPEG2, LAYER2}, 4},    64 },
    {{{MPEG2, LAYER2}, 5},    80 },
    {{{MPEG2, LAYER2}, 6},    96 },
    {{{MPEG2, LAYER2}, 7},    112},
    {{{MPEG2, LAYER2}, 8},    128},
    {{{MPEG2, LAYER2}, 9},    160},
    {{{MPEG2, LAYER2}, 10},   192},
    {{{MPEG2, LAYER2}, 11},   224},
    {{{MPEG2, LAYER2}, 12},   256},
    {{{MPEG2, LAYER2}, 13},   320},
    {{{MPEG2, LAYER2}, 14},   384},
    {{{MPEG2, LAYER2}, 15},   -1 },

    {{{MPEG2_5, LAYER2}, 0},  0  },
    {{{MPEG2_5, LAYER2}, 1},  8  },
    {{{MPEG2_5, LAYER2}, 2},  16 },
    {{{MPEG2_5, LAYER2}, 3},  24 },
    {{{MPEG2_5, LAYER2}, 4},  32 },
    {{{MPEG2_5, LAYER2}, 5},  40 },
    {{{MPEG2_5, LAYER2}, 6},  48 },
    {{{MPEG2_5, LAYER2}, 7},  56 },
    {{{MPEG2_5, LAYER2}, 8},  64 },
    {{{MPEG2_5, LAYER2}, 9},  80 },
    {{{MPEG2_5, LAYER2}, 10}, 96 },
    {{{MPEG2_5, LAYER2}, 11}, 112},
    {{{MPEG2_5, LAYER2}, 12}, 128},
    {{{MPEG2_5, LAYER2}, 13}, 144},
    {{{MPEG2_5, LAYER2}, 14}, 160},
    {{{MPEG2_5, LAYER2}, 15}, -1 },

    {{{MPEG2, LAYER3}, 0},    0  },
    {{{MPEG2, LAYER3}, 1},    8  },
    {{{MPEG2, LAYER3}, 2},    16 },
    {{{MPEG2, LAYER3}, 3},    24 },
    {{{MPEG2, LAYER3}, 4},    32 },
    {{{MPEG2, LAYER3}, 5},    64 },
    {{{MPEG2, LAYER3}, 6},    80 },
    {{{MPEG2, LAYER3}, 7},    56 },
    {{{MPEG2, LAYER3}, 8},    64 },
    {{{MPEG2, LAYER3}, 9},    128},
    {{{MPEG2, LAYER3}, 10},   160},
    {{{MPEG2, LAYER3}, 11},   112},
    {{{MPEG2, LAYER3}, 12},   128},
    {{{MPEG2, LAYER3}, 13},   256},
    {{{MPEG2, LAYER3}, 14},   320},
    {{{MPEG2, LAYER3}, 15},   -1 },

    {{{MPEG2_5, LAYER3}, 0},  0  },
    {{{MPEG2_5, LAYER3}, 1},  8  },
    {{{MPEG2_5, LAYER3}, 2},  16 },
    {{{MPEG2_5, LAYER3}, 3},  24 },
    {{{MPEG2_5, LAYER3}, 4},  32 },
    {{{MPEG2_5, LAYER3}, 5},  40 },
    {{{MPEG2_5, LAYER3}, 6},  48 },
    {{{MPEG2_5, LAYER3}, 7},  56 },
    {{{MPEG2_5, LAYER3}, 8},  64 },
    {{{MPEG2_5, LAYER3}, 9},  80 },
    {{{MPEG2_5, LAYER3}, 10}, 96 },
    {{{MPEG2_5, LAYER3}, 11}, 112},
    {{{MPEG2_5, LAYER3}, 12}, 128},
    {{{MPEG2_5, LAYER3}, 13}, 144},
    {{{MPEG2_5, LAYER3}, 14}, 160},
    {{{MPEG2_5, LAYER3}, 15}, -1 },
};

const static std::map<std::pair<MusicVersion, uint8_t>, float> MusicSamplingRateMap {
    {{MPEG1, 0},   44.1  },
    {{MPEG1, 1},   48    },
    {{MPEG1, 2},   32    },
    {{MPEG2, 0},   22.05 },
    {{MPEG2, 1},   24    },
    {{MPEG2, 2},   16    },
    {{MPEG2_5, 0}, 11.025},
    {{MPEG2_5, 1}, 12    },
    {{MPEG2_5, 2}, 8     }
};

const static std::map<std::pair<MusicVersion, MusicLayer>, uint16_t> MusicSamplingCountMap {
    {{MPEG1, LAYER1},   384 },
    {{MPEG1, LAYER2},   1152},
    {{MPEG1, LAYER3},   1152},
    {{MPEG2, LAYER1},   384 },
    {{MPEG2, LAYER2},   1152},
    {{MPEG2, LAYER3},   576 },
    {{MPEG2_5, LAYER1}, 384 },
    {{MPEG2_5, LAYER2}, 1152},
    {{MPEG2_5, LAYER3}, 576 },
};

enum MusicChannel {
    STEREO = 0,
    JOINT_STEREO,
    DOUBLE_MONO,
    MONO
};

struct MusicExtension {
    bool intensityStereo;
    bool msStereo;
};

const static std::map<uint8_t, MusicExtension> MusicExtensionMap {
    {0, {false, false}},
    {1, {true, false} },
    {2, {false, true} },
    {3, {true, true}  }
};

enum MusicEmphasis {
    FIFTY_OR_FIFTEEN = 1,
    CCITT_J17        = 3
};

std::unordered_map<uint8_t, std::string> GenreMap {
    {0,   "Blues"                }, {1,   "ClassicRock"          }, {2,   "Country"              },
    {3,   "Dance"                }, {4,   "Disco"                }, {5,   "Funk"                 },
    {6,   "Grunge"               }, {7,   "Hip-Hop"              }, {8,   "Jazz"                 },
    {9,   "Metal"                }, {10,  "NewAge"               }, {11,  "Oldies"               },
    {12,  "Other"                }, {13,  "Pop"                  }, {14,  "R&B"                  },
    {15,  "Rap"                  }, {16,  "Reggae"               }, {17,  "Rock"                 },
    {18,  "Techno"               }, {19,  "Industrial"           }, {20,  "Alternative"          },
    {21,  "Ska"                  }, {22,  "Deathl"               }, {23,  "Pranks"               },
    {24,  "Soundtrack"           }, {25,  "Euro-Techno"          }, {26,  "Ambient"              },
    {27,  "Trip-Hop"             }, {28,  "Vocal"                }, {29,  "Jazz+Funk"            },
    {30,  "Fusion"               }, {31,  "Trance"               }, {32,  "Classical"            },
    {33,  "Instrumental"         }, {34,  "Acid"                 }, {35,  "House"                },
    {36,  "Game"                 }, {37,  "SoundClip"            }, {38,  "Gospel"               },
    {39,  "Noise"                }, {40,  "AlternRock"           }, {41,  "Bass"                 },
    {42,  "Soul"                 }, {43,  "Punk"                 }, {44,  "Space"                },
    {45,  "Meditative"           }, {46,  "InstrumentalPop"      }, {47,  "InstrumentalRock"     },
    {48,  "Ethnic"               }, {49,  "Gothic"               }, {50,  "Darkwave"             },
    {51,  "Techno-Industrial"    }, {52,  "Electronic"           }, {53,  "Pop-Folk"             },
    {54,  "Eurodance"            }, {55,  "Dream"                }, {56,  "SouthernRock"         },
    {57,  "Comedy"               }, {58,  "Cult"                 }, {59,  "Gangsta"              },
    {60,  "Top40"                }, {61,  "ChristianRap"         }, {62,  "Pop/Funk"             },
    {63,  "Jungle"               }, {64,  "NativeAmerican"       }, {65,  "Cabaret"              },
    {66,  "NewWave"              }, {67,  "Psychadelic"          }, {68,  "Rave"                 },
    {69,  "Showtunes"            }, {70,  "Trailer"              }, {71,  "Lo-Fi"                },
    {72,  "Tribal"               }, {73,  "AcidPunk"             }, {74,  "AcidJazz"             },
    {75,  "Polka"                }, {76,  "Retro"                }, {77,  "Musical"              },
    {78,  "Rock&Roll"            }, {79,  "HardRock"             }, {80,  "Folk"                 },
    {81,  "Folk-Rock"            }, {82,  "NationalFolk"         }, {83,  "Swing"                },
    {84,  "FastFusion"           }, {85,  "Bebob"                }, {86,  "Latin"                },
    {87,  "Revival"              }, {88,  "Celtic"               }, {89,  "Bluegrass"            },
    {90,  "Avantgarde"           }, {91,  "GothicRock"           }, {92,  "ProgessiveRock"       },
    {93,  "PsychedelicRock"      }, {94,  "SymphonicRock"        }, {95,  "SlowRock"             },
    {96,  "BigBand"              }, {97,  "Chorus"               }, {98,  "EasyListening"        },
    {99,  "Acoustic"             }, {100, "Humour"               }, {101, "Speech"               },
    {102, "Chanson"              }, {103, "Opera"                }, {104, "ChamberMusic"         },
    {105, "Sonata"               }, {106, "Symphony"             }, {107, "BootyBass"            },
    {108, "Primus"               }, {109, "PornGroove"           }, {110, "Satire"               },
    {111, "SlowJam"              }, {112, "Club"                 }, {113, "Tango"                },
    {114, "Samba"                }, {115, "Folklore"             }, {116, "Ballad"               },
    {117, "PowerBallad"          }, {118, "RhythmicSoul"         }, {119, "Freestyle"            },
    {120, "Duet"                 }, {121, "PunkRock"             }, {122, "DrumSolo"             },
    {123, "Acapella"             }, {124, "Euro-House"           }, {125, "DanceHall"            },
    {126, "Goa"                  }, {127, "Drum&Bass"            }, {128, "Club-House"           },
    {129, "Hardcore"             }, {130, "Terror"               }, {131, "Indie"                },
    {132, "BritPop"              }, {133, "Negerpunk"            }, {134, "PolskPunk"            },
    {135, "Beat"                 }, {136, "ChristianGangstaRap"  }, {137, "Heavyl"               },
    {138, "Blackl"               }, {139, "Crossover"            }, {140, "ContemporaryChristian"},
    {141, "ChristianRock"        }, {142, "Merengue"             }, {143, "Salsa"                },
    {144, "Trashl"               }, {145, "Anime"                }, {146, "JPop"                 },
    {147, "Synthpop"             },
};
} // namespace myUtil

#endif // _MUSIC_CONST_H
