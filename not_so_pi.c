#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 60
#define SCREEN_HEIGHT 40

// ------------------ π的几何参数 ------------------
// 顶部横梁
static const float BAR_LENGTH    = 15.0f;   // 顶部横梁长度
static const float BAR_RADIUS    = 1.5f;  // 顶部横梁半径
static const float BAR_CENTER_Y  = 0.5f;   // 横梁中心在 y 轴的位置

// 左腿
static const float LEFT_LEG_LENGTH = 6.0f;   // 左腿长度
static const float LEFT_LEG_RADIUS = 1.5f;  // 左腿半径

// 右腿(分两段)
static const float RIGHT_LEG_SHORT = 3.6f;   // 右腿竖直部分长度(比左腿短)
static const float LEG_RADIUS      = 1.5f;  // 右腿半径（与左腿相同）

static const float LEG_OFFSET_X    = 4.0f;   // 左右腿在 x 方向的偏移（±）

// 右腿末端的四分之一弯管
static const float ARC_RADIUS = 2.0f;  // 弯管主弧线半径

// ------------------ 投影/旋转参数 ------------------
static const float K1 = 30.0f;     // 控制投影缩放
static const float K2 = 15.0f;     // 控制 z 到屏幕的距离 (越大越“远”)
static const float STEP_U = 0.0314f; // 圆柱“圆周”方向采样步长
static const float STEP_V = 0.0314f; // 圆柱“轴向”方向采样步长

// 四分之一弯管的采样步长
static const float STEP_ARC_ALPHA = 0.0314f; // 弧线方向
static const float STEP_ARC_V     = 0.0314f; // 截面圆方向

// 旋转速度
static const float ROTATION_SPEED_A = 0.1f; // 绕 Y 轴旋转
static const float ROTATION_SPEED_B = 0.002f; // 绕 X 轴旋转

// 光照和字符映射表
static const char *SHADING_CHARS = ".,-~:;=!*#$@";



// ------------------ 工具函数 ------------------
// 与“光源方向”做点积，这里假设光源固定
float dot_light(float nx, float ny, float nz) {
    // 可以修改光源方向，如 (0,1,-1)/sqrt(2) 等
    float lx = 0.0f, ly = 1.0f, lz = -1.0f;
    float lenL = sqrtf(lx*lx + ly*ly + lz*lz);
    lx /= lenL; ly /= lenL; lz /= lenL;

    float d = nx*lx + ny*ly + nz*lz;
    return d < 0 ? 0 : d;
}

// 先绕 X 轴旋转 B，再绕 Y 轴旋转 A
void rotateXYZ(float x, float y, float z, float A, float B,
               float *xp, float *yp, float *zp)
{
    // 绕 X 轴旋转 B
    float x1 = x;
    float y1 = y*cosf(B) - z*sinf(B);
    float z1 = y*sinf(B) + z*cosf(B);

    // 绕 Y 轴旋转 A
    float x2 = x1*cosf(A) + z1*sinf(A);
    float y2 = y1;
    float z2 = -x1*sinf(A) + z1*cosf(A);

    *xp = x2;
    *yp = y2;
    *zp = z2;
}

// ------------------ 主函数 ------------------
int main(void) {
    float A = 0.0f; // 绕 Y 轴旋转角
    float B = 0.0f; // 绕 X 轴旋转角

    // 准备屏幕缓冲
    char output[SCREEN_WIDTH * SCREEN_HEIGHT];
    float zbuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

    while (1) {
        // 初始化输出和 z-buffer
        memset(output, ' ', SCREEN_WIDTH * SCREEN_HEIGHT);
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            zbuffer[i] = 1e9;
        }

        // ========== 1. 顶梁（水平圆柱，轴线沿 X 轴） ==========
        for (float u = 0; u < 2.0f * M_PI; u += STEP_U) {
            for (float v = 0; v < 1.0f; v += STEP_V) {
                // 圆柱表面参数：
                //  X 轴向：x = -BAR_LENGTH/2 + v*BAR_LENGTH
                //  截面圆：y = BAR_RADIUS*cos(u) + BAR_CENTER_Y
                //          z = BAR_RADIUS*sin(u)
                float x = -BAR_LENGTH/2.0f + v*BAR_LENGTH;
                float y = BAR_RADIUS*cosf(u) + BAR_CENTER_Y;
                float z = BAR_RADIUS*sinf(u);

                // 法向量(在未旋转前)
                float nx = 0.0f;
                float ny = cosf(u);
                float nz = sinf(u);

                // 旋转
                float xp, yp, zp;
                rotateXYZ(x, y, z, A, B, &xp, &yp, &zp);

                float nxp, nyp, nzp;
                rotateXYZ(nx, ny, nz, A, B, &nxp, &nyp, &nzp);

                // 投影
                float invZ = 1.0f / (zp + K2);
                int px = (int)(SCREEN_WIDTH / 2 + K1 * xp * invZ);
                int py = (int)(SCREEN_HEIGHT / 2 - K1 * yp * invZ);

                // 亮度
                float lum = dot_light(nxp, nyp, nzp);
                int idx = (int)(lum * (float)(strlen(SHADING_CHARS) - 1));

                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    float zval = zp + K2;
                    if (zval < zbuffer[py * SCREEN_WIDTH + px]) {
                        zbuffer[py * SCREEN_WIDTH + px] = zval;
                        output[py * SCREEN_WIDTH + px] = SHADING_CHARS[idx];
                    }
                }
            }
        }

        // ========== 2. 左腿（垂直圆柱，轴线沿 Y 轴） ==========
        // 从 y = BAR_CENTER_Y 到 y = BAR_CENTER_Y - LEFT_LEG_LENGTH
        for (float u = 0; u < 2.0f * M_PI; u += STEP_U) {
            for (float v = 0; v < 1.0f; v += STEP_V) {
                float x = LEFT_LEG_RADIUS*cosf(u) - LEG_OFFSET_X;  // 左腿在 x = -LEG_OFFSET_X
                float y = BAR_CENTER_Y - v*LEFT_LEG_LENGTH;        // 沿 Y 轴
                float z = LEFT_LEG_RADIUS*sinf(u);

                // 法向量(未旋转前)
                float nx = cosf(u);
                float ny = 0.0f;
                float nz = sinf(u);

                // 旋转
                float xp, yp, zp;
                rotateXYZ(x, y, z, A, B, &xp, &yp, &zp);

                float nxp, nyp, nzp;
                rotateXYZ(nx, ny, nz, A, B, &nxp, &nyp, &nzp);

                // 投影
                float invZ = 1.0f / (zp + K2);
                int px = (int)(SCREEN_WIDTH / 2 + K1 * xp * invZ);
                int py = (int)(SCREEN_HEIGHT / 2 - K1 * yp * invZ);

                float lum = dot_light(nxp, nyp, nzp);
                int idx = (int)(lum * (float)(strlen(SHADING_CHARS) - 1));

                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    float zval = zp + K2;
                    if (zval < zbuffer[py * SCREEN_WIDTH + px]) {
                        zbuffer[py * SCREEN_WIDTH + px] = zval;
                        output[py * SCREEN_WIDTH + px] = SHADING_CHARS[idx];
                    }
                }
            }
        }

        // ========== 3. 右腿(短的垂直圆柱) ==========
        // 从 y = BAR_CENTER_Y 到 y = BAR_CENTER_Y - RIGHT_LEG_SHORT
        float rightLegBottomY = BAR_CENTER_Y - RIGHT_LEG_SHORT; // 记录下底部 y
        for (float u = 0; u < 2.0f * M_PI; u += STEP_U) {
            for (float v = 0; v < 1.0f; v += STEP_V) {
                float x = LEG_RADIUS*cosf(u) + LEG_OFFSET_X;   
                float y = BAR_CENTER_Y - v*RIGHT_LEG_SHORT;   
                float z = LEG_RADIUS*sinf(u);

                // 法向量
                float nx = cosf(u);
                float ny = 0.0f;
                float nz = sinf(u);

                // 旋转
                float xp, yp, zp;
                rotateXYZ(x, y, z, A, B, &xp, &yp, &zp);

                float nxp, nyp, nzp;
                rotateXYZ(nx, ny, nz, A, B, &nxp, &nyp, &nzp);

                // 投影
                float invZ = 1.0f / (zp + K2);
                int px = (int)(SCREEN_WIDTH / 2 + K1 * xp * invZ);
                int py = (int)(SCREEN_HEIGHT / 2 - K1 * yp * invZ);

                float lum = dot_light(nxp, nyp, nzp);
                int idx = (int)(lum * (float)(strlen(SHADING_CHARS) - 1));

                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    float zval = zp + K2;
                    if (zval < zbuffer[py * SCREEN_WIDTH + px]) {
                        zbuffer[py * SCREEN_WIDTH + px] = zval;
                        output[py * SCREEN_WIDTH + px] = SHADING_CHARS[idx];
                    }
                }
            }
        }


        // ========== 4. 右腿末端弯管(1/4甜甜圈) ==========
        // 让弧管在 X-Y 平面上生成，alpha ∈ [0, π/2] 控制弧度
        for (float alpha = 0; alpha <= M_PI/2; alpha += STEP_ARC_ALPHA) {
            // 计算弧管中心：
            // 当 alpha=0 时，arcCenterX = LEG_OFFSET_X, arcCenterY = rightLegBottomY, arcCenterZ = 0.0
            // 当 alpha=π/2 时，arcCenterX = LEG_OFFSET_X + ARC_RADIUS, arcCenterY = rightLegBottomY - ARC_RADIUS
            
            float arcCenterX = LEG_OFFSET_X + ARC_RADIUS * (1.0f - cosf(alpha));
            float arcCenterY = rightLegBottomY - ARC_RADIUS * sinf(alpha);
            float arcCenterZ = 0.0f;
        
            // 截面圆的生成
            for (float v = 0; v < 2.0f * M_PI; v += STEP_ARC_V) {
                float nx = cosf(v);  // 截面法向量 x 分量
                float ny = 0.0f;
                float nz = sinf(v);  // 截面法向量 z 分量
        
                float x = arcCenterX + LEG_RADIUS * nx;
                float y = arcCenterY; 
                float z = arcCenterZ + LEG_RADIUS * nz;

                float xp, yp, zp;
                rotateXYZ(x, y, z, A, B, &xp, &yp, &zp);

                float nxp, nyp, nzp;
                rotateXYZ(nx, ny, nz, A, B, &nxp, &nyp, &nzp);

                float invZ = 1.0f / (zp + K2);
                int px = (int)(SCREEN_WIDTH / 2 + K1 * xp * invZ);
                int py = (int)(SCREEN_HEIGHT / 2 - K1 * yp * invZ);

                float lum = dot_light(nxp, nyp, nzp);
                int idx = (int)(lum * (float)(strlen(SHADING_CHARS) - 1));

                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    float zval = zp + K2;
                    if (zval < zbuffer[py * SCREEN_WIDTH + px]) {
                        zbuffer[py * SCREEN_WIDTH + px] = zval;
                        output[py * SCREEN_WIDTH + px] = SHADING_CHARS[idx];
                    }
                }
            }
        }

        // ========== 输出到终端 ==========
        system("cls");
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                putchar(output[j * SCREEN_WIDTH + i]);
            }
            putchar('\n');
        }

        // 增加旋转角度
        A += ROTATION_SPEED_A;
        B += ROTATION_SPEED_B;

        // 控制帧率
        //usleep(30000);
    }

    return 0;
}
