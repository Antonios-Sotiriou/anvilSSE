#include "../headers/quaternions.h"

/* Quaternions library for C. */
/* Quaternion definition: q = w + xi + yj + zk */
/* Quaternion type: i = j = k = sqrt(-1) */

Quat unitQuat(void) {
    Quat q = {
        .w = 1.0,
        .v = { 0.0, 0.0, 0.0 }
    };
    return q;
}

Quat setQuat(const float w, const vec4f vec) {
    Quat q = {
        .w = w,
        .v = vec
    };
    return q;
}

void normalizeQuat(Quat *q) {
    vec4f cache = q->v * q->v;
    if (((q->w * q->w) + cache[0] + cache[1] + cache[2]) > 1.000010) {
        float magnitude = magnitudeQuat(*q);
        q->w /= magnitude;
        q->v /= magnitude;
    }
}

Quat conjugateQuat(const Quat q) {
    Quat res = {
        .w = q.w,
        .v = -q.v
    };
    return res;
}

float magnitudeQuat(const Quat q) {
    vec4f cache = q.v * q.v;
    return sqrtf((q.w * q.w) + cache[0] + cache[1] + cache[2]);
}

Quat rotationQuat(const float angle, const vec4f axis) {
    float radius = angle * (3.14159 / 180.0);
    float c = sinf(radius / 2.00);
    Quat res = {
        .w = cosf(radius / 2.00),
        .v = axis * c
    };
    return res;
}

Quat eulertoQuat(const float roll, const float yaw, const float pitch) {
    float cr, sr, cy, sy, cp, sp, cpcy, spsy;
    cr = cosf(roll / 2.00);
    sr = sinf(roll / 2.00);
    cy = cosf(yaw / 2.00);
    sy = sinf(yaw / 2.00);
    cp = cosf(pitch / 2.00);
    sp = sinf(pitch / 2.00);
    cpcy = cp * cy;
    spsy = sp * sy;

    Quat res = {
        .w = cr * cpcy + sr * spsy,
        .v = {
            sr * cpcy - cr * spsy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            0.0
        }
    };
    return res;
}

Quat multiplyQuats(const Quat q1, const Quat q2) {
    vec4f r = q1.v * q2.v;
    Quat res = {
        .w = (q1.w * q2.w) - r[0] - r[1] - r[2],
        .v = {
            (q1.w * q2.v[0])  +  (q1.v[0] * q2.w)    + (q1.v[1] * q2.v[2])  -  (q1.v[2] * q2.v[1]),
            (q1.w * q2.v[1])  -  (q1.v[0] * q2.v[2]) + (q1.v[1] * q2.w)    +  (q1.v[2] * q2.v[0]),
            (q1.w * q2.v[2])  +  (q1.v[0] * q2.v[1]) - (q1.v[1] * q2.v[0])  +  (q1.v[2] * q2.w),
            0.0
        }
    };
    return res;
}

Quat addQuats(const Quat q1, const Quat q2) {
    Quat res = {
        .w = q1.w + q2.w,
        .v = q1.v + q2.v
    };
    return res;
}

Mat4x4 MatfromQuat(const Quat q, const vec4f v) {
    Mat4x4 m = { 0 };

    m.m[0][0] = 2.0 * ( (q.w * q.w) + (q.v[0] * q.v[0]) ) - 1.0;
    m.m[0][1] = 2.0 * ( (q.v[0] * q.v[1]) - (q.w * q.v[2]) );
    m.m[0][2] = 2.0 * ( (q.v[0] * q.v[2]) + (q.w * q.v[1]) );
    m.m[0][3] = 0.0;

    m.m[1][0] = 2.0 * ( (q.v[0] * q.v[1]) + (q.w * q.v[2]) );
    m.m[1][1] = 2.0 * ( (q.w * q.w) + (q.v[1] * q.v[1]) ) - 1.0;
    m.m[1][2] = 2.0 * ( (q.v[1] * q.v[2]) - (q.w * q.v[0]) );
    m.m[1][3] = 0.0;

    m.m[2][0] = 2.0 * ( (q.v[0] * q.v[2]) - (q.w * q.v[1]) );
    m.m[2][1] = 2.0 * ( (q.v[1] * q.v[2]) + (q.w * q.v[0]) );
    m.m[2][2] = 2.0 * ( (q.w * q.w) + (q.v[2] * q.v[2]) ) - 1.0;
    m.m[2][3] = 0.0;

    if (m.m[2][0] != 0.0)
        m.m[3][0] = v[0] - v[0] * m.m[0][0] - v[1] * m.m[1][0] - v[2] * m.m[2][0];
    if (m.m[2][1] != 0.0)
        m.m[3][1] = v[1] - v[0] * m.m[0][1] - v[1] * m.m[1][1] - v[2] * m.m[2][1];
    if (m.m[2][2] != 0.0)
        m.m[3][2] = v[2] - v[0] * m.m[0][2] - v[1] * m.m[1][2] - v[2] * m.m[2][2];
    m.m[3][3] = 1.0;

    return m;
}


