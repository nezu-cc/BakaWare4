#include "util.h"

void se::transform::to_matrix(mat3x4& matrix) const noexcept {
    matrix[0][0] = 1.0 - 2.0 * rot.y * rot.y - 2.0 * rot.z * rot.z;
    matrix[1][0] = 2.0 * rot.x * rot.y + 2.0 * rot.w * rot.z;
    matrix[2][0] = 2.0 * rot.x * rot.z - 2.0 * rot.w * rot.y;

    matrix[0][1] = 2.0f * rot.x * rot.y - 2.0f * rot.w * rot.z;
    matrix[1][1] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z;
    matrix[2][1] = 2.0f * rot.y * rot.z + 2.0f * rot.w * rot.x;

    matrix[0][2] = 2.0f * rot.x * rot.z + 2.0f * rot.w * rot.y;
    matrix[1][2] = 2.0f * rot.y * rot.z - 2.0f * rot.w * rot.x;
    matrix[2][2] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y;

    matrix[0][3] = pos.x;
    matrix[1][3] = pos.y;
    matrix[2][3] = pos.z;
}