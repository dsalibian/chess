#include <inttypes.h>

struct {
    const char* fen;
    const uint32_t depth;
    const uint64_t result;
} const fens[] = 
{
    // https://www.chessprogramming.org/Perft_Results
    { .fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"                , .depth = 6, .result = 119060324ull   },    // startpos
    { .fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"    , .depth = 6, .result = 8031647685ull  },    // kiwipete
    { .fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"                               , .depth = 8, .result = 3009794393ull  },    // cpw3
    { .fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -"        , .depth = 6, .result = 706045033ull   },    // cpw4
    { .fen = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ -"        , .depth = 6, .result = 706045033ull   },    // cpw4 (mirrored)
    { .fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -"               , .depth = 6, .result = 3048196529ull  },    // cpw5
    { .fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -" , .depth = 6, .result = 6923051137ull  },    // cpw6

    // https://talkchess.com/viewtopic.php?p=405479
    { .fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - -"                                 , .depth = 7, .result = 1482218224ull  },
    { .fen = "r3k2r/pppp1ppp/1b3bnN/nP6/BBP1P3/q4NP1/P2P2PP/R2Q1RK1 w kq -"        , .depth = 6, .result = 538683019ull   },
    
    // https://www.talkchess.com/forum3/viewtopic.php?f=2&t=70543
    { .fen = "r3k2r/1bp2pP1/5n2/1P1Q4/1pPq4/5N2/1B1P2p1/R3K2R b KQkq c3"           , .depth = 6, .result = 8419356881ull  },
    { .fen = "r6r/1bp2pP1/R2qkn2/1P6/1pPQ4/1B3N2/1B1P2p1/4K2R b K c3"              , .depth = 6, .result = 10410709544ull },
    { .fen = "r3k2r/1bp2pP1/5n2/1P1Q4/1pPq4/5N2/1B1P2p1/R1R1K2R b KQkq c3"         , .depth = 6, .result = 9299902790ull  },
    { .fen = "8/K7/8/8/2Q1Pp1k/8/8/8 b - e3"                                       , .depth = 8, .result = 536974869ull   },

    { .fen = "r3k2r/pppp1ppp/1b4nN/nPP5/BB2Pq1b/3P1NP1/P4KPP/R2Q1R2 w kq -"        , .depth = 6, .result = 2163726473ull  },    // from cpw4 at depth 7

    // some ordinary positions
    { .fen = "r1bqk2r/pp3ppp/2nppn2/2p5/2PP4/2PBPN2/P4PPP/R1BQK2R w KQkq -"        , .depth = 6, .result = 2315917533ull  },
    { .fen = "r1bqk1nr/ppp2pbp/2np2p1/4p3/2P5/2NP2P1/PP2PPBP/R1BQK1NR w KQkq -"    , .depth = 6, .result = 2333210325ull  },
    { .fen = "1k6/8/PK6/8/8/8/8/8 b - -"                                           , .depth = 9, .result = 1554162ull     },

    // positions from the perft 7 challenge threads
    { .fen = "rnb1kbnr/pp1pp1pp/1qp2p2/8/Q1P5/N7/PP1PPPPP/1RB1KBNR b Kkq -"        , .depth = 7, .result = 14794751816ull  },   // https://www.talkchess.com/forum/viewtopic.php?t=59781  
    { .fen = "rnbq1b1r/ppppkppp/4pn2/8/1Q6/2PP4/PP2PPPP/RNB1KBNR b KQ -"           , .depth = 7, .result = 3016055832ull   },   // https://www.talkchess.com/forum/viewtopic.php?t=59818 
    { .fen = "rnbq1bnr/ppppk1pp/5p2/4p3/1Q6/2PP4/PP2PPPP/RNB1KBNR b KQ -"          , .depth = 7, .result = 4517445538ull   },   // https://www.talkchess.com/forum/viewtopic.php?t=59915 
    { .fen = "rnbqk1nr/p1pp1ppp/1p6/2b1p1B1/8/1QPP4/PP2PPPP/RN2KBNR b KQkq -"      , .depth = 7, .result = 40552058742ull  },   // https://www.talkchess.com/forum/viewtopic.php?t=59957 
    { .fen = "rnb1kbnr/ppp1pppp/8/3p4/1P6/P2P3q/2P1PPP1/RNBQKBNR b KQkq -"         , .depth = 7, .result = 44950307154ull  },   // https://www.talkchess.com/forum/viewtopic.php?t=59961 
    { .fen = "rnb1kbnr/ppp1pppp/3p4/1q6/2BPP3/8/PPPQ1PPP/RNB1K1NR b KQkq -"        , .depth = 7, .result = 115009794943ull },   // https://www.talkchess.com/forum/viewtopic.php?t=60102 
    { .fen = "rn1qkbnr/ppp2ppp/3pp3/8/3PP1b1/5Q2/PPP2PPP/RNB1KBNR b KQkq -"        , .depth = 7, .result = 75528515636ull  },   // https://www.talkchess.com/forum/viewtopic.php?t=60114 
    { .fen = "rnbqkb1r/1pppppp1/p4n1p/1B6/4P3/4Q3/PPPP1PPP/RNB1K1NR b KQkq -"      , .depth = 7, .result = 18436658361ull  },   // https://www.talkchess.com/forum/viewtopic.php?t=61329 
    { .fen = "rnbqkb1r/1pppppp1/p4n1p/1B6/4PP2/5Q2/PPPP2PP/RNB1K1NR b KQkq -"      , .depth = 7, .result = 15669029401ull  },
};
