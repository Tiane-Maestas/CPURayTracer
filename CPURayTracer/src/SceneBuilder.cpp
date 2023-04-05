#include "../header/SceneBuilder.h"

bool CustomSceneBuilder::ReadCommandParameters(std::stringstream& cmdParams, const int numparams, float* parameters)
{
    for (int i = 0; i < numparams; i++) {
        cmdParams >> parameters[i];
        if (cmdParams.fail()) {
            std::cout << "Failed reading value: " << cmdParams.str() << "\n";
            return false;
        }
    }
    return true;
}

std::shared_ptr<Scene> CustomSceneBuilder::BuildFromFile(const char* filename)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    std::ifstream scenefile;
    scenefile.open(filename);
    if (scenefile.is_open()) {

        // Matrix Stack for object transforms.
        std::stack<mat4> transfstack;
        transfstack.push(mat4(1.0));  // identity

        // Current Scene Object Properties.
        float currAmbient[3] = { 0, 0, 0 };
        float currDiffuse[3] = { 0, 0, 0 };
        float currSpecular[3] = { 0, 0, 0 };
        float currEmission[3] = { 0, 0, 0 };
        float currShininess = 0;

        float attenuation[3] = { 1, 0, 0 }; // No attenuation by default.

        std::string line, cmd;
        std::getline(scenefile, line);
        while (scenefile) {
            // Exclude Comments and Blank Lines.
            if ((line.find_first_not_of(" \t\r\n") != std::string::npos) && (line[0] != '#'))
            {
                std::stringstream cmdParams(line);
                cmdParams >> cmd;
                float params[10]; // At most 10 parameters in a command.

                if (cmd == "size") // Set Image Size.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 2, params); // w, h 
                    if (validinput) 
                    {
                        m_options.ImageWidth = params[0];
                        m_options.ImageHeight = params[1];
                    }
                }
                else if (cmd == "maxdepth") // Set Ray Depth.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 1, params); // depth 
                    if (validinput) 
                    {
                        m_options.MaxRayDepth = (int)params[0];
                    }
                }
                else if (cmd == "output") // Set Output File Name.
                {
                    cmdParams >> m_options.OutputFileName;
                }
                else if (cmd == "camera") // Add Camera to Scene.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 10, params); // pos, eye, up, fov 
                    if (validinput) 
                    {
                        scene->camera = Camera(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
                    }
                }
                else if (cmd == "sph") // Add Sphere to Scene.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // pos, radius 
                    if (validinput) 
                    {
                        std::shared_ptr<Sphere> sph = std::make_unique<Sphere>(params[0], params[1], params[2], params[3]);
                        sph->material = Material();
                        for (int i = 0; i < 3; i++) 
                        {
                            sph->material.ambient[i] = currAmbient[i];
                            sph->material.diffuse[i] = currDiffuse[i];
                            sph->material.specular[i] = currSpecular[i];
                            sph->material.emission[i] = currEmission[i];
                        }
                        sph->material.shininess = currShininess;
                        sph->UpdateTransform(transfstack.top());

                        scene->meshes.push_back(sph);
                    }
                }
                else if (cmd == "elp") // Add Ellipsoid to Scene.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // pos, radius 
                    if (validinput) 
                    {
                        std::shared_ptr<Ellipsoid> elp = std::make_unique<Ellipsoid>(params[0], params[1], params[2], params[3]);
                        elp->material = Material();
                        for (int i = 0; i < 3; i++) 
                        {
                            elp->material.ambient[i] = currAmbient[i];
                            elp->material.diffuse[i] = currDiffuse[i];
                            elp->material.specular[i] = currSpecular[i];
                            elp->material.emission[i] = currEmission[i];
                        }
                        elp->material.shininess = currShininess;
                        elp->UpdateTransform(transfstack.top());

                        scene->meshes.push_back(elp);
                    }
                }
                else if (cmd == "vertex") // Store Vertex.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // pos 
                    if (validinput) 
                    {
                        m_vertices.push_back(std::pair<vec4,vec3>(vec4(params[0], params[1], params[2], 1), vec3(0.0)));
                    }
                }
                else if (cmd == "vertexnormal") // Store Vertex w/ Normal.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 6, params); // pos, normal 
                    if (validinput) 
                    {
                        vec4 vert(params[0], params[1], params[2], 1);
                        vec3 norm(params[3], params[4], params[5]);
                        m_vertices.push_back(std::pair<vec4, vec3>(vert, norm));
                    }
                }
                else if (cmd == "tri") // Store Triangle.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // v1, v2, v3 
                    if (validinput) 
                    {
                        Triangle tri = Triangle(m_vertices[(int)params[0]].first, m_vertices[(int)params[1]].first, m_vertices[(int)params[2]].first);
                        tri.setNormals(m_vertices[(int)params[0]].second, m_vertices[(int)params[1]].second, m_vertices[(int)params[2]].second);
                        m_currentTriangles.push_back(tri);
                    }
                }
                else if (cmd == "mesh") // Add mesh with current vector of triangles.
                {
                    if (m_currentTriangles.size() > 0) 
                    {
                        std::string name; cmdParams >> name;
                        std::shared_ptr<Mesh> mesh = std::make_unique<Mesh>(0, 0, 0, name);
                        mesh->triangles = std::vector<Triangle>(m_currentTriangles);
                        m_currentTriangles.clear(); // Clear triangles for next mesh.

                        mesh->material = Material();
                        for (int i = 0; i < 3; i++) 
                        {
                            mesh->material.ambient[i] = currAmbient[i];
                            mesh->material.diffuse[i] = currDiffuse[i];
                            mesh->material.specular[i] = currSpecular[i];
                            mesh->material.emission[i] = currEmission[i];
                        }
                        mesh->material.shininess = currShininess;

                        mesh->UpdateTransform(transfstack.top());
                        scene->meshes.push_back(mesh);
                    }
                }
                else if (cmd == "directional") // Add Lights to the Scene.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 6, params); // x, y, z, r, g, b
                    if (validinput) 
                    {
                        std::shared_ptr<Directional> light = std::make_unique<Directional>(params[0], params[1], params[2], params[3], params[4], params[5]);
                        light->UpdateTransform(transfstack.top());
                        scene->lights.push_back(light);
                    }
                }
                else if (cmd == "point") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 6, params); // x, y, z, r, g, b
                    if (validinput) 
                    {
                        std::shared_ptr<Point> light = std::make_unique<Point>(params[0], params[1], params[2], params[3], params[4], params[5]);
                        light->UpdateTransform(transfstack.top());
                        light->attenuation = vec3(attenuation[0], attenuation[1], attenuation[2]);
                        scene->lights.push_back(light);
                    }
                }
                else if (cmd == "attenuation") // Below are light and material properties.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // const, linear, quadratic 
                    if (validinput) 
                    {
                        attenuation[0] = params[0];
                        attenuation[1] = params[1];
                        attenuation[2] = params[2];
                    }
                }
                else if (cmd == "ambient") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b 
                    if (validinput) 
                    {
                        for (int i = 0; i < 3; i++) 
                        {
                            currAmbient[i] = params[i];
                        }
                    }
                }
                else if (cmd == "diffuse") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) 
                    {
                        for (int i = 0; i < 3; i++) 
                        {
                            currDiffuse[i] = params[i];
                        }
                    }
                }
                else if (cmd == "specular") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) 
                    {
                        for (int i = 0; i < 3; i++) 
                        {
                            currSpecular[i] = params[i];
                        }
                    }
                }
                else if (cmd == "emission") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) 
                    {
                        for (int i = 0; i < 3; i++) 
                        {
                            currEmission[i] = params[i];
                        }
                    }
                }
                else if (cmd == "shininess") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 1, params); // s
                    if (validinput) 
                    {
                        currShininess = params[0];
                    }
                }
                else if (cmd == "translate") // Below are Transformations.
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // x, y, z
                    if (validinput) 
                    {
                        mat4& T = transfstack.top();
                        T = translate(T, vec3(params[0], params[1], params[2]));
                    }
                }
                else if (cmd == "scale") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // x, y, z
                    if (validinput) 
                    {
                        mat4& T = transfstack.top();
                        T = scale(T, vec3(params[0], params[1], params[2]));
                    }
                }
                else if (cmd == "rotate") 
                {
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // x, y, z, angle
                    if (validinput) 
                    {
                        vec3 axis(params[0], params[1], params[2]);
                        mat4& T = transfstack.top();
                        T = rotate(T, glm::radians(params[3]), axis);
                    }
                }
                else if (cmd == "pushTransform") 
                {
                    transfstack.push(transfstack.top());
                }
                else if (cmd == "popTransform") 
                {
                    if (transfstack.size() <= 1) 
                    {
                        std::cerr << "Stack has no elements. Cannot Pop\n";
                    }
                    else 
                    {
                        transfstack.pop();
                    }
                }
                else 
                {
                    std::cerr << "Unknown Command: " << cmd << " -> Skipped \n";
                }
            }
            std::getline(scenefile, line);
        }
    }
    else 
    {
        std::cerr << "Unable to Open Input Scene File " << filename << std::endl;
        return nullptr;
    }
    return scene;
}