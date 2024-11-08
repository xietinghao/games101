//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include <thread>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.

std::vector<Vector3f> framebuffer;
int spp;
void renderSingle(const unsigned int pixelBegin, const unsigned int pixelEnd, const int spp, const Scene &scene){
    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    for (uint32_t i = pixelBegin; i < pixelEnd; i++) {
        int row = i / scene.width;
        int col = i - row * scene.width;
        float x = (2 * (col + 0.5) / (float)scene.width - 1) * imageAspectRatio * scale;
        float y = (1 - 2 * (row + 0.5) / (float)scene.height) * scale;

        Vector3f dir = normalize(Vector3f(-x, y, 1.0f));
        for (int j = 0; j < spp; j++) {
            framebuffer[i] += scene.castRay(Ray(eye_pos, dir), 0) / spp;
        }
    }
    std::cout << pixelBegin << "->" << pixelEnd << " processed" << std::endl;
}
void renderThreads(const Scene &scene, std::vector<Vector3f> &framebuffer){
    const int coreNums = 18;
    const int pixelNum = scene.height * scene.width;
    const int threadPixel = (pixelNum + coreNums) / coreNums;
    std::vector<std::thread> vecThread;
    unsigned int pixelBegin = 0, pixelEnd = std::min(threadPixel, pixelNum);
    
    const int spp = 16;
    for (unsigned int threadId = 0; threadId < coreNums;threadId++) {
        vecThread.push_back(std::thread(renderSingle, pixelBegin, pixelEnd, spp, std::cref(scene)));
        pixelBegin += threadPixel;
        pixelEnd = std::min(pixelBegin + threadPixel, static_cast<unsigned int>(pixelNum));
    }
    

    for (int i = 0; i < coreNums; i++)vecThread[i].join();
    
}

void Renderer::Render(const Scene& scene)
{
    framebuffer=std::vector<Vector3f>(scene.width * scene.height);
    spp = 16;
    /*float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    int m = 0;*/
    
    

    // change the spp value to change sample ammount
    std::cout << "SPP: " << spp << "\n";

    renderThreads(scene, framebuffer);

    
    // for (uint32_t j = 0; j < scene.height; ++j) {
    //     for (uint32_t i = 0; i < scene.width; ++i) {
    //         // generate primary ray direction
    //         float x = (2 * (i + 0.5) / (float)scene.width - 1) *
    //                   imageAspectRatio * scale;
    //         float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

    //         Vector3f dir = normalize(Vector3f(-x, y, 1));
    //         for (int k = 0; k < spp; k++){
    //             framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
    //         }
    //         m++;
    //     }
    //     UpdateProgress(j / (float)scene.height);
    // }
    // UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
