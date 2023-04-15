#pragma once

namespace se {

class hitbox {
public:
    PAD(0x20)
    vec3 mins;
    vec3 maxs;
    const char* name;
    const char* surface_property;
    const char* bone_name;
    float shape_radius;
    uint32_t bone_name_hash;
    int32_t group_id;
    clr4 render_color;
    uint16_t hitBox_index;
	uint8_t shape_type;
    PAD(1)
    bool translation_only;	
	bool visible;	
	bool selected;
    PAD(2)
};

static_assert(sizeof(hitbox) == 0x70);

class hitbox_set {
public:
    const char* name;
    uint32_t name_hash;
    PAD(0x4)
    util_vector<hitbox> hit_boxes;
    const char* source_filename;
};

static_assert(sizeof(hitbox_set) == 0x30);

}