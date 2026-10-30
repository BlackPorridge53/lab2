#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t step0 = 3u * (uint32_t)(challenge - (challenge >> 32)) + 0xf194u;
    uint32_t step1 = ((uint32_t)(challenge >> 16) + 0xc049u) ^ (uint32_t)(challenge >> 32);
    uint32_t step2 = (3u * step1) ^ ((uint32_t)challenge + 0xc0beu);
    values[0] = step0 ^ step2;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = 3u * ((challenge >> 33) &0x3fu);
    size_t shift = 13;
    for(size_t i = 1; i < 6; i++){
        values[i] = 4u * values[i - 1] + ((uint32_t)i ^ ((challenge >> shift) & 0xfu));
        shift += 3;
    }
    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t now = (challenge >> 32) ^ (challenge >> 3);
    values[0] = challenge & 0x7u;
    switch (values[0]) {
        case 0: values[1] = (now >> 2) + (now ^ 0xfda); break;
        case 1: values[1] = (2 * now) ^ 0xb613u; break;
        case 2: values[1] = (2 * now) ^ 0xdf63u; break;
        case 3: values[1] = 5 * now + 0x2c33fu; break;
        case 4: values[1] = 3 * now + 0x4e77u; break;
        case 5: values[1] = 3 * now + 0x4653u; break;
        case 6: values[1] = (now >> 1) + (0x1d0u ^ now) ; break;
        case 7: values[1] = 5 * now + 0x29086u; break;
    }
    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk(uint32_t x1, uint32_t x2, uint32_t x3, uint32_t x4) {
    uint32_t x5 = ((x3 - x2) >> 1) + x2;
    if(x5 == x1)
        return (0xf2u + x5) ^ x4;
    uint32_t saved = x5 + x4;
    if(x1 < x5)
        return 3u * p4_walk(x1, x2, x5 - 0x1u, x4) ^ saved ^ 0x1fu;
    else
        return 3u * p4_walk(x1, x5 + 0x1u, x3, x4) ^ saved ^ 0x1eu;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = p4_walk(challenge & 0x7fu, 0u, 0x7fu, (challenge >> 32) ^ (challenge >> 7));
    return write_answer(out, cap, values, count);
}

static uint32_t p5_traverse(uint64_t x1, uint32_t *x2) {
    static const struct { uint32_t a, b; int nxt;} nodes[12] = {
        {0x9329u, 0x063bu,  3},
        {0x230au, 0x0b18u,  9},
        {0xb737u, 0x0116u,  5},
        {0x4c60u, 0x0940u,  2},
        {0x28e8u, 0x0014u,  6},
        {0xc00cu, 0x077fu, 11},
        {0x1a60u, 0x0248u,  7},
        {0x389cu, 0x0a5au,  8},
        {0xad97u, 0x034bu, -1},
        {0xed16u, 0x0429u, 10},
        {0xbda8u, 0x0886u,  0},
        {0x7612u, 0x055eu,  4}
    };
    static const int heads[8] = {1, 9, 10, 0, 3, 2, 5, 11};
    uint32_t x3 = heads[x1 & 0x7u], x4 = (x1 >> 3) & 0x3u, x5 = (x1 >> 6) ^ (x1 >> 32) ^ 0x41fbu;
    for(uint32_t i = 0; i <= x4; i++) {
        x5 = (5u * (x5 + nodes[x3].a)) ^ nodes[x3].b;
        *x2 = nodes[x3].b;
        if(i < x4) x3 = nodes[x3].nxt;
    }
    return x5;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = p5_traverse(challenge, &values[1]);
    return write_answer(out, cap, values, count);
}

static uint32_t p6_op_0(uint32_t x) {
    return (x >> 2) | (x << 30);
}

static uint32_t p6_op_1(uint32_t x) {
    return ((x >> 3) ^ x) + 0x5548u;
}

static uint32_t p6_op_2(uint32_t x) {
    return (x >> 3) ^ (x + 0xec5du);
}

static uint32_t p6_op_3(uint32_t x) {
    return (x << 5) + (x ^ 0xec5eu);
}

static uint32_t p6_op_4(uint32_t x) {
    return (2u * x + 0x8879u) ^ (7u * x);
}

static uint32_t p6_op_5(uint32_t x) {
    return ((x << 5) | (x >> 27)) ^ 0x62a9u;
}

static uint32_t p6_dispatch(uint64_t x) {
    static uint32_t (*const ops[8])(uint32_t) = {
        p6_op_1, p6_op_2, p6_op_0, p6_op_5,
        p6_op_0, p6_op_3, p6_op_1, p6_op_4
    };
    uint32_t x1 = (x >> 6) ^ (x >> 32), x2 = x & 0x7u;
    uint32_t result = ops[(x & 0x38u) >> 3](ops[x2](x1) ^ 0x1cadu);
    return result + 0x2c5cu;
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = p6_dispatch(challenge);
    return write_answer(out, cap, values, count);
}

static uint32_t secret_mix(uint64_t x) {
    return (((x << 3) | (x >> 29)) + 0x66f5u) ^ 0x924f;
}

static void secret_copy(char *dst, const char *src) {
    int i = 0;
    do {
        dst[i] = src[i];
    } while (src[i++] != '\0');
}

static int secret_gate (uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t x1 = context & 3u;
    uint32_t a = challenge ^ (challenge >> 32);
    uint32_t b = context ^ (context >> 32);
    uint32_t adj = (x1 * 0x66f5u) ^ (context >> 16);
    uint32_t x2 = secret_mix(a ^ b) ^ adj;
    uint32_t x3 = x2 ^ (b + x1 + 0x924fu + (a >> 3));
    char numbers[33];
    int n = snprintf(numbers, sizeof(numbers),  "%" PRIu32 " %" PRIu32 " %" PRIu32,  x1, x2, x3);
    if (n < 0 || (size_t)n >= sizeof(numbers))
        return 1;
    if (cap < 36u + (size_t)n + 1u)
        return 1;
    for (size_t i = 0; i < 32; i++)
        out[i] = 'A';
    out[32] = 'O', out[33] = 'P', out[34] = 'E', out[35] = 'N';
    secret_copy(out + 36, numbers);
    return 0;
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    return secret_gate(challenge, context, out, cap);
}
