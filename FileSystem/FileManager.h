#ifndef FILESYSTEM_FILE_MANAGER_H
#define FILESYSTEM_FILE_MANAGER_H

#include "../Shapes/Shape.h"
#include "../Shapes/Line.h"
#include "../Shapes/Circle.h"
#include "../Shapes/Ellipse.h"
#include "../Shapes/Curve.h"
#include "../Core/Enums.h"
#include <string>
#include <vector>

// Saves and loads drawing data from files.
class FileManager {
public:
    FileManager();

    void saveShapes(const std::string& filePath, const std::vector<Shape*>& shapes);
    void loadShapes(const std::string& filePath, std::vector<Shape*>& shapes);
};

#endif
