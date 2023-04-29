#include<iostream>
#include<map>
#include<time.h>

//Class for the maze
enum mazeTiles{
    empty, wall, start, end
};

class Maze{
    public:
        //Default constructor
        Maze(){
            mazeWidth = 0;
            mazeHeight = 0;
        }

        //Generate a blank maze of the given width and height
        Maze(int width, int height){
            mazeWidth = width;
            mazeHeight = height;

            for(int i = 0; i < mazeHeight; i++){
                for(int j = 0; j < mazeWidth; j++){
                    maze.insert(std::pair<std::pair<int, int>, mazeTiles>(std::pair<int, int>(i, j), empty));
                }
            }

            //Add start and end in opposite corners of the maze
            maze.at(std::pair<int, int>(0, 0)) = start;
            maze.at(std::pair<int, int>(mazeHeight - 1, mazeWidth - 1)) = end;

            //Generate the maze
            generateMaze(0, mazeWidth - 1, 0, mazeHeight - 1, (bool)(rand() % 2));
        }

        void generateMaze(int startWidth, int endWidth, int startHeight, int endHeight, bool ver = true){
            //If the size of the subsection has a width or height of 1 then there is no need to continue cutting up
            if(endWidth - startWidth  <= 1 || endHeight - startHeight <= 1){
                return;
            }

            //Print some debug stuff
            //std::cout << "StartWidth: " << startWidth <<  " | EndWidth: " << endWidth << " | StartHeight: " << startHeight << " | EndHeight: " << endHeight <<  " | " << std::endl;
            //printMaze();
            //std::cout << std::endl;

            //Pick a horozontal or vertical position(depending of ver) to split the space at given the current subspace
            int splitAt;
            int splitConnection;
            //Split vertically
            if(ver){
                //Calcluate split with a margine
                splitAt = startWidth + 1 + rand() % (endWidth - startWidth - 1);
                int split = rand() % 2;
                if(split == 0){
                    splitConnection = endHeight;
                } else {
                    splitConnection = startHeight; //startHeight + 1 + rand() % (endHeight - startHeight - 1);
                }
                
                //Set all tiles in the subsection at that split to wall tiles
                for(int i = startHeight; i < endHeight + 1; i++){
                    if(i != splitConnection){
                        maze.at(std::pair<int, int>(i, splitAt)) = wall;
                    }
                }
                
                //Call generate maze two more times for each of the new subsections created by this split
                generateMaze(startWidth, splitAt - 1, startHeight, endHeight, !ver);
                generateMaze(splitAt + 1, endWidth, startHeight, endHeight, !ver);

            } else /*Split horizontally*/ {
                //Calcluate split with a margine
                splitAt = startHeight + 1 + rand() % (endHeight - startHeight - 1);

                int split = rand() % 2;
                if(split == 0){
                    splitConnection = endWidth;
                } else {
                    splitConnection = startWidth; //startWidth + 1 + rand() % (endWidth - startWidth - 1);
                }

                //Set all tiles in the subsection at that split to wall tiles
                for(int i = startWidth; i < endWidth + 1; i++){
                    if(i != splitConnection){
                        maze.at(std::pair<int, int>(splitAt, i)) = wall;
                    }
                }

                //Call generate maze two more times for each of the new subsections created by this split
                generateMaze(startWidth, endWidth, startHeight, splitAt - 1, !ver);
                generateMaze(startWidth, endWidth, splitAt + 1, endHeight, !ver);
            }
        }

        //Print the maze out to the console
        bool printMaze(){
            for(int i = 0; i < mazeWidth + 2; i++){
                std::cout << "X";
            }
            std::cout << std::endl;

            for(int i = 0; i < mazeHeight; i++){
                std::cout << "X";
                for(int j = 0; j < mazeWidth; j++){
                    switch (getTile(j, i))
                    {
                    case empty:
                        std::cout << " ";
                        break;
                    case wall:
                        std::cout << "X";
                        break;
                    case start:
                        std::cout << "S";
                        break;
                    case end:
                        std::cout << "E";
                        break;
                    default:
                        std::cout << std::endl << "Character not found!" << std::endl;
                        return false;
                    }
                }
                std::cout << "X" << std::endl;
            }

            for(int i = 0; i < mazeWidth + 2; i++){
                std::cout << "X";
            }
            std::cout << std::endl;
            return true;
        }

        //Getters for width and height
        int getWidth() {return mazeWidth;}
        int getHeight() {return mazeHeight;}
        //Get a tile in the maze
        mazeTiles getTile(int x, int z){ return maze.at(std::pair<int, int>(z, x));}

    private:
        int mazeWidth = 0;
        int mazeHeight = 0;
        //key is width then height for the pair
        std::map<std::pair<int, int>, mazeTiles> maze;
};