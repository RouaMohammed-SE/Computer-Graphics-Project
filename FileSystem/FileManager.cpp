#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
FileManager::FileManager() {}

void FileManager::saveShapes(const std::string& filePath, const std::vector<Shape*>& shapes) {
    // Save shapes to file.
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cout << "[ERROR] Cannot open file for saving: " << filePath << std::endl;
        return;
    }
    for (const Shape* shape : shapes) {
        file << shape->serialize() << "\n";
    }
    file.close();
    std::cout << "[LOG]   Saved " << shapes.size() << " shape(s) to " << filePath << std::endl;
}

void FileManager::loadShapes(const std::string& filePath, std::vector<Shape*>& shapes) {
    // Load shapes from file.
    // Clean up existing shapes first.
    for (Shape* shape : shapes) {
        delete shape; 
    }
    shapes.clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "[ERROR] Cannot open file for loading: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string shapeType;
        ss >> shapeType;

        if (shapeType == "LINE") {
            int x1, y1, x2, y2, alg, r, g, b;
            ss >> x1 >> y1 >> x2 >> y2 >> alg >> r >> g >> b;
            shapes.push_back(new Line(Point(x1, y1), Point(x2, y2), static_cast<LineAlgorithmType>(alg), Color(r, g, b)));
        }
        else if (shapeType == "CIRCLE") {
            int cx, cy, radius, alg, r, g, b;
            ss >> cx >> cy >> radius >> alg >> r >> g >> b;
            shapes.push_back(new Circle(Point(cx, cy), radius, static_cast<CircleAlgorithmType>(alg), Color(r, g, b)));
        }
        else if (shapeType == "ELLIPSE") {
            int cx, cy, rx, ry, alg, r, g, b;
            ss >> cx >> cy >> rx >> ry >> alg >> r >> g >> b;
            shapes.push_back(new MyEllipse(Point(cx, cy), rx, ry, static_cast<EllipseAlgorithmType>(alg), Color(r, g, b)));
        }
        else if (shapeType == "CURVE") {
            double tension;
            int numPoints;
            ss >> tension >> numPoints;
            std::vector<Point> pts;
            for (int i = 0; i < numPoints; i++) {
                int px, py;
                ss >> px >> py;
                pts.push_back(Point(px, py));
            }
            int r, g, b;
            ss >> r >> g >> b;
            shapes.push_back(new Curve(pts, tension, Color(r, g, b)));
        }
        else {
            std::cout << "[WARN]  Unknown shape type in file: " << shapeType << std::endl;
        }
    }
    file.close();
    std::cout << "[LOG]   Loaded " << shapes.size() << " shape(s) from " << filePath << std::endl;
}