#include <fstream>
#include <sstream>
#include "model.h"
#include "../core/gameobject.h"

void Model::Update()
{
    if (autoRotation)
    {
        gameObject->transform->rotation.y += 1;
        if (gameObject->transform->rotation.y > 360)
            gameObject->transform->rotation.y -= 360;
    }
}

void Model::set_file_name(const char* filename)
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vector3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            Vector3f n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms_.push_back(n);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            Vector2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uv_.push_back(uv);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<Vector3i> f;
            Vector3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
            {
                for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    //std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
    //世界空间法线
    // load_texture(filename, "_nm.tga", normalmap_);
    //切线空间法线
    load_texture(filename, "_nm_tangent.tga", normalmap_);
    load_texture(filename, "_spec.tga", specularmap_);
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

Vector3f Model::vert(int i)
{
    return verts_[i];
}

Vector3f Model::vert(int iface, int nthvert)
{
    return verts_[faces_[iface][nthvert][0]];
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        //std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.read_tga_file(texfile.c_str());
        img.flip_vertically();
    }
}

TGAColor Model::diffuse(Vector2f uvf)
{
    Vector2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

Vector3f Model::normal(Vector2f uvf)
{
    Vector2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vector3f res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
    return res;
}

Vector2f Model::uv(int iface, int nthvert)
{
    return uv_[faces_[iface][nthvert][1]];
}

float Model::specular(Vector2f uvf)
{
    Vector2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

Vector3f Model::tangent(int x, int y)
{
    Vector2i uv(x * normalmap_.get_width(), y * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vector3f res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
    return res;
}

Vector3f Model::normal(int iface, int nthvert)
{
    int idx = faces_[iface][nthvert][2];
    return vector_normalize(norms_[idx]);
}

TGAImage* Model::get_diffuse_map()
{
    return &diffusemap_;
}

TGAImage* Model::get_normal_map()
{
    return &normalmap_;
}

TGAImage* Model::get_specular_map()
{
    return &specularmap_;
}
