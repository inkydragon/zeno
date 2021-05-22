#include <zen/zen.h>
#include <zen/MeshObject.h>
#include <zen/PrimitiveObject.h>
#include <zen/NumericObject.h>
#include <zen/vec.h>
#include <cstring>
#include <cstdlib>
#include <cassert>

namespace zenbase {

struct MeshToPrimitive : zen::INode{
    virtual void apply() override {
    auto mesh = get_input("mesh")->as<MeshObject>();
    auto result = zen::IObject::make<PrimitiveObject>();
    result->add_attr<zen::vec3f>("pos");
    result->add_attr<zen::vec3f>("clr");
    result->add_attr<zen::vec3f>("nrm");
    result->resize(mesh->vertices.size());
    result->tris.resize(mesh->vertices.size()/3);
    result->quads.resize(0);

#pragma omp parallel for
    for(int i=0;i<mesh->vertices.size();i++)
    {
        result->attr<zen::vec3f>("pos")[i] = zen::vec3f(mesh->vertices[i].x,
            mesh->vertices[i].y, mesh->vertices[i].z);
        result->attr<zen::vec3f>("clr")[i] = zen::vec3f(mesh->uvs[i].x, mesh->uvs[i].y,
            0);
        result->attr<zen::vec3f>("nrm")[i] = zen::vec3f(mesh->normals[i].x, mesh->normals[i].y,mesh->normals[i].z);
    }
#pragma omp parallel for
    for(int i=0;i<mesh->vertices.size()/3;i++)
    {
        result->tris[i] = zen::vec3i(i*3, i*3+1, i*3+2);
    }
    
    set_output("prim", result);
  }
};

static int defMeshToPrimitive = zen::defNodeClass<MeshToPrimitive>("MeshToPrimitive",
    { /* inputs: */ {
        "mesh",
    }, /* outputs: */ {
        "prim",
    }, /* params: */ { 
    }, /* category: */ {
    "primitive",
    }});

}
