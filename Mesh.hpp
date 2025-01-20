#ifndef Mesh_hpp
#define Mesh_hpp

#if defined (__APPLE__)
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include "glm/glm.hpp"

#include "Shader.hpp"


#include <string>
#include <vector>


namespace gps {

    struct Vertex {

        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {

        GLuint id;
        //ambientTexture, diffuseTexture, specularTexture
        std::string type;
        std::string path;
    };

    struct Material {

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct Buffers {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    };

    class Mesh {

    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

	    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	    Buffers getBuffers();

	    void Draw(gps::Shader shader);

        ////raycasting
        //void BuildBVH();
        //bool Raycast(const Ray& ray, float& t) const;
        //float SurfaceArea(const BoundingBox& bbox) const;
        //float SAH(const BoundingBox& bbox, int numTriangles) const;

    private:
        /*  Render data  */
        Buffers buffers;

	    // Initializes all the buffer objects/arrays
	    void setupMesh();


        ////raycasting
        //std::vector<BVHNode> bvhNodes;

        //bool IntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) const;
        //void BuildBVHRecursive(int start, int end, int& currentIndex,int depth);

    };

}
#endif /* Mesh_hpp */
