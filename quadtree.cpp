#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>

struct Point {
    double x;
    double y;
};

struct Boundary {
    double x;  
    double y;  
    double hw; 
    double hh; 

    bool contains(const Point& p) const {
        return (p.x >= x - hw && p.x <= x + hw &&
                p.y >= y - hh && p.y <= y + hh);
    }

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

    QuadTree* nw;
    QuadTree* ne;
    QuadTree* sw;
    QuadTree* se;

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

        if (points.size() < capacity && !divided) {
            points.push_back(p);
            return true;
        }

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

        if (nw->insert(p)) return true;
        if (ne->insert(p)) return true;
        if (sw->insert(p)) return true;
        if (se->insert(p)) return true;

        return false;
    }

    void query(const Boundary& range, std::vector<Point>& found) const {
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

// Naive Linear Search for Performance Comparison
void linear_query(const Boundary& range, const std::vector<Point>& all_points, std::vector<Point>& found) {
    for (const auto& p : all_points) {
        if (range.contains(p)) {
            found.push_back(p);
        }
    }
}

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

    Boundary world = {50.0, 50.0, 50.0, 50.0};
    QuadTree qtree(world, 4); 

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error opening data file\n";
        return 1;
    }

    std::vector<Point> all_raw_points;
    double x, y;
    while (infile >> x >> y) {
        qtree.insert({x, y});
        all_raw_points.push_back({x, y}); // Keep raw list for linear comparison
    }
    infile.close();

    Boundary search_range = {qx, qy, qhw, qhh};
    
    // Benchmark 1: QuadTree Query Time
    auto start_q = std::chrono::high_resolution_clock::now();
    std::vector<Point> qtree_results;
    qtree.query(search_range, qtree_results);
    auto end_q = std::chrono::high_resolution_clock::now();
    auto duration_q = std::chrono::duration_cast<std::chrono::microseconds>(end_q - start_q).count();

    // Benchmark 2: Naive Linear Query Time
    auto start_l = std::chrono::high_resolution_clock::now();
    std::vector<Point> linear_results;
    linear_query(search_range, all_raw_points, linear_results);
    auto end_l = std::chrono::high_resolution_clock::now();
    auto duration_l = std::chrono::duration_cast<std::chrono::microseconds>(end_l - start_l).count();

    // Line 1: Performance metrics piped out to Python
    std::cout << "BENCHMARK:" << duration_q << ":" << duration_l << "\n";

    // Line 2: Spatial coordinates matching the query criteria
    for (size_t i = 0; i < qtree_results.size(); ++i) {
        std::cout << qtree_results[i].x << "," << qtree_results[i].y;
        if (i < qtree_results.size() - 1) std::cout << " ";
    }
    std::cout << "\n";

    return 0;
}