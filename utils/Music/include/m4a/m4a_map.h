//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_MAP_H
#define _M4A_MAP_H

#include <map>

#include "m4a/m4a_general.h"
#include "m4a/m4a_movie.h"
#include "m4a/m4a_movie_fragments.h"
#include "m4a/m4a_sample_group.h"
#include "m4a/m4a_sample_tables.h"
#include "m4a/m4a_track.h"
#include "m4a/m4a_track_data_layout.h"
#include "m4a/m4a_track_media.h"
#include "m4a/m4a_track_time.h"
#include "m4a/m4a_user_data.h"

namespace myUtil {

using BoxCreator = std::function<std::shared_ptr<Box>()>;

const std::map<BoxType, BoxCreator> BoxMap = {
    // common
    {myUtil::BoxType::FTYP, []() { return std::make_shared<FileTypeBox>(); }                            },
    // general
    {myUtil::BoxType::MDAT, []() { return std::make_shared<MediaDataBox>(); }                           },
    {myUtil::BoxType::FREE, []() { return std::make_shared<FreeBox>(); }                                },
    {myUtil::BoxType::SKIP, []() { return std::make_shared<SkipBox>(); }                                },
    {myUtil::BoxType::PDIN, []() { return std::make_shared<ProgressiveDownloadInfoBox>(); }             },
    // movie
    {myUtil::BoxType::MOOV, []() { return std::make_shared<MovieBox>(); }                               },
    {myUtil::BoxType::MVHD, []() { return std::make_shared<MovieHeaderBox>(); }                         },
    // track
    {myUtil::BoxType::TRAK, []() { return std::make_shared<TrackBox>(); }                               },
    {myUtil::BoxType::TKHD, []() { return std::make_shared<TrackHeaderBox>(); }                         },
    {myUtil::BoxType::TREF, []() { return std::make_shared<TrackReferenceTypeBox>(); }                  },
    {myUtil::BoxType::TRGR, []() { return std::make_shared<TrackGroupTypeBox>(); }                      },
    // track media
    {myUtil::BoxType::MDIA, []() { return std::make_shared<MediaBox>(); }                               },
    {myUtil::BoxType::MDHD, []() { return std::make_shared<MediaHeaderBox>(); }                         },
    {myUtil::BoxType::HDLR, []() { return std::make_shared<HandlerBox>(); }                             },
    {myUtil::BoxType::MINF, []() { return std::make_shared<MediaInformationBox>(); }                    },
    {myUtil::BoxType::NMHD, []() { return std::make_shared<NullMediaHeaderBox>(); }                     },
    {myUtil::BoxType::ELNG, []() { return std::make_shared<ExtendedLanguageBox>(); }                    },
    // sample tables
    {myUtil::BoxType::STBL, []() { return std::make_shared<SampleTableBox>(); }                         },
    {myUtil::BoxType::STSD, []() { return std::make_shared<SampleDescriptionBox>(); }                   },
    {myUtil::BoxType::STDP, []() { return std::make_shared<DegradationPriorityBox>(); }                 },
    // track time
    {myUtil::BoxType::STTS, []() { return std::make_shared<TimeToSampleBox>(); }                        },
    {myUtil::BoxType::CTTS, []() { return std::make_shared<CompositionOffsetBox>(); }                   },
    {myUtil::BoxType::CSLG, []() { return std::make_shared<CompositionToDecodeBox>(); }                 },
    {myUtil::BoxType::STSS, []() { return std::make_shared<SyncSampleBox>(); }                          },
    {myUtil::BoxType::STSH, []() { return std::make_shared<ShadowSyncSampleBox>(); }                    },
    {myUtil::BoxType::SDTP, []() { return std::make_shared<SampleDependencyTypeBox>(); }                },
    {myUtil::BoxType::EDTS, []() { return std::make_shared<EditBox>(); }                                },
    {myUtil::BoxType::ELST, []() { return std::make_shared<EditListBox>(); }                            },
    // track data layout
    {myUtil::BoxType::DINF, []() { return std::make_shared<DataInformationBox>(); }                     },
    {myUtil::BoxType::URL,  []() { return std::make_shared<DataEntryUrlBox>(); }                        },
    {myUtil::BoxType::URN,  []() { return std::make_shared<DataEntryUrnBox>(); }                        },
    {myUtil::BoxType::DREF, []() { return std::make_shared<DataReferenceBox>(); }                       },
    {myUtil::BoxType::STSZ, []() { return std::make_shared<SampleSizeBox>(); }                          },
    {myUtil::BoxType::STZ2, []() { return std::make_shared<CompactSampleSizeBox>(); }                   },
    {myUtil::BoxType::STSC, []() { return std::make_shared<SampleToChunkBox>(); }                       },
    {myUtil::BoxType::STCO, []() { return std::make_shared<ChunkOffsetBox>(); }                         },
    {myUtil::BoxType::CO64, []() { return std::make_shared<ChunkLargeOffsetBox>(); }                    },
    {myUtil::BoxType::PADB, []() { return std::make_shared<PaddingBitsBox>(); }                         },
    {myUtil::BoxType::SUBS, []() { return std::make_shared<SubSampleInformationBox>(); }                },
    {myUtil::BoxType::SAIZ, []() { return std::make_shared<SampleAuxiliaryInformationSizesBox>(); }     },
    {myUtil::BoxType::SAIO, []() { return std::make_shared<SampleAuxiliaryInformationOffsetsBox>(); }   },
    // movie fragments
    {myUtil::BoxType::MVEX, []() { return std::make_shared<MovieExtendsBox>(); }                        },
    {myUtil::BoxType::MEHD, []() { return std::make_shared<MovieExtendsHeaderBox>(); }                  },
    {myUtil::BoxType::TREX, []() { return std::make_shared<TrackExtendsBox>(); }                        },
    {myUtil::BoxType::MOOF, []() { return std::make_shared<MovieFragmentBox>(); }                       },
    {myUtil::BoxType::MFHD, []() { return std::make_shared<MovieFragmentHeaderBox>(); }                 },
    {myUtil::BoxType::TRAF, []() { return std::make_shared<TrackFragmentBox>(); }                       },
    {myUtil::BoxType::TFHD, []() { return std::make_shared<TrackFragmentHeaderBox>(); }                 },
    {myUtil::BoxType::TRUN, []() { return std::make_shared<TrackRunBox>(); }                            },
    {myUtil::BoxType::MFRA, []() { return std::make_shared<MovieFragmentRandomAccessBox>(); }           },
    {myUtil::BoxType::TFRA, []() { return std::make_shared<TrackFragmentRandomAccessBox>(); }           },
    {myUtil::BoxType::MFRO, []() { return std::make_shared<MovieFragmentRandomAccessOffsetBox>(); }     },
    {myUtil::BoxType::TFDT, []() { return std::make_shared<TrackFragmentBaseMediaDecodeTimeBox>(); }    },
    {myUtil::BoxType::LEVA, []() { return std::make_shared<LevelAssignmentBox>(); }                     },
    {myUtil::BoxType::TREP, []() { return std::make_shared<TrackExtensionPropertiesBox>(); }            },
    {myUtil::BoxType::ASSP, []() { return std::make_shared<AlternativeStartupSequencePropertiesBox>(); }},
    // sample group
    {myUtil::BoxType::SBGP, []() { return std::make_shared<SampleToGroupBox>(); }                       },
    {myUtil::BoxType::SGPD, []() { return std::make_shared<SampleGroupDescriptionBox>(); }              },
    // user data
    {myUtil::BoxType::UDTA, []() { return std::make_shared<UserDataBox>(); }                            },
    {myUtil::BoxType::CPRT, []() { return std::make_shared<CopyrightBox>(); }                           },
    {myUtil::BoxType::TSEL, []() { return std::make_shared<TrackSelectionBox>(); }                      },
    {myUtil::BoxType::KIND, []() { return std::make_shared<KindBox>(); }                                },
    {myUtil::BoxType::META, []() { return std::make_shared<MetaBox>(); }                                },
    {myUtil::BoxType::XML,  []() { return std::make_shared<XMLBox>(); }                                 },
    {myUtil::BoxType::BXML, []() { return std::make_shared<BinaryXMLBox>(); }                           },
    {myUtil::BoxType::ILOC, []() { return std::make_shared<ItemLocationBox>(); }                        },
    {myUtil::BoxType::PITM, []() { return std::make_shared<PrimaryItemBox>(); }                         },
    {myUtil::BoxType::IPRO, []() { return std::make_shared<ItemProtectionBox>(); }                      },
    {myUtil::BoxType::IINF, []() { return std::make_shared<ItemInfoBox>(); }                            },
    {myUtil::BoxType::INFE, []() { return std::make_shared<ItemInfoEntry>(); }                          },
};
} // namespace myUtil

#endif // _M4A_MAP_H
