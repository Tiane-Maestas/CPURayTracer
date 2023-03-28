#include "../header/SceneBuilder.h"

bool CustomSceneBuilder::ReadCommandParameters(stringstream& cmdParams, const int numparams, float* parameters)
{
    for (int i = 0; i < numparams; i++) {
        cmdParams >> parameters[i];
        if (cmdParams.fail()) {
            cout << "Failed reading value: " << cmdParams.str() << "\n";
            return false;
        }
    }
    return true;
}

Scene* CustomSceneBuilder::BuildFromFile(const char* filename) 
{
    Scene* scene = new Scene();

    ifstream scenefile;
    scenefile.open(filename);
    if (scenefile.is_open()) {

        // Matrix Stack for object transforms.
        stack<mat4> transfstack;
        transfstack.push(mat4(1.0));  // identity

        // Current Scene Object Properties.
        float currAmbient[3] = { 0, 0, 0 };
        float currDiffuse[3] = { 0, 0, 0 };
        float currSpecular[3] = { 0, 0, 0 };
        float currEmission[3] = { 0, 0, 0 };
        float currShininess = 0;

        float attenuation[3] = { 1, 0, 0 }; // No attenuation by default.

        string line, cmd;
        std::getline(scenefile, line);
        while (scenefile) {
            // Exclude Comments and Blank Lines.
            if ((line.find_first_not_of(" \t\r\n") != string::npos) && (line[0] != '#')) {
                stringstream cmdParams(line);
                cmdParams >> cmd;
                float params[10]; // At most 10 parameters in a command.

                if (cmd == "size") { // Set Image Size.
                    bool validinput = ReadCommandParameters(cmdParams, 2, params); // w, h 
                    if (validinput) {
                        m_options.ImageWidth = params[0];
                        m_options.ImageHeight = params[1];
                    }
                }
                else if (cmd == "maxdepth") { // Set Ray Depth.
                    bool validinput = ReadCommandParameters(cmdParams, 1, params); // depth 
                    if (validinput) {
                        m_options.MaxRayDepth = (int)params[0];
                    }
                }
                else if (cmd == "output") { // Set Output File Name.
                    cmdParams >> m_options.OutputFileName;
                }
                else if (cmd == "camera") { // Add Camera to Scene.
                    bool validinput = ReadCommandParameters(cmdParams, 10, params); // pos, eye, up, fov 
                    if (validinput) {
                        Camera cam(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
                        scene->addCamera(cam);
                    }
                }
                else if (cmd == "sphere") { // Add Sphere to Scene.
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // pos, radius 
                    if (validinput) {
                        shared_ptr<Sphere> sph = make_unique<Sphere>(params[0], params[1], params[2], params[3]);
                        Material mat;
                        for (int i = 0; i < 3; i++) {
                            mat.ambient[i] = currAmbient[i];
                            mat.diffuse[i] = currDiffuse[i];
                            mat.specular[i] = currSpecular[i];
                            mat.emission[i] = currEmission[i];
                        }
                        mat.shininess = currShininess;
                        sph->setMaterial(mat);
                        sph->UpdateTransform(transfstack.top());

                        scene->addMesh(sph);
                    }
                }
                else if (cmd == "elp") { // Add Ellipsoid to Scene.
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // pos, radius 
                    if (validinput) {
                        shared_ptr<Ellipsoid> sph = make_unique<Ellipsoid>(params[0], params[1], params[2], params[3]);
                        Material mat;
                        for (int i = 0; i < 3; i++) {
                            mat.ambient[i] = currAmbient[i];
                            mat.diffuse[i] = currDiffuse[i];
                            mat.specular[i] = currSpecular[i];
                            mat.emission[i] = currEmission[i];
                        }
                        mat.shininess = currShininess;
                        sph->setMaterial(mat);
                        sph->UpdateTransform(transfstack.top());

                        scene->addMesh(sph);
                    }
                }
                else if (cmd == "vertex") { // Store Vertex.
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // pos 
                    if (validinput) {
                        m_vertices.push_back(pair<vec4,vec4>(vec4(params[0], params[1], params[2], 1), vec4(0.0)));
                    }
                }
                else if (cmd == "vertexnormal") { // Store Normal w/ Vertex.
                    bool validinput = ReadCommandParameters(cmdParams, 6, params); // pos, normal 
                    if (validinput) {
                        vec4 vert(params[0], params[1], params[2], 1);
                        vec4 norm(params[3], params[4], params[5], 1);
                        m_vertices.push_back(pair<vec4, vec4>(vert, norm));
                    }
                }
                else if (cmd == "tri") { // Store Triangle.
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // v1, v2, v3 
                    if (validinput) {
                        Triangle tri = Triangle(m_vertices[(int)params[0]].first, m_vertices[(int)params[1]].first, m_vertices[(int)params[2]].first);
                        tri.setNormals(m_vertices[(int)params[0]].second, m_vertices[(int)params[1]].second, m_vertices[(int)params[2]].second);
                        m_triangles.push_back(tri);
                    }
                }
                else if (cmd == "mesh") { // Add mesh with current vector of triangles. 
                    if (m_triangles.size() > 0) {
                        string name; cmdParams >> name;
                        shared_ptr<Mesh> mesh = make_unique<Mesh>(0, 0, 0, name);
                        mesh->addTriangles(m_triangles);
                        m_triangles.clear(); // Clear triangles for next mesh.
                        Material mat;
                        for (int i = 0; i < 3; i++) {
                            mat.ambient[i] = currAmbient[i];
                            mat.diffuse[i] = currDiffuse[i];
                            mat.specular[i] = currSpecular[i];
                            mat.emission[i] = currEmission[i];
                        }
                        mat.shininess = currShininess;
                        mesh->setMaterial(mat);
                        mesh->UpdateTransform(transfstack.top());
                        scene->addMesh(mesh);
                    }
                }
                else if (cmd == "directional") { // Add Lights to the Scene.
                    bool validinput = ReadCommandParameters(cmdParams, 7, params); // x, y, z, r, g, b, a 
                    if (validinput) {
                        shared_ptr<Directional> light = make_unique<Directional>(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
                        light->UpdateTransform(transfstack.top());
                        scene->addLight(light);
                    }
                }
                else if (cmd == "point") {
                    bool validinput = ReadCommandParameters(cmdParams, 7, params); // x, y, z, r, g, b, a 
                    if (validinput) {
                        shared_ptr<Point> light = make_unique<Point>(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
                        light->UpdateTransform(transfstack.top());
                        light->setAttenuation(vec3(attenuation[0], attenuation[1], attenuation[2]));
                        scene->addLight(light);
                    }
                }
                else if (cmd == "attenuation") {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // const, linear, quadratic 
                    if (validinput) {
                        attenuation[0] = params[0];
                        attenuation[1] = params[1];
                        attenuation[2] = params[2];
                    }
                }
                else if (cmd == "ambient") { // Below are light and material properties.
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b 
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            currAmbient[i] = params[i];
                        }
                    }
                }
                else if (cmd == "diffuse") {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            currDiffuse[i] = params[i];
                        }
                    }
                }
                else if (cmd == "specular") {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            currSpecular[i] = params[i];
                        }
                    }
                }
                else if (cmd == "emission") {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // r, g, b
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            currEmission[i] = params[i];
                        }
                    }
                }
                else if (cmd == "shininess") {
                    bool validinput = ReadCommandParameters(cmdParams, 1, params); // s
                    if (validinput) {
                        currShininess = params[0];
                    }
                }
                else if (cmd == "translate") { // Below are Transformations.
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // x, y, z
                    if (validinput) {
                        mat4& T = transfstack.top();
                        T = translate(T, vec3(params[0], params[1], params[2]));
                    }
                }
                else if (cmd == "scale") {
                    bool validinput = ReadCommandParameters(cmdParams, 3, params); // x, y, z
                    if (validinput) {
                        mat4& T = transfstack.top();
                        T = scale(T, vec3(params[0], params[1], params[2]));
                    }
                }
                else if (cmd == "rotate") {
                    bool validinput = ReadCommandParameters(cmdParams, 4, params); // x, y, z, angle
                    if (validinput) {
                        vec3 axis(params[0], params[1], params[2]);
                        mat4& T = transfstack.top();
                        T = rotate(T, params[3], axis);
                    }
                }
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top());
                }
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements. Cannot Pop\n";
                    }
                    else {
                        transfstack.pop();
                    }
                }
                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n";
                }
            }
            std::getline(scenefile, line);
        }
    }
    else {
        cerr << "Unable to Open Input Scene File " << filename << endl;
        return nullptr;
    }
    return scene;
}