#include "features.h"
#include "../entity_cache.h"

struct record {
public:
    bool valid = { };
    float simulation_time = { };
    std::array<cs::bone_data, MAX_STUDIO_BONES> bone_data = { };
    vec3 head_pos = { };
};

struct record_set {
public:
    record* record1;
    record* record2;
    float fraction;
};

class backtrack_entity {
public:
    cs::handle<> handle = {};
    std::vector<record> records = {};
    bool last_valid = { };
    cs::model* model = { };
    uint32_t num_bones = { };

    backtrack_entity(cs::handle<> handle) noexcept : handle(handle) {
        ASSERT(cheat::global_vars);
        CONVAR(sv_maxunlag, float);

        const auto max_tics = sv_maxunlag / cheat::global_vars->interval_per_tick2;
        records.resize(max_tics);
        records.resize(records.capacity()); // basically free, so why not
    }

    void save_data(unsigned int index) noexcept {
        auto& rec = records[index % records.size()];

        auto controller = handle.get<cs::player_controller>();
        if (!controller || controller->m_bIsLocalPlayerController() || !controller->m_bPawnIsAlive()) {
            invalidate();
            return;
        }
        
        auto player = controller->m_hPawn().get();
        if (!player) {
            invalidate();
            return;
        }

        if (!cheat::local->is_enemy(player)) {
            invalidate();
            return;
        }

        rec.simulation_time = player->m_flSimulationTime();
        rec.head_pos = player->get_eye_pos();

        const auto game_scene_node = player->m_pGameSceneNode();
        if (!game_scene_node) {
            invalidate();
            return;
        }
        
        const auto skeleton = game_scene_node->get_skeleton_instance();
        if (!skeleton){
            invalidate();
            return;
        }
        
        skeleton->calc_world_space_bones(cs::bone_flags::FLAG_HITBOX);

        auto& model_state = skeleton->m_modelState();
        model = model_state.m_hModel();
        if (!model) {
            invalidate();
            return;
        }

        num_bones = model->num_bones();
        if (num_bones == 0) {
            invalidate();
            return;
        }

        auto bone_data = model_state.get_bone_data();

        if (num_bones > MAX_STUDIO_BONES)
            num_bones = MAX_STUDIO_BONES;

        memcpy(rec.bone_data.data(), bone_data, sizeof(cs::bone_data) * num_bones);
        rec.valid = true;
        last_valid = true;
    }

    float closest_record(vec3 start, vec3 end, float las_valid_sim_time) {
        float closest = std::numeric_limits<float>::max();

        for (auto& rec : records) {
            if (!rec.valid)
                continue;

            if (rec.simulation_time <= las_valid_sim_time)
                continue;

            float dist = get_distance(rec.head_pos, start, end);
            if (dist < closest)
                closest = dist;
        }

        return closest;
    }

    record_set find_best_records(vec3 start, vec3 end, float las_valid_sim_time) {

        int best_bone = -1;
        int best_record = -1;
        float best_bone_dist = std::numeric_limits<float>::max();
        
        for (int bone_index = 0; bone_index < (int)num_bones; bone_index++) {
             if (!(model->bone_flags(bone_index) & cs::bone_flags::FLAG_HITBOX)) {
                continue;
            }

            auto parent_bone_index = model->bone_parent(bone_index);
            if (parent_bone_index == -1)
                continue;

            for (int record_index = 0; record_index < std::size(records); record_index++) {
                const auto& rec = records[record_index];
                if (!rec.valid)
                    continue;
                
                if (rec.simulation_time <= las_valid_sim_time)
                    continue;

                const float bone_dist = math::segment_dist(start, end, rec.bone_data[bone_index].pos, rec.bone_data[parent_bone_index].pos);
                if (bone_dist < best_bone_dist) {
                    best_bone_dist = bone_dist;
                    best_bone = bone_index;
                    best_record = record_index;
                }
            }
        }

        ASSERT(best_bone != -1 && best_record != -1);
        
        int prev_record = best_record - 1;
        if (prev_record < 0)
            prev_record += records.size();
        if (!records[prev_record].valid)
            prev_record = best_record;
        
        int next_record = (best_record + 1) % records.size();
        if (!records[next_record].valid)
            next_record = best_record;

        const auto& best_rec = records[best_record];
        uint32_t best_bone_parent = model->bone_parent(best_bone);


        float prev_dist = math::segment_dist(
            start, end, 
            records[prev_record].bone_data[best_bone].pos, 
            records[prev_record].bone_data[best_bone_parent].pos
        );
        float next_dist = math::segment_dist(
            start, end, 
            records[next_record].bone_data[best_bone].pos, 
            records[next_record].bone_data[best_bone_parent].pos
        );

        bool prev_is_best = prev_dist < next_dist;
        int rec1_index = prev_is_best ? prev_record : best_record;
        int rec2_index = prev_is_best ? best_record : next_record;

        // records are the same
        if (rec1_index == rec2_index)
            return { &records[rec1_index], &records[rec2_index], 0.0f };
        
        const float best_full_dist = (prev_is_best ? prev_dist : next_dist) + best_bone_dist;
        const float fraction = prev_is_best ? prev_dist / best_full_dist : best_bone_dist / best_full_dist;

        return { &records[rec1_index], &records[rec2_index], fraction };
    }

private:

    void invalidate() noexcept {
        if (last_valid) {
            for (auto& rec : records)
                rec.valid = false;
            last_valid = false;
        }
    }

    float get_distance(vec3 p, vec3 a, vec3 b) {
        vec3 ap = p - a;
        vec3 ab = b - a;
        
        float ab2 = ab.dot_product(ab);
        float ap_ab = ap.dot_product(ab);
        float t = ap_ab / ab2;

        // ignore if player is behind ur or too far away
        if (t < 0.0f || t > 1.0f)
            return std::numeric_limits<float>::max();
        
        vec3 nearest = a + ab * t;
        return (p - nearest).length();
    }
};

std::unordered_map<int, std::unique_ptr<backtrack_entity>> records { };

void features::backtrack::initialize() noexcept {
    entity_cache::register_callback(
        entity_cache::entity_type::PLAYER,
        [](int index, const cs::handle<> handle) noexcept {
            records.insert_or_assign(index, std::make_unique<backtrack_entity>(handle));
        },
        [](int index, const cs::handle<> handle) noexcept {
            records.erase(index);
        }
    );
}

// TODO: not sure if this is correct
float calc_lerp() noexcept {
    CONVAR(cl_interp, float);
    CONVAR(cl_updaterate, int);

	auto lerp = cl_interp / cl_updaterate;

	if (lerp <= cl_interp)
		lerp = cl_interp;

	return lerp;
}

float get_las_valid_sim_time() noexcept {
    CONVAR(sv_maxunlag, float);
    const auto nci = interfaces::engine->get_net_channel_info(0);
    if (!nci)
        return std::numeric_limits<float>::max();

    const auto latency = nci->get_latency(se::flow::FLOW_OUTGOING) + nci->get_latency(se::flow::FLOW_INCOMING);
    const float correct = std::clamp(latency + calc_lerp(), 0.0f, sv_maxunlag);
    const float max_delta = std::min((sv_maxunlag - correct), cfg.legit.backtrack.max_time_ms / 1000.f);

    return cheat::global_vars->curtime - max_delta; 
}

void features::backtrack::render(render::renderer *r) noexcept {
    if (!cfg.legit.backtrack.enabled || !cfg.legit.backtrack.skeleton)
        return;

    const float las_valid_sim_time = get_las_valid_sim_time();
    for (const auto& [index, entity] : records) {
        if (!entity->last_valid)
            continue;
        
        record const* last_record = nullptr;
        float last_record_time = std::numeric_limits<float>::max();

        for (const auto& rec : entity->records) {
            if (!rec.valid || rec.simulation_time <= las_valid_sim_time)
                continue;

            if (rec.simulation_time < last_record_time) {
                last_record_time = rec.simulation_time;
                last_record = &rec;
            }
        }

        if (!last_record)
            continue;

        for (uint32_t i = 0; i < entity->num_bones; i++) {
            if (!(entity->model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
                continue;
            }

            auto parent_index = entity->model->bone_parent(i);
            if (parent_index == -1)
                continue;

            vec2 screen_pos, screen_parent_pos;
            if (!math::world_to_screen(last_record->bone_data[i].pos, screen_pos))
                continue;
            if (!math::world_to_screen(last_record->bone_data[parent_index].pos, screen_parent_pos))
                continue;

            r->line(
                screen_pos.x, screen_pos.y,
                screen_parent_pos.x, screen_parent_pos.y,
                clr4::cyan()
            );
        }
    }
}

void features::backtrack::create_move(se::user_cmd *cmd) noexcept {
    if (!cfg.legit.backtrack.enabled)
        return;

    for (auto& [index, entity] : records) {
        entity->save_data(cmd->base->command_number);
    }

    // DEBUG_SECTION("BACKTRACK2", [&](const debug_overlay::section_accessor& section) {
    //     const float las_valid_sim_time = get_las_valid_sim_time();

    //     for (const auto& [index, entity] : records) {
    //         if (!entity->last_valid)
    //             continue;

    //         for (const auto& rec : entity->records) {
    //             if (!rec.valid || rec.simulation_time <= las_valid_sim_time)
    //                 continue;

    //             for (uint32_t i = 0; i < entity->num_bones; i++) {
    //                 if (!(entity->model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
    //                     continue;
    //                 }

    //                 auto parent_index = entity->model->bone_parent(i);
    //                 if (parent_index == -1)
    //                     continue;
                    
    //                 section.line(
    //                     rec.bone_data[i].pos,
    //                     rec.bone_data[parent_index].pos,
    //                     clr4::cyan()
    //                 );
    //             }
    //         }
    //     }
    // });

    // only backtrack if we're attacking
    // FIXME: full auto, burst
    if (cmd->attack1_start_history_index == -1)
        return;

    const auto sub_tick = cmd->sub_tick_container.get_history_entry(cmd->attack1_start_history_index);

    if (!sub_tick->view_angles || !sub_tick->sv_interp0 || !sub_tick->sv_interp1) {
        LOG_ERROR(XOR("invalid sub tick"));
        return;
    }
    
    using cheat::local;
    if (!local || !local.valid())
        return;

    auto active_weapon = local->get_active_weapon();
    if (!active_weapon)
        return;
    
    auto v_data = active_weapon->get_v_data();
    // FIXME: knife, revolver
    if (!v_data || v_data->m_bMeleeWeapon())
        return;

    vec3 forward = math::angle_vector(sub_tick->view_angles->angles);
    vec3 start_pos = local->get_eye_pos();
    vec3 end_pos = start_pos + (forward * v_data->m_flRange());
    const float las_valid_sim_time = get_las_valid_sim_time();

    int target_index = -1;
    float target_index_dist = std::numeric_limits<float>::max();
    for (auto& [index, entity] : records) {
        if (!entity->last_valid)
            continue;
        const float distance = entity->closest_record(start_pos, end_pos, las_valid_sim_time);
        if (distance < target_index_dist) {
            target_index = index;
            target_index_dist = distance;
        }
    }

    // no entities or to far away
    if (target_index == -1 || target_index_dist > 100.0f)
        return;
    
    const auto entity = records[target_index].get();

    const auto records = entity->find_best_records(start_pos, end_pos, las_valid_sim_time);

    const float time_per_tick = cheat::global_vars->interval_per_tick2;
    const float best_time = std::lerp(records.record1->simulation_time, records.record2->simulation_time, records.fraction);
    const float best_tick_time = best_time / time_per_tick;
    int best_tick = (int)std::floor(best_tick_time);
    const float best_tick_fraction = best_tick_time - best_tick;

    best_tick -= 1; // FIXME: engine prediction?
    LOG_INFO(XOR("backtracking to: {} + {} ({:.2f} ms)"), best_tick, best_tick_fraction, (cheat::global_vars->curtime - best_time) * 1000.f);

    sub_tick->cl_interp->src_tick = best_tick;
    sub_tick->cl_interp->dst_tick = best_tick + 1;
    sub_tick->cl_interp->frac = best_tick_fraction;

    sub_tick->sv_interp0->src_tick = best_tick;
    sub_tick->sv_interp0->dst_tick = best_tick + 1;
    sub_tick->sv_interp0->frac = 0.f;

    sub_tick->sv_interp1->src_tick = best_tick + 1;
    sub_tick->sv_interp1->dst_tick = best_tick + 2;
    sub_tick->sv_interp1->frac = 0.f;


    // DEBUG_SECTION("BACKTRACK", [&](const debug_overlay::section_accessor& section) {
    //     for (uint32_t i = 0; i < entity->num_bones; i++) {
    //         if (!(entity->model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
    //             continue;
    //         }

    //         auto parent_index = entity->model->bone_parent(i);
    //         if (parent_index == -1)
    //             continue;
            
    //         section.line(
    //             records.record1->bone_data[i].pos,
    //             records.record1->bone_data[parent_index].pos,
    //             clr4::red()
    //         );
    //     }

    //     for (uint32_t i = 0; i < entity->num_bones; i++) {
    //         if (!(entity->model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
    //             continue;
    //         }

    //         auto parent_index = entity->model->bone_parent(i);
    //         if (parent_index == -1)
    //             continue;
            
    //         section.line(
    //             records.record2->bone_data[i].pos,
    //             records.record2->bone_data[parent_index].pos,
    //             clr4::yellow()
    //         );
    //     }

    //     for (uint32_t i = 0; i < entity->num_bones; i++) {
    //         if (!(entity->model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
    //             continue;
    //         }

    //         auto parent_index = entity->model->bone_parent(i);
    //         if (parent_index == -1)
    //             continue;
            
    //         section.line(
    //             math::lerp(records.record1->bone_data[i].pos, records.record2->bone_data[i].pos, records.fraction),
    //             math::lerp(records.record1->bone_data[parent_index].pos, records.record2->bone_data[parent_index].pos, records.fraction),
    //             clr4::white()
    //         );
    //     }
    // });
}