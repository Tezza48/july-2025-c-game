#pragma once
#include "tzl.h"

typedef size entity_id;

typedef struct transform
{
    entity_id key;

    vec3 pos;
    vec3 scale;
    vec3 rot;
} transform;

typedef struct model
{
    entity_id key;

    shader_id shader;
    mesh_id mesh;
} model;