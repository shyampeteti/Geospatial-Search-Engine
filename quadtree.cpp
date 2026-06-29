#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

struct Point {
    double x;
    double y;
};

struct Boundary {
    double x;  // Center X coordinate
    double y;  // Center Y coordinate
    double hw; // Half-width (extent from center to edge)
    double hh; // Half-height (extent from center to edge)

    // Checks if a point lies within this bounding box
    bool contains(const Point& p) const {
        return (p.x >= x - hw && p.x <= x + hw &&
                p.y >= y - hh && p.y <= y + hh);
    }

    // Checks if another bounding box overlaps with this one
    bool intersects(const Boundary& other) const {
        return !(other.x - other.hw > x + hw ||
                 other.x + other.hw < x - hw ||
                 other.y - other.hh > y + hh ||
                 other.y + other.hh < y - hh);
    }
};

class QuadTree {
private:
    Boundary boundary;
    int capacity;
    std::vector<Point> points;
    bool divided;

    // The four quadrants (Children nodes)
    QuadTree* nw;
    QuadTree* ne;
    QuadTree* sw;
    QuadTree* se;

    // Splits the current node into 4 equal quadrants
    void subdivide() {
        double x = boundary.x;
        double y = boundary.y;
        double hw = boundary.hw / 2.0;
        double hh = boundary.hh / 2.0;

        nw = new QuadTree({x - hw, y + hh, hw, hh}, capacity);
        ne = new QuadTree({x + hw, y + hh, hw, hh}, capacity);
        sw = new QuadTree({x - hw, y - hh, hw, hh}, capacity);
        se = new QuadTree({x + hw, y - hh, hw, hh}, capacity);

        divided = true;
    }

public:
    QuadTree(Boundary b, int cap) : boundary(b), capacity(cap), divided(false), 
                                    nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}

    ~QuadTree() {
        if (divided) {
            delete nw;
            delete ne;
            delete sw;
            delete se;
        }
    }

    bool insert(const Point& p) {
        if (!boundary.contains(p)) {
            return false;
        }

        // If there's space and we haven't split, store it here
        if (points.size() < capacity && !divided) {
            points.push_back(p);
            return true;
        }

        // If full, split the node and push existing elements down
        if (!divided) {
            subdivide();
            for (const auto& pt : points) {
                if (nw->insert(pt)) continue;
                if (ne->insert(pt)) continue;
                if (sw->insert(pt)) continue;
                if (se->insert(pt)) continue;
            }
            points.clear();
        }

        // Route the incoming point to its proper quadrant
        if (nw->insert(p)) return true;
        if (ne->insert(p)) return true;
        if (sw->insert(p)) return true;
        if (se->insert(p)) return true;

        return false;
    }

    void query(const Boundary& range, std::vector<Point>& found) const {
        // Pruning Step: If search box doesn't touch this quadrant, instantly discard it
        if (!boundary.intersects(range)) {
            return;
        }

        if (divided) {
            nw->query(range, found);
            ne->query(range, found);
            sw->query(range, found);
            se->query(range, found);
        } else {
            for (const auto& p : points) {
                if (range.contains(p)) {
                    found.push_back(p);
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <points_file> <qx> <qy> <qhw> <qhh>\n";
        return 1;
    }

    std::string filename = argv[1];
    double qx = std::stod(argv[2]);
    double qy = std::stod(argv[3]);
    double qhw = std::stod(argv[4]);
    double qhh = std::stod(argv[5]);

    // Initialize map boundaries from 0 to 100 on both axes
    Boundary world = {50.0, 50.0, 50.0, 50.0};
    QuadTree qtree(world, 4); // Max 4 points per quadrant node

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error opening data file\n";
        return 1;
    }

    double x, y;
    while (infile >> x >> y) {
        qtree.insert({x, y});
    }
    infile.close();

    Boundary search_range = {qx, qy, qhw, qhh};
    std::vector<Point> results;
    qtree.query(search_range, results);

    // Stream coordinates back out to standard out for Python ingestion
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << results[i].x << "," << results[i].y;
        if (i < results.size() - 1) std::cout << " ";
    }
    std::cout << "\n";

    return 0;
}