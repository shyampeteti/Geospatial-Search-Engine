# Geospatial Search Engine

A high-performance geospatial search engine that efficiently performs 2D spatial queries using a **QuadTree** data structure. The project combines a **C++ backend** for fast spatial indexing with a **Python Streamlit** interface for interactive visualization and benchmarking.

---

## Features

- 🚀 **Fast Spatial Search** using a custom QuadTree implementation
- 📍 Efficient range queries with **O(log N)** average search complexity
- 💻 **C++ Core Engine** optimized with `-O3` for maximum performance
- 📊 Interactive **Streamlit Dashboard** for visualization
- 📈 **Live Performance Benchmarking** comparing QuadTree search against linear search
- 🎯 Real-time visualization of search regions and matching points

---

## Project Overview

### C++ QuadTree Engine
- Implemented a QuadTree from scratch for efficient spatial indexing.
- Supports recursive node subdivision when capacity exceeds a threshold.
- Uses bounding-box intersection checks to prune unnecessary regions during search.
- Optimized for low-latency range queries.

### Python Streamlit Interface
- Interactive web application built with Streamlit.
- Allows users to:
  - Generate and visualize random 2D points.
  - Move and resize the search region.
  - Query the C++ backend.
  - Display returned points on the map.
  - Compare QuadTree and linear search performance.

### Live Benchmarking
- Measures execution time for:
  - QuadTree search
  - Brute-force linear search
- Displays the runtime difference in real time.

---

## How It Works

### 1. Building the QuadTree

- The entire map is initially represented as a single bounding box.
- Each node stores up to **4 points**.
- Once a node exceeds its capacity, it subdivides into four equal child regions:
  - North-West
  - North-East
  - South-West
  - South-East
- Points are redistributed recursively, creating a hierarchical spatial index.

### 2. Performing Range Search

Instead of scanning every point:

- The algorithm checks whether a QuadTree node intersects the search region.
- Non-overlapping nodes are immediately discarded.
- Only relevant branches are explored recursively.
- This significantly reduces the number of point comparisons, yielding approximately **O(log N)** search complexity on average.

---

## Tech Stack

- **C++** — QuadTree implementation and search engine
- **Python**
- **Streamlit** — Interactive UI
- **NumPy** — Data generation
- **Matplotlib** — Visualization

---

## Project Structure

```
.
├── app.py              # Streamlit application
├── quadtree.cpp        # C++ QuadTree implementation
├── quadtree.exe        # Compiled executable (Windows)
├── README.md
```

---

## Installation

### 1. Clone the Repository

```bash
git clone <repository-url>
cd Geospatial-Search-Engine
```

### 2. Install Python Dependencies

```bash
pip install streamlit numpy matplotlib
```

---

## Compile the C++ Engine

Compile with maximum optimization (`-O3`).

### Linux / macOS

```bash
g++ -O3 quadtree.cpp -o quadtree
```

### Windows

```bash
g++ -O3 quadtree.cpp -o quadtree.exe
```

---

## Run the Application

Launch the Streamlit interface:

```bash
streamlit run app.py
```

Open the local URL displayed in the terminal (typically `http://localhost:8501`).

---

## Performance

Compared to a traditional linear search, the QuadTree:

- Avoids scanning every point.
- Prunes irrelevant regions early.
- Provides significantly faster query times on large datasets.
- Includes built-in benchmarking to visualize the performance improvement.

---

## Future Improvements

- k-Nearest Neighbor (k-NN) Search
- Point Insertion & Deletion
- Circular Range Queries
- Dynamic Dataset Loading
- Support for Geographic Coordinates (Latitude/Longitude)
- Parallel Search Optimization

---

## Author

Developed as a data structures and algorithms project demonstrating efficient spatial indexing, recursive partitioning, and interactive visualization.