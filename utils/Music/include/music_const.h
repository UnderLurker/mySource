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
} // namespace myUtil

#endif // _MUSIC_CONST_H
