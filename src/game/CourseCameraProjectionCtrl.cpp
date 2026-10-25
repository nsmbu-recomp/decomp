#include <game/CourseCameraProjectionCtrl.h>
#include <game/CourseCameraUpdater.h>

#include <cstring>

namespace {

void transformMatrixBlocks_(sead::Matrix44f* mtx);
void scaleProjectionRows_(CourseCameraUpdater* updater, sead::Matrix44f* matrix);
void mtx44RowMul_(sead::Matrix44f* dst, const sead::Matrix44f* a, const sead::Matrix44f* b, u32 row_index);
void mtx44CopyRow_(sead::Matrix44f* dst, const sead::Matrix44f* src, u32 row_index);

}

// Address: 0x02A05C00
void transformMatrixBlocks_(sead::Matrix44f* mtx)
{
    sead::Matrix44f tmp_a;
    sead::Matrix44f tmp_b;

    mtx44RowMul_(&tmp_a, mtx, mtx, 0);
    mtx44CopyRow_(mtx, &tmp_a, 0);

    mtx44RowMul_(&tmp_b, mtx, mtx, 1);
    mtx44CopyRow_(mtx, &tmp_b, 1);

    mtx44RowMul_(&tmp_a, &tmp_b, mtx, 0);
    mtx44CopyRow_(mtx, &tmp_a, 0);

    mtx44RowMul_(&tmp_b, &tmp_a, mtx, 1);
    mtx44CopyRow_(mtx, &tmp_b, 1);
}

void scaleProjectionRows_(CourseCameraUpdater* updater, sead::Matrix44f* matrix)
{
    f32* m = reinterpret_cast<f32*>(matrix);
    f32 scale_x = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(updater) + 0x88);
    f32 scale_y = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(updater) + 0x8c);

    m[8] = m[8] * scale_x;
    m[9] = m[9] * scale_x;
    m[10] = (m[14] * scale_y + m[10]) * scale_x;
    m[11] = m[11] * scale_x + m[15] * scale_y;
}

// Address: 0x02A05C68
void CourseCameraApplyProjection(CourseCameraUpdater* updater, sead::Matrix44f* matrix, void* copy_dest, u32 mode)
{
    std::memcpy(copy_dest, matrix, sizeof(sead::Matrix44f));

    f32* m = reinterpret_cast<f32*>(matrix);

    if (mode >= 3)
    {
        if (mode == 4)
        {
            m[0] = -m[0];
            m[5] = -m[5];
            m[7] = -m[7];
            m[4] = -m[4];
            m[6] = -m[6];
            m[2] = -m[2];
            m[1] = -m[1];
            m[3] = -m[3];
            m[8] = -m[8];
            m[9] = -m[9];
            m[11] = -m[11];
            m[10] = -m[10];
            m[12] = -m[12];
            m[13] = -m[13];
            m[15] = -m[15];
            m[14] = -m[14];
        }
        else if (mode == 5)
        {
            m[4] = -m[4];
            m[6] = -m[6];
            m[5] = -m[5];
            m[7] = -m[7];
        }
        scaleProjectionRows_(updater, matrix);
        return;
    }

    if (mode == 1)
    {
        m[0] = -m[0];
        m[1] = -m[1];
        m[2] = -m[2];
        m[3] = -m[3];
        transformMatrixBlocks_(matrix);
        scaleProjectionRows_(updater, matrix);
        return;
    }

    if (mode == 2)
    {
        m[4] = -m[4];
        m[5] = -m[5];
        m[6] = -m[6];
        m[7] = -m[7];
        transformMatrixBlocks_(matrix);
        scaleProjectionRows_(updater, matrix);
        return;
    }

    scaleProjectionRows_(updater, matrix);
}

namespace {

void mtx44RowMul_(sead::Matrix44f* dst, const sead::Matrix44f* a, const sead::Matrix44f* b, u32 row_index)
{
    const f32* af = reinterpret_cast<const f32*>(a);
    const f32* bf = reinterpret_cast<const f32*>(b);
    f32* df = reinterpret_cast<f32*>(dst);
    u32 o = row_index * 4;

    df[o + 0] = af[0] * bf[o + 0] + af[1] * bf[o + 1] + af[2] * bf[o + 2] + af[3] * bf[o + 3];
    df[o + 1] = af[4] * bf[o + 0] + af[5] * bf[o + 1] + af[6] * bf[o + 2] + af[7] * bf[o + 3];
    df[o + 2] = af[8] * bf[o + 0] + af[9] * bf[o + 1] + af[10] * bf[o + 2] + af[11] * bf[o + 3];
    df[o + 3] = af[12] * bf[o + 0] + af[13] * bf[o + 1] + af[14] * bf[o + 2] + af[15] * bf[o + 3];
}

void mtx44CopyRow_(sead::Matrix44f* dst, const sead::Matrix44f* src, u32 row_index)
{
    const f32* sf = reinterpret_cast<const f32*>(src);
    f32* df = reinterpret_cast<f32*>(dst);
    u32 o = row_index * 4;

    df[o + 0] = sf[o + 0];
    df[o + 1] = sf[o + 1];
    df[o + 2] = sf[o + 2];
    df[o + 3] = sf[o + 3];
}

}
