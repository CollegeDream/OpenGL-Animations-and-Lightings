/*
 * Based on work by Graham Sellers and OpenGL SuperBible7
 * Also: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
 *       http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/ 
 *       https://antongerdelan.net/opengl/cubemaps.html
 * 
 * Goal is to build on our skycube system to show off textures
 */
#include <maze.h>
#include <sb7.h>
#include <shader.h>
#include <vmath.h>

#include <loadingFunctions.h>
#include <skybox.h>

//Needed for file loading (also vector)
#include <string>
#include <fstream>

// For error checking
#include <vector>
#include <cassert>
#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

class test_app : public sb7::application{

    Maze maze;

    public:

    void init(){
        // Set up appropriate title
        static const char title[] = "Skybox with textures Example";
        sb7::application::init();
        memcpy(info.title, title, sizeof(title));

        info.windowWidth = 900; //Make sure things are square to start with
        info.windowHeight = 900;
    }
    
    void startup(){

        //Generate the maze and create cube objects for the maze walls
        std::vector<std::pair<int, int>> walls;
        std::pair<int, int> endFlag;
        maze = Maze(20, 20);
        for(int i = 0; i < maze.getHeight(); i++){
            for(int j = 0; j < maze.getWidth(); j++){
                if(maze.getTile(j, i) == wall){
                    walls.push_back(std::pair<int, int>(j, i));
                }
                if(maze.getTile(j, i) == end){
                    endFlag = std::pair<int, int>(j, i);
                }
            }
        }


        //////////////////////
        // Load Object Info //
        //////////////////////
        objects.push_back(obj_t()); //Push one new object into the vector list
        objects.push_back(obj_t()); //Push second new object into the vector list
        // This program is set up to load multiple *different* objects
        // If you wanted to decouple the data for objects from the transforms for object, it would be beneficial to 
        // have two cooperative structs. One would hold the vertex data, the other would reference that data with 
        // individual transform infomation.

        //Also notice this could be automated / streamlined with a list of objects to load

        //Load two objects
        load_obj(".\\bin\\media\\car23.obj", objects[0].verticies, objects[0].uv, objects[0].normals, objects[0].vertNum);

         //Create a wall object for each item in vector and set their position
         /*
        for(int i = 1; i < walls.size() + 1; i++){
            objects.push_back(objects[0]);
            objects[i].obj2world = vmath::mat4::identity() * vmath::translate((float)(walls[i].first* 2) + 2, 0.0f, (float)(walls[i].second * 2) + 2);
        }*/

        //Generate the outer walls of the maze
        //top wall
        /*
        for(int i = 0; i < maze.getWidth(); i++){
            obj_t temp = objects[0];
            temp.obj2world = vmath::mat4::identity() * vmath::translate((float)(i * 2), 0.0f, 0.0f);
            objects.push_back(temp);

            temp.obj2world = vmath::mat4::identity() * vmath::translate((float)(i * 2), 0.0f, (float)(maze.getHeight() * 2));
            objects.push_back(temp);
        }

        // right wall
        for(int i = 0; i < maze.getHeight(); i++){
            obj_t temp = objects[0];
            temp.obj2world = vmath::mat4::identity() * vmath::translate((float)(maze.getWidth() * 2), 0.0f, (float)(i * 2));
            objects.push_back(temp);

            //Left wall
            temp.obj2world = vmath::mat4::identity() * vmath::translate(0.0f, 0.0f, (float)(i * 2));
            objects.push_back(temp); 
        }*/


        ////////////////////////////////
        //Set up Object Scene Shaders //
        ////////////////////////////////
        GLuint shaders[2];

        //Load scene rendering based shaders
        //These need to be co-located with main.cpp in src
        shaders[0] = sb7::shader::load(".\\src\\vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(shaders[0]);
        shaders[1] = sb7::shader::load(".\\src\\fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(shaders[1]);
        //Put together scene rendering program from the two loaded shaders
        rendering_program = sb7::program::link_from_shaders(shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////////////////
        // Transfer Object Into OpenGL //
        /////////////////////////////////

        //Set up vao
        glUseProgram(rendering_program); //TODO:: This might not be necessary (because of the above link_from_shaders)
        glCreateVertexArrays(1,&vertex_array_object);
        glBindVertexArray(vertex_array_object);

        for(int i = 0; i < objects.size(); i++){
            //For each object in objects, set up openGL buffers
            glGenBuffers(1,&objects[i].vertices_buffer_ID); //Create the buffer id for this object
            glBindBuffer( GL_ARRAY_BUFFER, objects[i].vertices_buffer_ID);
            glBufferData( GL_ARRAY_BUFFER,
                objects[i].verticies.size() * sizeof(objects[i].verticies[0]), //Size of element * number of elements
                objects[i].verticies.data(),                                   //Actual data
                GL_STATIC_DRAW);                                               //Set to static draw (read only)  

            //Set up UV buffers
            glGenBuffers(1,&objects[i].uv_buffer_ID); //Create the buffer id for this object
            glBindBuffer( GL_ARRAY_BUFFER, objects[i].uv_buffer_ID);
            glBufferData( GL_ARRAY_BUFFER,
                objects[i].uv.size() * sizeof(objects[i].uv[0]), //Size of element * number of elements
                objects[i].uv.data(),                            //Actual data
                GL_STATIC_DRAW); 
        }
        
        GL_CHECK_ERRORS
        ////////////////////////////////////
        // Grab IDs for rendering program //
        ////////////////////////////////////
        transform_ID = glGetUniformLocation(rendering_program,"transform");
        perspec_ID = glGetUniformLocation(rendering_program,"perspective");
        toCam_ID = glGetUniformLocation(rendering_program,"toCamera");
        vertex_ID = glGetAttribLocation(rendering_program,"obj_vertex");
        uv_ID = glGetAttribLocation(rendering_program,"obj_uv");        

        ///////////////////////////
        // Set Up Simple Texture //
        ///////////////////////////
        /*
        #define B 0x00, 0x00, 0x00, 0x00 //Black
        #define W 0xFF, 0xFF, 0xFF, 0xFF //White
        static const GLubyte tex_data[] =
        {
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
        };//Giant grid of black and white alternating
        #undef B
        #undef W

        glGenTextures(1,&objects[0].texture_ID);
        glBindTexture(GL_TEXTURE_2D, objects[0].texture_ID);
        glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB8,16,16);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenTextures(1,&objects[1].texture_ID);
        glBindTexture(GL_TEXTURE_2D, objects[1].texture_ID);
        glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB8,16,16);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    */
        //////////////////////////////////////
        // Loading More Complicated Texture //
        //////////////////////////////////////
        
        //Memory spaces for texture data
        unsigned char* loadedTextureData;
        unsigned int texWidth;
        unsigned int texHeight;

        // Load texture data from bitmap file to CPU memory
        
        //load_BMP(".\\bin\\media\\strat.bmp",loadedTextureData,texWidth,texHeight);

        //Assign Texture from CPU memory to GPU memory
        /*
        for(int i = 0; i < objects.size(); i++){
            glGenTextures(1,&objects[i].texture_ID);
            glBindTexture(GL_TEXTURE_2D, objects[i].texture_ID);
            glTexImage2D( GL_TEXTURE_2D, //What kind of texture are we loading in
                                    0, // Level of detail, 0 base level
                                GL_RGBA, // Internal (target) format of data, in this case Red, Gree, Blue, Alpha
                            texWidth, // Width of texture data (max is 1024, but maybe more)
                            texHeight, // Height of texture data
                                    0, //border (must be zero)
                                GL_RGBA, //Format of input data (in this case we added the alpha when reading in data)
                    GL_UNSIGNED_BYTE, //Type of data being passed in
                        loadedTextureData); // Finally pointer to actual data to be passed in
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }*/
        //Get rid of dynamic memory after use
        //delete[] loadedTextureData;
        //GL_CHECK_ERRORS
        
        

        ///////////////////////////
        //Set up Skycube shaders //
        ///////////////////////////
        // Placeholders for loaded shaders
        GLuint sc_shaders[2];

        //Load Skycube based shaders
        //These need to be co-located with main.cpp in src
        sc_shaders[0] = sb7::shader::load(".\\src\\sc_vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(sc_shaders[0]);
        sc_shaders[1] = sb7::shader::load(".\\src\\sc_fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(sc_shaders[1]);

        //Put together Sky cube program from the two loaded shaders
        sc_program = sb7::program::link_from_shaders(sc_shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////
        //Load Skycube info//
        /////////////////////
        //skycube_vertices holds triangle form of a cube
        createCube(skycube_vertices);
        //Set up Vertex Array Object and associated Vertex Buffer Object
        GLuint vBufferObject; //We only need this to associate with sc_vertex_array_object
        glGenBuffers(1,&vBufferObject); //Create the buffer id
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glBufferData( GL_ARRAY_BUFFER,
                skycube_vertices.size() * sizeof(skycube_vertices[0]), //Size of element * number of elements
                skycube_vertices.data(),                               //Actual data
                GL_STATIC_DRAW);                                       //Set to static draw (read only)  
        
        glGenVertexArrays(1, &sc_vertex_array_object); // Get ID for skycube vao
        glBindVertexArray(sc_vertex_array_object);
        glEnableVertexAttribArray(0); //Enable Vertex Attribute Array
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, NULL); //Linking the buffer filled above to a vertex attribute
        GL_CHECK_ERRORS

        //Set up texture information
        glActiveTexture(GL_TEXTURE0);     //Set following data to GL_TEXTURE0
        glGenTextures(1,&sc_map_texture); //Grab texture ID
        //Call a file loading function to load in textures for skybox
        loadCubeTextures(".\\bin\\media\\Skycube\\",sc_map_texture);
        GL_CHECK_ERRORS

        //Get uniform handles for perspective and camera matrices
        sc_Perspective = glGetUniformLocation(sc_program,"perspective");
        sc_Camera= glGetUniformLocation(sc_program,"toCamera");
        GL_CHECK_ERRORS

        /////////////////////
        // Camera Creation //
        /////////////////////
        camera.camera_near = 0.1f; //Near Clipping Plane
        camera.camera_far = 100.0f; //Far Clipping Plane
        camera.fovy       = 67.0f; //Field of view in the y direction (x defined by aspect)
        //Initial camera details
        camera.position = vmath::vec3(2.0f, 0.0f, 2.0f); //Starting camera at position (0,0,5)
        camera.focus = vmath::vec3(0.0f, 0.0f, 0.0f); //Camera is looking at origin
        
        //Now that we have parameters set, calculate the Projection and View information for this camera
        calcProjection(camera); //Calculate the projection matrix used by this camera
        calcView(camera); //Calculate the View matrix for camera

        //Link locations to Uniforms
        glUseProgram(sc_program);
        glUniformMatrix4fv(sc_Perspective,1,GL_FALSE,camera.proj_Matrix);
        glUniformMatrix4fv(sc_Camera,1,GL_FALSE,camera.view_mat_no_translation);
        GL_CHECK_ERRORS

        // General openGL settings
        //src:: https://github.com/capnramses/antons_opengl_tutorials_book/tree/master/21_cube_mapping
        glEnable( GL_DEPTH_TEST );          // enable depth-testing
        glDepthFunc( GL_LESS );             // depth-testing interprets a smaller value as "closer"
        glEnable( GL_CULL_FACE );           // cull face
        glCullFace( GL_BACK );              // cull back face
        glFrontFace( GL_CCW );              // set counter-clock-wise vertex order to mean the front
        glClearColor( 0.2, 0.2, 0.2, 1.0 ); // grey background to help spot mistakes

        //End of set up check
        GL_CHECK_ERRORS
    }

    void shutdown(){
        //Clean up Buffers
        glDeleteVertexArrays(1, &sc_vertex_array_object);
        glDeleteTextures(1,&sc_map_texture);
        glDeleteProgram(sc_program);
    }

    void render(double curTime){

        glViewport( 0, 0, info.windowWidth, info.windowHeight ); //Set Viewport information

        //if Auto rotate flag is set, update the position of the camera
        if(autoRotate){
            camera.position = vmath::vec3(static_cast<float>(cos(curTime/10.0) * 5.0),
                                          0.0f,
                                          static_cast<float>(sin(curTime/10.0) * 5.0) );
        }

        //recalculate the View matrix for camera
        //calcProjection(camera);
        calcView(camera);

        //Clear output
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        runtime_error_check(1);

        //Draw the skyCube!
        drawSkyCube(curTime);

        runtime_error_check(2);

        /////////////////////////////////////////////////////////////////////////////////
        // This would be where you want to call another program to render your 'stuff' //
        // This could also go into a function for organizational ease                  //
        /////////////////////////////////////////////////////////////////////////////////

        //Set up obj->world transforms for each object (these could be modified for animation)
        //Cube
        //objects[0].obj2world = vmath::translate(1.5f, 0.2f, 1.5f) * vmath::scale(0.5f); // translate for object0
        objects[0].obj2world = vmath::mat4::identity() * vmath::translate(1.0f, -2.0f, 1.0f);

        for(int i = 0; i < objects.size(); i++ ){
            //render loop, go through each object and render it!
            glUseProgram(rendering_program); //activate the render program
            glBindVertexArray(vertex_array_object); //Select base vao

            //glBindTexture(GL_TEXTURE_2D, objects[i].texture_ID);

            //Copy over all the transforms
            glUniformMatrix4fv(transform_ID, 1,GL_FALSE, objects[i].obj2world); //Load in transform for this object
            //TODO::These might only need to be loaded once (for all objects)
            glUniformMatrix4fv(perspec_ID, 1,GL_FALSE, camera.proj_Matrix); //Load camera projection
            glUniformMatrix4fv(toCam_ID, 1,GL_FALSE, camera.view_mat); //Load in view matrix for camera

            //link to object buffer
            glEnableVertexAttribArray(vertex_ID); //Recall the vertex ID
            glBindBuffer(GL_ARRAY_BUFFER,objects[i].vertices_buffer_ID);//Link object buffer to vertex_ID
            glVertexAttribPointer( //Index into the buffer
                    vertex_ID, //Attribute in question
                    4,         //Number of elements per vertex call (vec4)
                    GL_FLOAT,  //Type of element
                    GL_FALSE,  //Normalize? Nope
                    0,         //No stride (steps between indexes)
                    0);       //initial offset

            /*
            glEnableVertexAttribArray(uv_ID); //Recall the vertex ID
            glBindBuffer(GL_ARRAY_BUFFER,objects[i].uv_buffer_ID);//Link object buffer to vertex_ID
            glVertexAttribPointer( //Index into the buffer
                    uv_ID, //Attribute in question
                    2,         //Number of elements per vertex call (vec2)
                    GL_FLOAT,  //Type of element
                    GL_FALSE,  //Normalize? Nope
                    0,         //No stride (steps between indexes)
                    0);       //initial offset
            */
            glDrawArrays( GL_TRIANGLES, 0, objects[i].verticies.size());
        }

        runtime_error_check(4);
    }

    void drawSkyCube(double curTime){

        glDepthMask( GL_FALSE ); //Used to force skybox 'into' the back, making sure everything is rendered over it
        glUseProgram( sc_program ); //Select the skycube program
        glUniformMatrix4fv( sc_Perspective, 1, GL_FALSE, camera.proj_Matrix); //Update the projection matrix (if needed)
        glUniformMatrix4fv( sc_Camera, 1, GL_FALSE, camera.view_mat_no_translation); //Update the projection matrix (if needed)
        glActiveTexture( GL_TEXTURE0 ); //Make sure we are using the CUBE_MAP texture we already set up
        glBindTexture( GL_TEXTURE_CUBE_MAP, sc_map_texture ); //Link to the texture
        glBindVertexArray( sc_vertex_array_object ); // Set up the vertex array
        glDrawArrays( GL_TRIANGLES, 0, skycube_vertices.size() ); //Start drawing triangles
        glDepthMask( GL_TRUE ); //Turn depth masking back on

        runtime_error_check();
    }

    void onResize(int w, int h) {
        info.windowWidth = w;
        info.windowHeight = h;
        //Recalculate the projection matrix used by camera
        calcProjection(camera); 
    }

    const float player_box_radius = 0.7f;
    void onKey(int key, int action) {
        //If something did happen
      if (action) {
            //WASD movement locked to the x,z plane
            double speed = 0.3;
            double x1, x2, z1, z2;
            x1 = camera.position[0];
            z1 = camera.position[2];
            x2 = camera.focus[0];
            z2 = camera.focus[2];
            //Angle of rotation of the lookat around camera
            double theta = atan(((z2 - z1))/(x2 - x1)); 

            //If outside the range of arctan, then add 180 to get to the other two quadrents
            if(x2 - x1 < 0){
                theta += M_PI;
            }

            //Calculate the new theoretical position of the camera based on what key is pressed
            float newCamX = camera.position[0];
            float newCamZ = camera.position[2];
            switch (key) {
                case 'W':
                    newCamX += speed * cos(theta);
                    newCamZ += speed * sin(theta);
                    break;
                case 'A': 
                    newCamX += speed * cos(theta - M_PI/2);
                    newCamZ += speed * sin(theta - M_PI/2);
                    break;
                case 'S':
                    newCamX += speed * cos(theta + M_PI);
                    newCamZ += speed * sin(theta + M_PI);
                    break;
                case 'D':
                    newCamX += speed * cos(theta + M_PI/2);
                    newCamZ += speed * sin(theta + M_PI/2);
                    break;
            }

            //Check for collisions
            //If there is a collision, then shorten the end location of the camera
            for(int i = 0; i < objects.size(); i++){
                float objX = objects[i].obj2world[3][0];
                float objZ = objects[i].obj2world[3][2];
                float objRad = 1.0f;

                float clipX = 0;
                float clipZ = 0;
                //Object is +x,+z to the camera
                if(objX - newCamX >= 0 && objZ - newCamZ >= 0){
                    clipX = (objX - objRad) - (newCamX + player_box_radius);
                    clipZ = (objZ - objRad) - (newCamZ + player_box_radius);
                    if(clipX < 0 && clipZ  < 0){
                        //std::cout << "Overlap +x +z" << std::endl;
                        if(-clipX > -clipZ){
                            newCamZ += clipZ;
                        } else{
                            newCamX += clipX;
                        }
                    }
                }
                //Object is +x, -z to the camera
                else if(objX - newCamX >= 0 && objZ - newCamZ < 0){
                    clipX = (objX - objRad) - (newCamX + player_box_radius);
                    clipZ = (newCamZ - player_box_radius) - (objZ + objRad);
                    if( clipX < 0 && clipZ < 0){
                        //std::cout << "Overlap +x -z" << std::endl;
                        if(-clipX > -clipZ){
                            newCamZ -= clipZ;
                        } else{
                            newCamX += clipX;
                        }
                    }
                }
                //Object is -x, +z to the camera
                else if(objX - newCamX < 0 && objZ - newCamZ >= 0){
                    clipX = (newCamX - player_box_radius) - (objX + objRad);
                    clipZ = (objZ - objRad) - (newCamZ + player_box_radius);
                    if(clipX < 0 && clipZ < 0){
                        //std::cout << "Overlap -x +z" << std::endl;
                        if(-clipX > -clipZ){
                            newCamZ += clipZ;
                        } else{
                            newCamX -= clipX;
                        }
                    }
                }
                //Object is -x, -z to the camera
                else{
                    clipX = (newCamX - player_box_radius) - (objX + objRad);
                    clipZ = (newCamZ - player_box_radius) - (objZ + objRad);
                    if(clipX < 0 && clipZ < 0){
                        //std::cout << "Overlap -x -z" << std::endl;
                        if(-clipX > -clipZ){
                            newCamZ -= clipZ;
                        } else{
                            newCamX -= clipX;
                        }
                    }
                }
            }

            //Move the camera to it's new pos
            camera.position[0] = newCamX;
            camera.position[2] = newCamZ;

            //Change lookat to follow direction
            int x, y;
            getMousePosition(x, y);
            onMouseMove(x, y);
        }

    }

    double offsetX = 0;
    double offsetY = 0;
    void onMouseMove(int x, int y)
    {
        //Keeping a disabled mouse cursor's coords within the window frame so x and y don't go to infinity
        //More important for the y because we want to have an unlimited range in the x direction but want to limit the y
        //Source: https://stackoverflow.com/questions/65541131/how-to-keep-mouse-coordinates-within-window-height-width-limits
        //store offsetX and offsetY values (and dont forget to give them 0 as initial value)
        double overshootX = x - offsetX;
        double overshootY = y - offsetY;

        if (overshootX > info.windowWidth) {
            offsetX = offsetX + overshootX - info.windowWidth;
        }
        if (overshootX < 0) {
            offsetX = offsetX + overshootX;
        }
        if (overshootY > info.windowHeight) {
            offsetY = offsetY + overshootY - info.windowHeight;
        }
        if (overshootY < 0) {
            offsetY = offsetY + overshootY;
        }

        float withinWindowCursorPosX = x - offsetX; 
        float withinWindowCursorPosY = y - offsetY;

        //Restricted x,y debug stuff
        //std::cout << "Mouse X = " << x << " | Mouse Y = " << y << std::endl;
        //std::cout << "X =  " << withinWindowCursorPosX << " | Y = " << withinWindowCursorPosY << std::endl;

        //Calculate the lookat based on the mouse position
        //radius is the distance to the lookat from the camera
        //theta is the angle looking up
        //phi is the angle looking around
        float radius = 1;
        float theta = 0; //((withinWindowCursorPosY / 900 * 160) - 80);
        float phi = x;
        //std::cout << "Theta = " << theta << " | Phi = " << phi << std::endl;
        float lookatX = radius * cos(theta * M_PI/180) * cos(phi * M_PI/180);
        float lookatY = -radius * sin(theta * M_PI/180);
        float lookatZ = radius * cos(theta * M_PI/180) * sin(phi * M_PI/180);
        //Lookat debug stuff
        //std::cout << "LookatX = " << lookatX << " | LookatY = " << lookatY << " | LookatZ = " << lookatZ << std::endl;

        //The focus is relative to the camera position so add the postion to the lookat
        camera.focus[0] = lookatX + camera.position[0];
        camera.focus[1] = lookatY + camera.position[1];
        camera.focus[2] = lookatZ + camera.position[2];
    }


    void runtime_error_check(GLuint tracker = 0)
    {
        GLenum err = glGetError();
        
        if (err) {
            char buf[50];
            sprintf(buf, "Error(%d) = %x", tracker, err);
            MessageBoxA(NULL, buf, "Error running program!", MB_OK);
        }
    }

    void compiler_error_check(GLuint shader)
    {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            if(maxLength == 0){
                char buf[50];
                sprintf(buf, "Possibly shaders not found!", maxLength);
                MessageBoxA(NULL, buf, "Error in shader compilation", MB_OK);
            } else {
                // Provide the infolog in whatever manor you deem best.
                MessageBoxA(NULL, &errorLog[0], "Error in shader compilation", MB_OK);
            }

            // Exit with failure.
            glDeleteShader(shader); // Don't leak the shader.
        }
    }

    private:
        //Scene Rendering Information
        GLuint rendering_program; //Program reference for scene generation
        GLuint vertex_array_object;
        
        //Uniform attributes for Scene Render
        GLuint transform_ID; //Dynamic transform of object
        GLuint perspec_ID;   //Perspective transform
        GLuint toCam_ID;     //World to Camera transform
        GLuint vertex_ID;    //This will be mapped to different objects as we load them
        GLuint uv_ID;

        //Structure to hold all the object info
        struct obj_t{
            //Data for object loaded from file
            std::vector<vmath::vec4> verticies;
            std::vector<vmath::vec4> normals;
            std::vector<vmath::vec2> uv;
            GLuint vertNum; //This should be the same as vertivies.size()

            //Handle from OpenGL set up
            GLuint vertices_buffer_ID; 
            GLuint uv_buffer_ID;       

            //Object to World transforms
            vmath::mat4 obj2world;

            //Texture Info
            GLuint texture_ID;
        };

        //Hold all of our objects
        std::vector<obj_t> objects;



        //Data for Skycube
        GLuint sc_program; //Program refernce

        GLuint sc_vertex_array_object;
        GLuint sc_map_texture;

        //TODO:: Rename these better names
        GLuint sc_Camera;
        GLuint sc_Perspective;

        std::vector<vmath::vec4> skycube_vertices; //List of skycube vertexes

        bool autoRotate = false;

        // Camera Stuff
        struct camera_t{ //Keep all of our camera stuff together
            float camera_near;   //Near clipping mask
            float camera_far;    //Far clipping mask
            float fovy;          //Field of View in y
            float aspect;        //Aspect ratio (w/h)
            vmath::mat4 proj_Matrix; //Collection of the above

            vmath::vec3 position; //Current world coordinates of the camera
            vmath::vec3 focus; //Current world focus of the camera
            //TODO:: Maybe we just want to use euler angles here

            vmath::mat4 view_mat; //World to Camera matrix
            vmath::mat4 view_mat_no_translation; //World to Camera matrix with no translation
        } camera;

        //Utility to update project matrix and view matrix of a camera_t
        void calcProjection(camera_t &cur){
            cur.aspect = static_cast<float>(info.windowWidth) / static_cast<float>(info.windowHeight); //Maybe this will keep it updated?
            //vmath perspective
            cur.proj_Matrix = vmath::perspective(cur.fovy,cur.aspect, cur.camera_near, cur.camera_far);
        }
        void calcView(camera_t &cur){
            cur.view_mat = vmath::lookat(cur.position,cur.focus,vmath::vec3(0.0f,1.0f,0.0f)); //Based on position and focus location
            cur.view_mat_no_translation = cur.view_mat;   
            //Removing the tranlational elements for skybox         
            cur.view_mat_no_translation[3][0] = 0;
            cur.view_mat_no_translation[3][1] = 0;
            cur.view_mat_no_translation[3][2] = 0;
        }


};


DECLARE_MAIN(test_app);
