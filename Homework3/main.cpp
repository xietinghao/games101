#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.f;
    rotation << cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0,
              0, 2.5, 0, 0,
              0, 0, 2.5, 0,
              0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    return translate * rotation * scale;
}

inline float angle2radian(float angle){return angle*MY_PI/180;}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    Eigen::Matrix4f proj = Eigen::Matrix4f::Identity();
    proj << zNear,0,0,0,
        0,zNear,0,0,
        0,0,zNear+zFar,-zNear*zFar,
        0,0,-1,0;
    Eigen::Matrix4f ortho;
    float eye_fovRadian=angle2radian(eye_fov);
    float top =  tan(eye_fovRadian/2)*zNear;
    float right = top * aspect_ratio;
    ortho << 1/right,0,0,0,
        0,1/top,0,0,
        0,0,2/(zNear-zFar),0,
        0,0,0,1;
    Eigen::Matrix4f overturn;
    overturn << 1,0,0,0,
        0,1,0,0,
        0,0,-1,0,
        0,0,0,1;
    proj = overturn * ortho * proj;
    return proj;

    // TODO: Use the same  projection matrix from the previous assignments

}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

Eigen::Vector3f computeLightIntensity(Eigen::Vector3f k, Eigen::Vector3f intensity, Eigen:: Vector3f distance, float costheta){
    Eigen::Vector3f vec{0,0,0};
    float R_2=distance.dot(distance);
    for(int i=0;i<3;i++){
        vec[i]=k[i]*intensity[i]/R_2*std::max(0.0f,costheta);
    }
    return vec;
}

// std::vector<Eigen:Vector3f> lightCompute(Eigen::Vector3f light_vec, Eigen::Vector3f view_vec){

// }

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        Texture tex = *payload.texture;
        return_color += tex.getColorBilinear(payload.tex_coords[0],payload.tex_coords[1]);
        // TODO: Get the texture value at the texture coordinates of the current fragment

    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        auto light_vec = light.position-point;
        
        Eigen::Vector3f unit{1.,1.,1.};
        auto ambient = computeLightIntensity(ka, amb_light_intensity, unit.normalized(), 1.0f);
        
        auto light_norm = light_vec.normalized();
        float costheta = light_norm.dot(normal);
        auto diffuse=computeLightIntensity(kd, light.intensity, light_vec, costheta);
        
        auto eye_vec=eye_pos-point;
        auto half = light_vec + eye_vec;
        auto half_norm = half.normalized();
        costheta = normal.dot(half_norm);
        auto specular=computeLightIntensity(ks, light.intensity, light_vec, std::pow(costheta,p));//镜面反射需要用参数p限定光照强度，否则会覆盖漫发射与环境光

        result_color += ambient + specular + diffuse;
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

    }

    return result_color * 255.f;
}




Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        auto light_vec = light.position-point;
        
        Eigen::Vector3f unit{1.,1.,1.};
        auto ambient = computeLightIntensity(ka, amb_light_intensity, unit.normalized(), 1.0f);
        
        auto light_norm = light_vec.normalized();
        float costheta = light_norm.dot(normal);
        auto diffuse=computeLightIntensity(kd, light.intensity, light_vec, costheta);
        
        auto eye_vec=eye_pos-point;
        auto half = light_vec + eye_vec;
        auto half_norm = half.normalized();
        costheta = normal.dot(half_norm);
        auto specular=computeLightIntensity(ks, light.intensity, light_vec, std::pow(costheta,p));//镜面反射需要用参数p限定光照强度，否则会覆盖漫发射与环境光
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        result_color += ambient + diffuse + specular ;
    }

    return result_color * 255.f;
}



Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    Texture tex=*payload.texture;
    float x=normal.x(),y=normal.y(),z=normal.z();
    Eigen::Vector3f t{ x*y/std::sqrt(x*x+z*z), std::sqrt(x*x+z*z), y*z/(std::sqrt(x*x+z*z)) };
    Eigen::Vector3f b = normal.cross(t);
    Eigen::Matrix3f TBN;
    TBN << t, b, normal;
    Eigen::Vector2f tex_coords=payload.tex_coords;
    float dU = kh * kn * ( tex.getColor(tex_coords.x()+1.f/tex.width, tex_coords.y()).norm() - tex.getColor(tex_coords.x(),tex_coords.y()).norm() );
    float dV = kh * kn * ( tex.getColor(tex_coords.x(), tex_coords.y()+1.f/tex.height).norm() - tex.getColor(tex_coords.x(),tex_coords.y()).norm() );
    Eigen::Vector3f ln;
    ln<<-dU, -dV, 1.f;
    Eigen::Vector3f n = (TBN*ln).normalized();
    point += kn*normal*tex.getColor(tex_coords.x(),tex_coords.y()).norm();
    normal=n.normalized();
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)


    Eigen::Vector3f result_color = {0, 0, 0};
    //result_color=n;
    for (auto& light : lights)
    {
        auto light_vec = light.position-point;
        auto light_norm=light_vec.normalized();
        float costheta=1;
        Eigen::Vector3f ambient=computeLightIntensity(ka, amb_light_intensity, light_norm, costheta);

        costheta = light_norm.dot(normal);
        auto diffuse=computeLightIntensity(kd, light.intensity, light_vec, costheta);

        auto view_vec=eye_pos-point;
        auto half_norm=(light_norm+view_vec.normalized()).normalized();
        costheta = half_norm.dot(normal);
        auto specular=computeLightIntensity(ks,light.intensity,light_vec,std::pow(costheta,p));
        if(costheta<0)specular<<0.f,0.f,0.f;
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        result_color+=ambient+diffuse+specular;
    }

    return result_color * 255.f;
}

Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;


    float kh = 0.2, kn = 0.1;

    Texture tex=*payload.texture;
    float x=normal.x(),y=normal.y(),z=normal.z();
    Eigen::Vector3f t{ x*y/std::sqrt(x*x+z*z), std::sqrt(x*x+z*z), y*z/(std::sqrt(x*x+z*z)) };
    Eigen::Vector3f b = normal.cross(t);
    Eigen::Matrix3f TBN;
    TBN << t, b, normal;
    Eigen::Vector2f tex_coords=payload.tex_coords;
    float dU = kh * kn * ( (tex.getColor(tex_coords.x()+1.f/tex.width, tex_coords.y())).norm() - (tex.getColor(tex_coords.x(),tex_coords.y())).norm() );
    float dV = kh * kn * ( (tex.getColor(tex_coords.x(), tex_coords.y()+1.f/tex.height)).norm() - (tex.getColor(tex_coords.x(),tex_coords.y())).norm() );
    Eigen::Vector3f ln;
    ln<<-dU, -dV, 1.f;
    Eigen::Vector3f n = (TBN*ln).normalized();
    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)


    Eigen::Vector3f result_color = {0, 0, 0};
    result_color=n;

    return result_color * 255.f;
}

int main(int argc, const char** argv)
{
    std::vector<Triangle*> TriangleList;

    float angle = 140.0;
    bool command_line = false;

    std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "../models/spot/";

    // Load .obj File
    bool loadout = Loader.LoadFile("../models/spot/spot_triangulated_good.obj");
    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            Triangle* t = new Triangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700);

    auto texture_path = "hmap.jpg";
    r.set_texture(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

    if (argc >= 2)
    {
        command_line = true;
        filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path = "spot_texture.png";
            r.set_texture(Texture(obj_path + texture_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,10};

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a' )
        {
            angle -= 0.1;
        }
        else if (key == 'd')
        {
            angle += 0.1;
        }

    }
    return 0;
}
