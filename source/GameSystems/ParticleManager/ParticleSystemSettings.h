#ifdef __cplusplus
#pragma once
#endif

#define MAX_TEXTURES_NUM 8
#define NUM_THREADS_X 1024
#define NUM_THREAD_GROUPS 32

#ifdef __cplusplus
constexpr unsigned int MAX_PARTICLES_NUM = NUM_THREADS_X * NUM_THREAD_GROUPS;
constexpr unsigned int NUM_PARTICLE_CS_DISPATCH = NUM_THREAD_GROUPS;
#endif