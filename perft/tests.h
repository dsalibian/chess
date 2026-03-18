#include <inttypes.h>

// chess programming wiki
const char* const fen_cpw[7] =
{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",                 // startpos
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",     // kiwipete
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",                                // cpw3
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -",         // cpw4
    "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ -",         // cpw4 (mirrored)
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -",                // cpw5
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -",  // cpw6
};

const uint64_t result_cpw[7] =
{
    119060324ull,
    8031647685ull,
    11030083ull,
    706045033ull,
    706045033ull,
    3048196529ull,
    6923051137ull,
};



// positions from ccc threads
const char* const fen_ccc[11] =
{
    // https://www.talkchess.com/forum/viewtopic.php?t=49000
    "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq -",
    "3qk3/8/8/8/8/8/8/3QK3 w - -",
    "r3k2r/8/8/8/8/8/8/R3K2R w KQkq -",
    "2b1kb2/8/8/8/8/8/8/2B1KB2 w - -",
    "1n2k1n1/8/8/8/8/8/8/1N2K1N1 w - -",

    // https://talkchess.com/viewtopic.php?p=405479
    "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - -",
    "r3k2r/pppp1ppp/1b3bnN/nP6/BBP1P3/q4NP1/P2P2PP/R2Q1RK1 w kq -",

    // https://www.talkchess.com/forum3/viewtopic.php?f=2&t=70543
    "r3k2r/1bp2pP1/5n2/1P1Q4/1pPq4/5N2/1B1P2p1/R3K2R b KQkq c3",
    "r6r/1bp2pP1/R2qkn2/1P6/1pPQ4/1B3N2/1B1P2p1/4K2R b K c3",
    "r3k2r/1bp2pP1/5n2/1P1Q4/1pPq4/5N2/1B1P2p1/R1R1K2R b KQkq c3",
    "8/K7/8/8/2Q1Pp1k/8/8/8 b - e3",
};

const uint64_t result_ccc[11] =
{
    8509434052ull,
    48001999ull, 
    179862938ull,
    34853962ull,
    3736106ull,
    71179139ull,
    538683019ull,
    8419356881ull,
    10410709544ull,
    9299902790ull,
    3627518ull,
};



// depth 7 challenge positions from ccc threads
const char* const fen_challenge[9] =
{
    "rnb1kbnr/pp1pp1pp/1qp2p2/8/Q1P5/N7/PP1PPPPP/1RB1KBNR b Kkq -",             // https://www.talkchess.com/forum/viewtopic.php?t=59781
    "rnbq1b1r/ppppkppp/4pn2/8/1Q6/2PP4/PP2PPPP/RNB1KBNR b KQ -",                // https://www.talkchess.com/forum/viewtopic.php?t=59818
    "rnbq1bnr/ppppk1pp/5p2/4p3/1Q6/2PP4/PP2PPPP/RNB1KBNR b KQ -",               // https://www.talkchess.com/forum/viewtopic.php?t=59915
    "rnbqk1nr/p1pp1ppp/1p6/2b1p1B1/8/1QPP4/PP2PPPP/RN2KBNR b KQkq -",           // https://www.talkchess.com/forum/viewtopic.php?t=59957
    "rnb1kbnr/ppp1pppp/8/3p4/1P6/P2P3q/2P1PPP1/RNBQKBNR b KQkq -",              // https://www.talkchess.com/forum/viewtopic.php?t=59961
    "rnb1kbnr/ppp1pppp/3p4/1q6/2BPP3/8/PPPQ1PPP/RNB1K1NR b KQkq -",             // https://www.talkchess.com/forum/viewtopic.php?t=60102
    "rn1qkbnr/ppp2ppp/3pp3/8/3PP1b1/5Q2/PPP2PPP/RNB1KBNR b KQkq -",             // https://www.talkchess.com/forum/viewtopic.php?t=60114
    "rnbqkb1r/1pppppp1/p4n1p/1B6/4P3/4Q3/PPPP1PPP/RNB1K1NR b KQkq -",           // https://www.talkchess.com/forum/viewtopic.php?t=61329
    "rnbqkb1r/1pppppp1/p4n1p/1B6/4PP2/5Q2/PPPP2PP/RNB1K1NR b KQkq -",                                                                                       
};

const uint64_t result_challenge[9] =
{
    14794751816ull,
    3016055832ull,
    4517445538ull,
    40552058742,
    44950307154ull,
    115009794943ull,
    75528515636ull,
    18436658361ull,
    15669029401ull,
};



// some more depth 6 positions
const char* const fen_extra[8] =
{
    "r3k2r/pppp1ppp/1b4nN/nPP5/BB2Pq1b/3P1NP1/P4KPP/R2Q1R2 w kq -",         // from cpw4 at depth 7
    "rnbqk2r/ppp1ppbp/3p1np1/8/2PP4/2N2NP1/PP2PP1P/R1BQKB1R b KQkq -",
    "r1bqk2r/pp3ppp/2nppn2/2p5/2PP4/2PBPN2/P4PPP/R1BQK2R w KQkq -",
    "r1bqk1nr/ppp2pbp/2np2p1/4p3/2P5/2NP2P1/PP2PPBP/R1BQK1NR w KQkq -",
    "1k6/8/PK6/8/8/8/8/8 b - -",
    "QN4n1/6r1/3k4/8/b2K4/8/8/8 b - -",
    "R7/8/8/8/7q/2K1B2p/7P/2Bk4 w - -",
    "6kr/3qrp2/1p1p1npb/p1nP3p/P1P1pP2/1P2B2P/3NBRP1/3Q1RK1 w - -",

};

const uint64_t result_extra[8] =
{
    2163726473ull,
    1835757857ull,
    2315917533ull,
    2333210325ull,
    6319ull,
    110753562ull,
    116055946ull,
    970480180ull,
};
