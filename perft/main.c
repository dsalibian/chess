#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef u64 bitboard;

u32 popcnt(const u64 x) {
    return __builtin_popcountll(x);
}

u32 tzcnt(const u64 x) {
    assert(x);

    return __builtin_ctzll(x);
}

u32 lzcnt(const u64 x) {
    assert(x);

    return __builtin_clzll(x);
}

u64 iso_lsb(const u64 x) {
    return x & -x;
}

u64 pop_lsb(const u64 x) {
    return x & (x - 1);
}

bitboard sqr_bb(const u32 sqr) {
    assert(sqr < 64);

    return 1ull << sqr;
}

void print_bb(const bitboard bb) {
    for(i32 r = 7; r > -1; --r) {
        printf(" %d   ", r + 1);

        for(i32 f = 0; f < 8; ++f)
            printf("%c ", ((bb >> (8 * r + f)) & 1) ? '1' : '.');

        printf("\n");
    }

    printf("\n     a b c d e f g h\n\n");
}

u64 time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (u64)ts.tv_sec * 1000000000ull + (u64)ts.tv_nsec;
}








const bitboard FILE_A  = 0x0101010101010101ull;
const bitboard FILE_B  = 0x0202020202020202ull;
const bitboard FILE_C  = 0x0404040404040404ull;
const bitboard FILE_D  = 0x0808080808080808ull;
const bitboard FILE_E  = 0x1010101010101010ull;
const bitboard FILE_F  = 0x2020202020202020ull;
const bitboard FILE_G  = 0x4040404040404040ull;
const bitboard FILE_H  = 0x8080808080808080ull;

const bitboard RANK_1  = 0x00000000000000ffull;
const bitboard RANK_2  = 0x000000000000ff00ull;
const bitboard RANK_3  = 0x0000000000ff0000ull;
const bitboard RANK_4  = 0x00000000ff000000ull;
const bitboard RANK_5  = 0x000000ff00000000ull;
const bitboard RANK_6  = 0x0000ff0000000000ull;
const bitboard RANK_7  = 0x00ff000000000000ull;
const bitboard RANK_8  = 0xff00000000000000ull;

enum square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};

enum color { WHITE, BLACK };

enum piece_type {
    PAWN,
    NIGHT,
    BISHP,
    ROOK,
    QUEEN,
    KING,

    EMPTY,
};









enum direction {
    N = 1u << 0,
    S = 1u << 1,
    E = 1u << 2,
    W = 1u << 3,

    NE = N | E,
    NW = N | W,
    SE = S | E,
    SW = S | W,
};

bitboard shift(bitboard bb, const u32 dir) {
    switch(dir) {
        case N:  return bb << 8;
        case S:  return bb >> 8;
        case E:  return (bb << 1) & ~FILE_A;
        case W:  return (bb >> 1) & ~FILE_H;
        case NE: return (bb << 9) & ~FILE_A;
        case NW: return (bb << 7) & ~FILE_H;
        case SE: return (bb >> 7) & ~FILE_A;
        case SW: return (bb >> 9) & ~FILE_H;
    }

    __builtin_unreachable();
}









#include "tables.h"

#ifdef __BMI2__

#include <immintrin.h>

#define RATTS_BB(s, all)    (_satts[s][false][_pext_u64(all, _relevant[s][false])])
#define BATTS_BB(s, all)    (_satts[s][true ][_pext_u64(all, _relevant[s][true])])

#else 
    
#define RATTS_BB(s, all)    (_satts[s][false][(_magic[s][false] * (all & _relevant[s][false])) >> _shamt[s][false]])
#define BATTS_BB(s, all)    (_satts[s][true ][(_magic[s][true ] * (all & _relevant[s][true ])) >> _shamt[s][true]])

#endif

#define QATTS_BB(s, all)    (RATTS_BB(s, all) | BATTS_BB(s, all))

#define PATTS_BB(s, turn)   (_patts[s][turn])
#define KATTS_BB(s)         (_katts[s])
#define NATTS_BB(s)         (_natts[s])

#define BETWEEN_BB(a, b)    (_between[a][b])
#define THROUGH_BB(a, b)    (_through[a][b])









struct move {
    u8 from, to, type, promo;
};

struct move mv_encode(const u32 mt, const u32 from, const u32 to) {
    return (struct move){.promo = 0, .type = mt, .from = from, .to = to};
}

struct move mv_encode_p(const u32 mt, const u32 pt, const u32 from, const u32 to) {
    return (struct move){.promo = pt, .type = mt, .from = from, .to = to};
}

void mv_print(const struct move m, const bool println) {
    printf("%c%c%c%c", 
            'a' + (m.from % 8),
            '1' + (m.from / 8),
            'a' + (m.to % 8),
            '1' + (m.to / 8));

    if(m.promo)
        printf("%c", (char[]){'n', 'b', 'r', 'q'}[m.promo - NIGHT]);

    if(println)
        printf("\n");
}

struct mstack {
    struct move arr[255];
    u32 size;
};

struct mstack ms_new() { return (struct mstack){.size = 0}; }

struct move ms_pop(struct mstack* ms) {
    assert(ms->size);

    return ms->arr[--(ms->size)];
}

void ms_push(struct mstack* ms, const struct move m) {
    assert(ms->size < 255);

    ms->arr[(ms->size)++] = m;
}

void ms_print(struct mstack* ms) {
    const u32 u = ms->size;

    for(u32 i = 0; i < u; ++i) {
        printf("%3u: ", i);
        mv_print(ms->arr[i], true);
    }
}









enum {
    F_OOW  = 1u << 0,
    F_OOB  = 1u << 1,
    F_OOOW = 1u << 2,
    F_OOOB = 1u << 3,
};

const u8 _rights[64] =
{
    0xbu, 0xfu, 0xfu, 0xfu, 0xau, 0xfu, 0xfu, 0xeu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 0xfu, 
    0x7u, 0xfu, 0xfu, 0xfu, 0x5u, 0xfu, 0xfu, 0xdu, 
};

#define SETRIGHTS(rights, to, from) ((rights) & _rights[to] & _rights[from])









struct position {
    bitboard type[5], color[2];
    u8 ksqr[2], ep_target, castle;
    bool turn;
};

#define COLOR_BB(p, c)          ((p)->color[c]) 
#define TYPE_BB(p, pt)          ((p)->type[pt]) 

#define SLIDERS_BB(p, pt, c)    ((TYPE_BB(p, pt) | TYPE_BB(p, QUEEN)) & COLOR_BB(p, c))
#define PC_BB(p, pt, c)         (TYPE_BB(p, pt) & COLOR_BB(p, c))

#define ALL_BB(p)               (COLOR_BB(p, WHITE) | COLOR_BB(p, BLACK))

void pos_fen(struct position* p, const char* fen) {
    const char startpos[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
    if(!fen)
        fen = startpos;

    memset(p, 0, sizeof(struct position));

    for(i32 r = 7, f = 0; *fen != ' '; ++fen) {
        char c = *fen;
        
        if(c == '/') {
            --r;
            f = 0;

            continue;
        }

        if(isdigit(c)) {
            f += c - '0';

            continue;
        }

        i32 s = 8 * r + f;
        u32 pt;

        switch(tolower(c)) {
            case 'p': pt = PAWN;  break;
            case 'n': pt = NIGHT; break;
            case 'b': pt = BISHP; break;
            case 'r': pt = ROOK;  break;
            case 'q': pt = QUEEN; break;
            case 'k': pt = KING;  break;
            default: 
                      assert(0);
                      __builtin_unreachable();
        };

        bool b = !!islower(c);

        p->color[b] |= sqr_bb(s);

        if(pt == KING)
            p->ksqr[b] = s;
        else
            p->type[pt] |= sqr_bb(s);

        ++f;
    }

    for(; *fen == ' '; ++fen);

    p->turn = *fen == 'w' ? WHITE : BLACK;
    for(; *(++fen) == ' ';);

    for(; *fen != ' '; ++fen) {
        if(*fen == 'K') p->castle |= F_OOW;
        if(*fen == 'k') p->castle |= F_OOB;
        if(*fen == 'Q') p->castle |= F_OOOW;
        if(*fen == 'q') p->castle |= F_OOOB;
    }
    for(; *(++fen) == ' ';);

    p->ep_target = *fen == '-' ? 0 : (*(fen + 1) - '0' - 1) * 8 + (*fen - 'a');
}

void pos_startpos(struct position* p) {
    pos_fen(p, NULL);
}

void pos_print(const struct position* p) {
    for(u32 r = 7; r < 8; --r) {
        printf(" %d   ", r + 1);

        for(u32 f = 0; f < 8; ++f) {
            u32 s = 8 * r + f;

            u32 pt;

            if((p->color[0] | p->color[1]) & sqr_bb(s)) {
                for(pt = PAWN; pt < KING; ++pt)
                    if(sqr_bb(s) & p->type[pt])
                        break;
            } else 
                pt = EMPTY;

            char c = (char[]){'p', 'n', 'b', 'r', 'q', 'k', '.'}[pt];
        
            printf("%c ", (sqr_bb(s) & p->color[WHITE]) ? toupper(c) : c);
        }

        printf("\n");
    }

    printf("\n     a b c d e f g h\n\n");

    printf("turn:   %c\n", p->turn == WHITE ? 'w' : 'b');

    if(p->ep_target)
        printf("ep:     %c%c\n", 'a' + (p->ep_target % 8), '1' + (p->ep_target / 8));
    else
        printf("ep:     none\n");

    printf("oo_w:   %c\n",   p->castle & F_OOW  ? 'y' : 'n');
    printf("ooo_w:  %c\n",   p->castle & F_OOOW ? 'y' : 'n');
    printf("oo_b:   %c\n",   p->castle & F_OOB  ? 'y' : 'n');
    printf("ooo_b:  %c\n\n", p->castle & F_OOOB ? 'y' : 'n');
}

bool sqr_attd(const struct position* p, const u32 sqr, const bool color) {
    return  
        (KATTS_BB(sqr)            & sqr_bb    (p->ksqr[color]))  || 
        (PATTS_BB(sqr, !color)    & PC_BB     (p, PAWN,  color)) || 
        (NATTS_BB(sqr)            & PC_BB     (p, NIGHT, color)) || 
        (BATTS_BB(sqr, ALL_BB(p)) & SLIDERS_BB(p, BISHP, color)) || 
        (RATTS_BB(sqr, ALL_BB(p)) & SLIDERS_BB(p, ROOK,  color));
}

bitboard gencheckers(const struct position* p) {
    const bool c = p->turn;
    const u32 s  = p->ksqr[c];

    return  
        (PATTS_BB(s, c)         & PC_BB     (p, PAWN,  !c)) | 
        (NATTS_BB(s)            & PC_BB     (p, NIGHT, !c)) | 
        (BATTS_BB(s, ALL_BB(p)) & SLIDERS_BB(p, BISHP, !c)) | 
        (RATTS_BB(s, ALL_BB(p)) & SLIDERS_BB(p, ROOK,  !c));
}

bitboard genpins(const struct position* p) {
    const bool color = p->turn;
    const u32 ks = p->ksqr[color];

    const bitboard potentially_pinned = QATTS_BB(ks, ALL_BB(p)) & COLOR_BB(p, color);
    const bitboard all_nopinned       = ALL_BB(p) ^ potentially_pinned;

    const bitboard xray_b = BATTS_BB(ks, all_nopinned);
    const bitboard xray_r = RATTS_BB(ks, all_nopinned);

    const bitboard opps_b = SLIDERS_BB(p, BISHP, !color);
    const bitboard opps_r = SLIDERS_BB(p, ROOK,  !color);

    bitboard pinner = (xray_b & opps_b) | (xray_r & opps_r);

    bitboard bb = 0;
    for(; pinner; pinner = pop_lsb(pinner))
        bb |= BETWEEN_BB(ks, tzcnt(pinner));

    return bb;
}

void makemv(struct position* p, const struct move m) {
    const u32 from          = m.from;
    const u32 to            = m.to;
    const u32 moved         = m.type;

    const bitboard from_bb  = sqr_bb(from);
    const bitboard to_bb    = sqr_bb(to);
    const bitboard mv_bb    = from_bb | to_bb;

    const bool turn         = p->turn;
    const bool w            = turn == WHITE;

    const bitboard opps     = COLOR_BB(p, !turn);
    const u32 ep            = p->ep_target;

    p->ep_target            = 0;
    p->castle               = SETRIGHTS(p->castle, to, from);
    p->turn                 = !p->turn;

    p->color[turn] ^= mv_bb;

    if(to_bb & opps) {
        p->color[!turn] &= ~to_bb;

        for(u32 pt = PAWN; pt < KING; ++pt) 
            p->type[pt] &= ~to_bb;
    }

    switch(moved) {
        case KING:
            p->ksqr[turn] = to;

            if(to == from + 2) {
                const bitboard rbb = w ? 0xa0ull : 0xa0ull << (7 * 8);

                p->type[ROOK]  ^= rbb;
                p->color[turn] ^= rbb;
            }
            else if(to == from - 2) {
                const bitboard rbb = w ? 0x9ull : 0x9ull << (7 * 8);

                p->type[ROOK]  ^= rbb;
                p->color[turn] ^= rbb;
            }

            break;

        case PAWN:
            if(m.promo) {
                p->type[PAWN]    ^= to_bb;
                p->type[m.promo] ^= to_bb;
            }

            else if(to == ep) {
                const bitboard cbb = shift(to_bb, w ? S : N);

                p->type[PAWN]   ^= cbb;
                p->color[!turn] ^= cbb;
            }

            else if(to - from == 16 || to - from == -16)
                p->ep_target = (to + from) / 2;

        default:
            p->type[moved] ^= mv_bb;
    }
}









void _mvs_push_mvbb(struct mstack* ms, const struct position* p, const u32 mt, const u32 from, bitboard bb) {
    for(; bb; bb = pop_lsb(bb)) 
        ms_push(ms, mv_encode(mt, from, tzcnt(bb)));
}

void _mvs_push_promo(struct mstack* ms, const u32 from, const u32 to) {
    for(u32 pt = NIGHT; pt < KING; ++pt)
        ms_push(ms, mv_encode_p(PAWN, pt, from, to));
}

void _mvs_push_pawnbb(struct mstack* ms, bitboard to, const u32 diff, const bool promo) {
    for(; to; to = pop_lsb(to)) {
        u32 to_idx = tzcnt(to);

        if(promo) _mvs_push_promo(ms, to_idx + diff, to_idx);
        else      ms_push(ms, mv_encode(PAWN, to_idx + diff, to_idx)); 
    } 
}

u64 _mvs(struct mstack* ms, const struct position* p, const bool turn, const bool gen) {
    const bool w = turn == WHITE;

    const bitboard all      = ALL_BB(p);
    const bitboard empty    = ~all;
    const bitboard us       = COLOR_BB(p, turn);
    const bitboard opps     = COLOR_BB(p, !turn);
    const u32      ksqr     = p->ksqr[turn];

    const bitboard checker  = gencheckers(p); 
    const bitboard pin      = genpins(p);
    const bitboard pinned   = us & pin;

    const bitboard dpush    = w ? RANK_2 : RANK_7;
    const bitboard promo    = w ? RANK_7 : RANK_2;
    const bitboard final    = w ? RANK_8 : RANK_1;

    const u32 up            = w ? N  : S;
    const u32 upr           = w ? NE : SW;
    const u32 upl           = w ? NW : SE;

    const u32 up_d          = w ? -8  : 8;
    const u32 upr_d         = w ? -9  : 9;
    const u32 upl_d         = w ? -7  : 7;
    const u32 up2_d         = w ? -16 : 16;

    u64 count = 0;

    bitboard pc, atts, target, which = us & ~pinned;
    u32 from;

    if(checker) {
        if(pop_lsb(checker)) 
            which = target = 0;
        else 
            target = BETWEEN_BB(ksqr, tzcnt(checker));
        
    } else {
        target = ~us;

        for(pc = TYPE_BB(p, PAWN) & pinned; pc; pc = pop_lsb(pc)) {
            bitboard from_bb = iso_lsb(pc);
            from = tzcnt(pc);

            bitboard thru = THROUGH_BB(ksqr, from);
            bitboard push = shift(from_bb, up) & empty;

            if(atts = ((PATTS_BB(from, turn) & opps) | push) & thru) {
                u32 to = tzcnt(atts);

                if(from_bb & promo) {
                    if(gen) _mvs_push_promo(ms, from, to);
                    else    count += 4;
                }
                else {
                    if(gen) ms_push(ms, mv_encode(PAWN, from, to));
                    else    ++count;

                    if((from_bb & dpush) && (atts = shift(push, up) & empty & thru)) {
                        if(gen) ms_push(ms, mv_encode(PAWN, from, to - up_d));
                        else    ++count;
                    }
                }
            }
        }



        for(pc = TYPE_BB(p, BISHP) & pinned; pc; pc = pop_lsb(pc)) {
            from = tzcnt(pc);
            atts = BATTS_BB(from, all) & ~us & THROUGH_BB(ksqr, from);

            if(gen) _mvs_push_mvbb(ms, p, BISHP, from, atts);
            else    count += popcnt(atts); 
        }

        for(pc = TYPE_BB(p, ROOK) & pinned; pc; pc = pop_lsb(pc)) {
            from = tzcnt(pc);
            atts = RATTS_BB(from, all) & ~us & THROUGH_BB(ksqr, from);

            if(gen) _mvs_push_mvbb(ms, p, ROOK, from, atts);
            else    count += popcnt(atts);
        }

        for(pc = TYPE_BB(p, QUEEN) & pinned; pc; pc = pop_lsb(pc)) {
            from = tzcnt(pc);
            atts = QATTS_BB(from, all) & ~us & THROUGH_BB(ksqr, from);

            if(gen) _mvs_push_mvbb(ms, p, QUEEN, from, atts);
            else    count += popcnt(atts);
        }



        if(
                (p->castle & (w ? F_OOW : F_OOB))         && 
                !(all & (w ? 0x60ull : 0x60ull << 7 * 8)) &&
                !sqr_attd(p, ksqr + 1, !turn)             &&
                !sqr_attd(p, ksqr + 2, !turn)) 
        {
            if(gen) ms_push(ms, mv_encode(KING, ksqr, ksqr + 2));
            else    ++count;
        }

        if(
                (p->castle & (w ? F_OOOW : F_OOOB))     && 
                !(all & (w ? 0xeull : 0xeull << 7 * 8)) &&
                !sqr_attd(p, ksqr - 1, !turn)           &&
                !sqr_attd(p, ksqr - 2, !turn)) 
        {

            if(gen) ms_push(ms, mv_encode(KING, ksqr, ksqr - 2));
            else    ++count;
        }
    }

    if(p->ep_target && (!checker || (PATTS_BB(ksqr, turn) & PC_BB(p, PAWN, !turn)))) {
        const u32 ep = p->ep_target;

        for(pc = PATTS_BB(p->ep_target, !turn) & PC_BB(p, PAWN, turn); pc; pc = pop_lsb(pc)) {
            from = tzcnt(pc);

            const bitboard all_nop  = ALL_BB(p) ^ (sqr_bb(from) | sqr_bb(ep) | sqr_bb(ep + up_d));

            const bitboard xray_b   = BATTS_BB(ksqr, all_nop);
            const bitboard xray_r   = RATTS_BB(ksqr, all_nop);

            const bitboard opps_b   = SLIDERS_BB(p, BISHP, !turn);
            const bitboard opps_r   = SLIDERS_BB(p, ROOK,  !turn);

            if(!(xray_b & opps_b) && !(xray_r & opps_r)) {
                if(gen) ms_push(ms, mv_encode(PAWN, from, ep));
                else    ++count;
            }
        }
    }

    pc = TYPE_BB(p, PAWN) & which;

    atts = shift(pc, up) & empty & target; 
    if(gen) {
        _mvs_push_pawnbb(ms, atts & ~final, up_d, false);
        _mvs_push_pawnbb(ms, atts &  final, up_d, true);
    } else {
        count += popcnt(atts &  final) * 4;
        count += popcnt(atts & ~final);
    }

    atts = shift(pc, upr) & opps & target; 
    if(gen) {
        _mvs_push_pawnbb(ms, atts & ~final, upr_d, false);
        _mvs_push_pawnbb(ms, atts &  final, upr_d, true);
    } else {
        count += popcnt(atts &  final) * 4;
        count += popcnt(atts & ~final);
    }

    atts = shift(pc, upl) & opps & target; 
    if(gen) {
        _mvs_push_pawnbb(ms, atts & ~final, upl_d, false);
        _mvs_push_pawnbb(ms, atts &  final, upl_d, true);
    } else {
        count += popcnt(atts &  final) * 4;
        count += popcnt(atts & ~final);
    }

    atts = shift(shift(pc & dpush, up) & empty, up) & empty & target;
    if(gen) _mvs_push_pawnbb(ms, atts, up2_d, false);
    else    count += popcnt(atts);



    for(pc = TYPE_BB(p, NIGHT) & which; pc; pc = pop_lsb(pc)) {
        from = tzcnt(pc);
        atts = NATTS_BB(from) & target;

        if(gen) _mvs_push_mvbb(ms, p, NIGHT, from, atts);
        else    count += popcnt(atts);
    }

    for(pc = TYPE_BB(p, BISHP) & which; pc; pc = pop_lsb(pc)) {
        from = tzcnt(pc);
        atts = BATTS_BB(from, all) & target;

        if(gen) _mvs_push_mvbb(ms, p, BISHP, from, atts);
        else    count += popcnt(atts);
    }

    for(pc = TYPE_BB(p, ROOK) & which; pc; pc = pop_lsb(pc)) {
        from = tzcnt(pc);
        atts = RATTS_BB(from, all) & target;

        if(gen) _mvs_push_mvbb(ms, p, ROOK, from, atts);
        else    count += popcnt(atts);
    }

    for(pc = TYPE_BB(p, QUEEN) & which; pc; pc = pop_lsb(pc)) {
        from = tzcnt(pc);
        atts = QATTS_BB(from, all) & target;

        if(gen) _mvs_push_mvbb(ms, p, QUEEN, from, atts);
        else    count += popcnt(atts);
    }



    const bitboard all_nok = all ^ sqr_bb(ksqr);

    for(pc = KATTS_BB(ksqr) & ~us; pc; pc = pop_lsb(pc)) {
        u32 to = tzcnt(pc);

        if(
                !(KATTS_BB(to)          & sqr_bb    (p->ksqr[!turn]))  &&
                !(PATTS_BB(to, turn)    & PC_BB     (p, PAWN,  !turn)) &&
                !(NATTS_BB(to)          & PC_BB     (p, NIGHT, !turn)) &&
                !(BATTS_BB(to, all_nok) & SLIDERS_BB(p, BISHP, !turn)) &&
                !(RATTS_BB(to, all_nok) & SLIDERS_BB(p, ROOK,  !turn)))
        {
            if(gen) ms_push(ms, mv_encode(KING, ksqr, to));
            else    ++count;
        }
    }

    return count;
}

__attribute__((flatten))
void genmvsw(struct mstack* ms, const struct position* p) {
    _mvs(ms, p, WHITE, true);
}

__attribute__((flatten))
void genmvsb(struct mstack* ms, const struct position* p) {
    _mvs(ms, p, BLACK, true);
}

__attribute__((flatten))
u64 cntmvsw(const struct position* p) {
    return _mvs(NULL, p, WHITE, false);
}

__attribute__((flatten))
u64 cntmvsb(const struct position* p) {
    return _mvs(NULL, p, BLACK, false);
}









u64 _perft(const struct position* p, const u32 depth, const bool div) {
    const bool w = p->turn == WHITE;

    if(depth < 2)
        return depth ? (w ? cntmvsw : cntmvsb)(p) : 1;

    struct mstack ms = ms_new();
    (w ? genmvsw : genmvsb)(&ms, p);

    u64 c = 0;

    for(; ms.size; ) {
        struct move m = ms_pop(&ms);

        struct position p2 = *p;
        makemv(&p2, m);
        
        u64 t = _perft(&p2, depth - 1, false);
        c += t;

        if(div) {
            mv_print(m, false);
            printf(" %lu\n", t);
        }
    }

    return c;
}









struct task {
    struct position p;
    u32 depth;
    u64 result;
};

struct {
    struct task* ls;
    size_t len, cap;
    pthread_mutex_t lk;

} tasks = {.lk = PTHREAD_MUTEX_INITIALIZER};


void tasks_push(const struct position* p, const u32 depth) {
    tasks.ls[tasks.len++] = (struct task){.p = *p, .depth = depth};
}

struct task* tasks_pop() {
    struct task* t;

    pthread_mutex_lock(&(tasks.lk));

    t = tasks.len ? &tasks.ls[--tasks.len] : NULL;

    pthread_mutex_unlock(&(tasks.lk));

    return t;
}

void* worker(void* p) {
    for(struct task* t; t = tasks_pop();) 
        t->result = _perft(&t->p, t->depth, false);

    return NULL;
}

void gentasks(const struct position* p, const u32 depth, const u32 taskd) {
    if(!depth) {
        tasks_push(p, taskd); 

        return;
    }

    struct mstack ms = ms_new();
    (p->turn == WHITE ? genmvsw : genmvsb)(&ms, p);

    for(; ms.size; ) {
        struct move m = ms_pop(&ms);

        struct position p2 = *p;
        makemv(&p2, m);

        gentasks(&p2, depth - 1, taskd);
    }
}

u64 perft(const char* fen, const u32 depth, const u8 threadc, const bool div, const bool print) {
    struct position p;
    pos_fen(&p, fen);

    u64 result, t;

    if(threadc > 1) {
        u32 d;
        for(d = 0; ; ++d)
            if((tasks.cap = _perft(&p, d, false)) >= threadc)
                break;

        tasks.ls = malloc(tasks.cap *sizeof(struct task));
        gentasks(&p, d, depth - d);

        pthread_t threads[threadc];

        t = time_ns();

        for(u32 i = 0; i < threadc; ++i) 
            pthread_create(threads + i, NULL, worker, NULL);

        for(u32 i = 0; i < threadc; ++i) 
            pthread_join(threads[i], NULL);

        result = 0;
        for(size_t i = 0; i < tasks.cap; ++i)
            result += tasks.ls[i].result;
    } else {
        t = time_ns();

        result = _perft(&p, depth, div);
    }

    t = time_ns() - t;

    if(print)
        printf("\n%lu\n%.0fms (%.0f nps)\n", result, t / 1e6, (double)result / t * 1e9);

    return result;
}









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

    // from cpw4 at depth 7 
    { .fen = "r3k2r/pppp1ppp/1b4nN/nPP5/BB2Pq1b/3P1NP1/P4KPP/R2Q1R2 w kq -"        , .depth = 6, .result = 2163726473ull  },

    // some ordinary positions
    { .fen = "r1bqk2r/pp3ppp/2nppn2/2p5/2PP4/2PBPN2/P4PPP/R1BQK2R w KQkq -"        , .depth = 6, .result = 2315917533ull  },
    { .fen = "r1bqk1nr/ppp2pbp/2np2p1/4p3/2P5/2NP2P1/PP2PPBP/R1BQK1NR w KQkq -"    , .depth = 6, .result = 2333210325ull  },
    { .fen = "1k6/8/PK6/8/8/8/8/8 b - -"                                           , .depth = 9, .result = 1554162ull     },

    // positions from some perft 7 challenge threads
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

void runtests(const bool all) {
    const u32 u = all ? 26 : 14;

    u64 t = time_ns();

    for(u32 i = 0; i < u; ++i) {
        printf("\r%u / %u", i, u);
        fflush(stdout);

        if(perft(fens[i].fen, fens[i].depth, 1, false, false) != fens[i].result)
            printf("\nfailed: %s\n", fens[i].fen);
    }    

    printf("\ncompleted tests in %.0fs\n", (time_ns() - t) / 1e9);
}

void pgo() {
    struct position p;

    pos_fen(&p, NULL);
    _perft(&p, 6, false);

    pos_fen(&p, fens[1].fen);
    _perft(&p, 6, false);
}









int main(int argc, char** argv) {
    const char* fen = NULL;
    u32 threadc = 1;
    u32 depth = 6;
    bool div = false;

    for(u32 i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "--pgo")) {
            pgo();

            return 0;
        }

        else if(!strcmp(argv[i], "--test")) {
            runtests(i + 1 < argc && !strcmp(argv[i + 1], "all"));

            return 0;
        }

        else if(!strcmp(argv[i], "--fen"))
            fen = argv[++i];

        else if(!strcmp(argv[i], "--kiwipete")) 
            fen = fens[1].fen;

        else if(!strcmp(argv[i], "--depth")) 
            depth = atoi(argv[++i]);

        else if(!strcmp(argv[i], "--threads")) 
            threadc = atoi(argv[++i]);

        else if(!strcmp(argv[i], "--div")) 
            div = true;

        else if(!strcmp(argv[i], "--help")) {
            printf(
                    "usage: ./a.out [options]\n\n"
                    "options:\n"
                    "--fen \"\"    : set root position (default: start position)\n"
                    "--depth n   : search to depth n (default: 6)\n"
                    "--threads n : use n threads     (default: 1, will ignore --div flag if n > 1)\n"
                    "--kiwipete  : use kiwipete position\n"
                    "--div       : show result for each move\n");

            return 0;
        }
        
        else {
            printf("invalid option \"%s\"\n", argv[i]);

            return 1;
        }
    }

    printf("\ngoing perft with fen=%s, depth=%u, threads=%u, div=%s\n",
            fen ? fen : "startpos", depth, threadc, (div &= threadc < 2) ? "true" : "false");
    fflush(stdout);

    perft(fen, depth, threadc, div, true);

    return 0;
}
