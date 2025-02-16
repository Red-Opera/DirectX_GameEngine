#include "stdafx.h"

DirectX::XMMATRIX Math::MultipleMatrixScale(DirectX::XMMATRIX matrix, float scale)
{
    matrix.r[3].m128_f32[0] *= scale;
    matrix.r[3].m128_f32[1] *= scale;
    matrix.r[3].m128_f32[2] *= scale;

    return matrix;
}
