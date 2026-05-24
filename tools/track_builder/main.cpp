// tools/track_builder/main.cpp
// Generates assets/track/spa.spa from hardcoded reference data.
// Run once: ./track_builder spa.spa
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <cstdio>
#include <cmath>

struct BinaryPoint {
    float x, y, z;
    float tx, ty, tz; // tangent
    float nx, ny, nz; // normal
    float widthL, widthR;
    float banking;
    int   surface;    // 0=asphalt 1=kerb 2=gravel 3=grass
    float grip;
};

// 30 key waypoints around Spa (local coords, meters, approximate)
static const float SPA_WP[][4] = { // x, z, y(elev), width
    {  0,    0,   0,  14}, // La Source hairpin exit
    { 50,  -80,  -5,  14}, // Eau Rouge approach
    { 60, -120, -20,  12}, // Eau Rouge valley bottom
    { 55, -140, -25,  12}, // Raidillon base
    { 30, -180, -10,  12}, // Raidillon mid
    {  0, -220,   2,  13}, // Raidillon top
    {-80, -260,   8,  14}, // Kemmel straight start
    {-200,-280,   6,  14}, // Kemmel mid
    {-320,-270,   4,  14}, // Les Combes approach
    {-380,-240,   2,  12}, // Les Combes 1
    {-400,-200,   0,  12}, // Les Combes 2
    {-380,-160,  -2,  12}, // Malmedy
    {-320,-120,  -5,  13}, // Rivage hairpin approach
    {-280, -80,  -8,  12}, // Rivage apex
    {-240, -40, -12,  13}, // Rivage exit
    {-180,  20, -15,  12}, // Pouhon approach
    {-120,  60, -18,  13}, // Pouhon apex
    { -60,  80, -15,  13}, // Pouhon exit
    {  20, 100, -10,  13}, // Fagnes
    {  80, 120,  -5,  12}, // Stavelot
    { 120, 150,  -2,  14}, // Blanchimont approach
    { 100, 200,   0,  14}, // Blanchimont
    {  60, 240,  -2,  14}, // Bus Stop approach
    {  20, 260,  -3,  12}, // Bus Stop chicane 1
    { -20, 270,  -3,  12}, // Bus Stop chicane 2
    { -40, 260,  -2,  13}, // Bus Stop exit
    { -20, 200,  -1,  14}, // Final sector
    {  0,  140,   0,  14}, // Back to finish
    {  0,   60,   0,  14}, // finish straight
    {  0,    0,   0,  14}, // finish line (close loop)
};

int main(int argc, char* argv[]) {
    const char* out = argc > 1 ? argv[1] : "spa.spa";
    std::vector<BinaryPoint> pts;
    int n = sizeof(SPA_WP)/sizeof(SPA_WP[0]);
    for (int i = 0; i < n; ++i) {
        BinaryPoint p{};
        p.x = SPA_WP[i][0]; p.z = SPA_WP[i][1]; p.y = SPA_WP[i][2];
        int next = (i+1)%n;
        glm::vec3 pos  = {p.x,p.y,p.z};
        glm::vec3 npos = {SPA_WP[next][0], SPA_WP[next][2], SPA_WP[next][1]};
        glm::vec3 t    = glm::normalize(npos-pos);
        p.tx=t.x; p.ty=t.y; p.tz=t.z;
        p.nx=0; p.ny=1; p.nz=0;
        p.widthL = p.widthR = SPA_WP[i][3]*0.5f;
        p.banking = 0.0f;
        p.surface = 0; p.grip = 1.0f;
        pts.push_back(p);
    }
    std::ofstream f(out, std::ios::binary);
    int cnt = (int)pts.size();
    f.write((char*)&cnt, sizeof(cnt));
    f.write((char*)pts.data(), pts.size()*sizeof(BinaryPoint));
    printf("Written %d points to %s\n", cnt, out);
}
