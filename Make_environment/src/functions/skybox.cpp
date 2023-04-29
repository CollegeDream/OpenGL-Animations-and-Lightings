/*
* Skybox Utility Functions
* Whitworth University, CS357, Computer Graphics
* Developed by Scott Griffith, Spring 2021
*
* Based on work from:
* Kent Jones, Whitworth
* Dr. Anton Gerdelan, https://github.com/capnramses 
*                     https://antongerdelan.net/opengl/cubemaps.html                 
*/
#include <skybox.h>
#include <loadingFunctions.h>
#include <fstream>

void createCube(std::vector<vmath::vec4> &vertices){
    //We need to enumerate all of the different sides of a cube
    //Each one will have two triangles that make up the side
    //The goal is to make this a unit cube that we can scale up later
    subPoints(1,0,3,2,vertices); //Side 1
    subPoints(2,3,7,6,vertices); //Side 2

    subPoints(3,0,4,7,vertices);
    subPoints(6,5,1,2,vertices);

    subPoints(4,5,6,7,vertices);
    subPoints(5,4,0,1,vertices);

}

void subPoints(int a, int b, int c, int d,std::vector<vmath::vec4> &vertices){
    //Triangle 1
    vertices.push_back(CUBE_VERTICES[a]);
    //vertices.push_back(CUBE_VERTICES[b]); //This is has to do with the winding order (this will be CW)
    vertices.push_back(CUBE_VERTICES[c]);

    vertices.push_back(CUBE_VERTICES[b]); //This is CCW winding

    //Triangle 2
    vertices.push_back(CUBE_VERTICES[a]);
    //vertices.push_back(CUBE_VERTICES[c]); //This is has to do with the winding order (this will be CW)
    vertices.push_back(CUBE_VERTICES[d]);

    vertices.push_back(CUBE_VERTICES[c]); //This is CCW winding
}

void loadCubeTextures(std::string directory, GLuint texture_ID){
    //Load each side in from the file
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, directory+".\\sc_front.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, directory+".\\sc_back.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, directory+".\\sc_down.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, directory+".\\sc_up.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_X, directory+".\\sc_right.bmp");
    loadCubeSide(texture_ID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, directory+".\\sc_left.bmp");

    // Set standard parameters for the cube map texture mapping
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );   
}

void loadCubeSide(GLint texture_ID, GLenum side, std::string file){
    //If you are curious why so many unsigned chars: https://stackoverflow.com/questions/75191/what-is-an-unsigned-char
    // Bind the next call to this CUBE_MAP
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ID);

    //Set up some function variables
    //Memory location of where we will put the texture data
    unsigned char *texture_data; 
    unsigned int tData_width = 0;
    unsigned int tData_height = 0;

    //Load texture data from Bitmap into memory
    load_BMP(file, texture_data, tData_width, tData_height);
    //This functionality was moved over to loadingFunctions.cpp to support other bitmap loading

    // Load the image data into a buffer to send to the GPU
	glTexImage2D( side, // Which side are you loading in (should be an enum)
                     0, // Level of detail, 0 base level
               GL_RGBA, // Internal (target) format of data, in this case Red, Gree, Blue, Alpha
           tData_width, // Width of texture data (max is 1024, but maybe more)
          tData_height, // Height of texture data
                     0, //border (must be zero)
               GL_RGBA, //Format of input data (in this case we added the alpha when reading in data)
      GL_UNSIGNED_BYTE, //Type of data being passed in
          texture_data); // Finally pointer to actual data to be passed in
    
    // Free the memory we used to load the texture
	delete[] texture_data;

    return; 
}
