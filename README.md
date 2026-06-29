# Geospatial Search Engine

A high-performance geospatial search engine for efficient 2D spatial queries using a custom **QuadTree** implementation. The project combines a **C++ backend** with a **Streamlit** interface for visualization and performance benchmarking.

## Features

* Custom **QuadTree** implementation for efficient spatial indexing and range queries.
* **k-Nearest Drivers** search to retrieve the nearest points to a given location.
* Optimized **C++ backend** compiled with `-O3` for high-performance query execution.
* Interactive **Streamlit** dashboard for data visualization and query execution.
* Built-in benchmarking comparing **QuadTree** search against brute-force linear search.

## Tech Stack

* **C++** – QuadTree implementation and query engine
* **Python**
* **Streamlit**
* **NumPy**
* **Matplotlib**

## Project Structure

```text
.
├── app.py              # Streamlit application
├── quadtree.cpp        # QuadTree implementation
├── quadtree.exe        # Compiled executable (Windows)
├── README.md
```

## Installation

Install the required Python packages:

```bash
pip install streamlit numpy matplotlib
```

Compile the C++ engine:

**Linux/macOS**

```bash
g++ -O3 quadtree.cpp -o quadtree
```

**Windows**

```bash
g++ -O3 quadtree.cpp -o quadtree.exe
```

## Run

```bash
streamlit run app.py
```

## Future Improvements

* Dynamic point insertion and deletion
* Circular range queries
* Support for geographic coordinates
* Parallel query processing
