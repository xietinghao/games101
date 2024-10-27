//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}


// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    
    Intersection p = Scene::intersect(ray);
    Vector3f light_dir, light_indir;

    if(!p.happened)
        return light_dir;
    
    Material *m = p.m;
    Object *hitObject = p.obj;
    Vector3f N=p.normal.normalized();

    if(p.m->hasEmission())
        return p.m->getEmission(); 
    
    Intersection x;
    float pdf_light;
    sampleLight(x, pdf_light);
    Vector3f ws = (x.coords-p.coords).normalized();
    Vector3f wo = ray.direction;
    Intersection middle = intersect(Ray(p.coords, ws));
    
    if(middle.happened && (middle.coords-x.coords).norm()>-0.01){
        light_dir = x.emit * m->eval(wo, ws, N) * dotProduct(ws, N) * dotProduct(-ws, x.normal.normalized()) / std::pow((x.coords-p.coords).norm(),2) / pdf_light;
    }

    float p_rr=get_random_float();
    if(p_rr<RussianRoulette){
        Vector3f wi = m->sample(wo, N).normalized();
        Intersection q = intersect(Ray(p.coords, wi));
        if(q.happened && !q.m->hasEmission()){
            float pdf = m->pdf(wo, wi, N);
            light_indir = castRay(Ray(p.coords, wi), depth+1) * m->eval(wo, wi, N) * dotProduct(wi, N) / RussianRoulette / pdf;
        }
    }
    return light_dir+light_indir;
}
