import streamlit as st
import numpy as np
import subprocess
import os
import matplotlib.pyplot as plt

DATA_FILE = "drivers.txt"
if not os.path.exists(DATA_FILE):
    np.random.seed(42)
    points = np.random.uniform(0, 100, (1500, 2))
    with open(DATA_FILE, "w") as f:
        for p in points:
            f.write(f"{p[0]} {p[1]}\n")

st.set_page_config(layout="wide", page_title="Spatial Search Engine")
st.title("Geospatial Search Engine (QuadTree Core)")
st.markdown("Combines a memory-managed C++ spatial index with a visualization frontend to query location datasets.")

# Sidebar Controls
st.sidebar.header("Search & Execution Configuration")
qx = st.sidebar.slider("Query Target X Center", 0.0, 100.0, 50.0)
qy = st.sidebar.slider("Query Target Y Center", 0.0, 100.0, 50.0)
q_radius = st.sidebar.slider("Search Box Half-Extent", 1.0, 25.0, 10.0)
k_target = st.sidebar.slider("K-Nearest Drivers (Green)", 1, 20, 3) # Added Slider

if st.sidebar.button("Build and Compile C++ Engine", type="primary"):
    try:
        res = subprocess.run(["g++", "-O3", "quadtree.cpp", "-o", "quadtree"], capture_output=True, text=True)
        if res.returncode == 0:
            st.sidebar.success("Backend built and optimized successfully!")
        else:
            st.sidebar.error(f"Build failed:\n{res.stderr}")
    except Exception as e:
        st.sidebar.error(f"Compiler utility not found: {e}")

binary_name = "./quadtree" if os.name != "nt" else "quadtree.exe"

if not os.path.exists(binary_name):
    st.warning("Please click 'Build and Compile C++ Engine' in the sidebar configuration to initialize the backend application.")
else:
    # Pass the new k_target variable to the C++ engine
    cmd = [binary_name, DATA_FILE, str(qx), str(qy), str(q_radius), str(q_radius), str(k_target)]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        lines = result.stdout.strip().split("\n")
        
        qtree_time = 0
        linear_time = 0
        found_points = []
        nearest_points = []
        
        # Parse Line 1: Benchmark
        if len(lines) >= 1 and lines[0].startswith("BENCHMARK:"):
            _, q_t, l_t = lines[0].split(":")
            qtree_time = int(q_t)
            linear_time = int(l_t)
            
        # Parse Line 2: All points in box
        if len(lines) >= 2 and lines[1]:
            pairs = lines[1].split(" ")
            for pair in pairs:
                if "," in pair:
                    fx, fy = map(float, pair.split(","))
                    found_points.append((fx, fy))
                    
        # Parse Line 3: Top K points
        if len(lines) >= 3 and lines[2]:
            pairs = lines[2].split(" ")
            for pair in pairs:
                if "," in pair:
                    nx, ny = map(float, pair.split(","))
                    nearest_points.append((nx, ny))
                    
        # Dashboard UI
        st.markdown("---")
        st.subheader("⚡ Execution Benchmark")
        
        col1, col2, col3 = st.columns(3)
        with col1:
            st.metric(label="QuadTree Query Time", value=f"{qtree_time} μs")
        with col2:
            st.metric(label="Linear Search Time", value=f"{linear_time} μs")
        with col3:
            if qtree_time > 0:
                speedup = round(linear_time / max(1, qtree_time), 1)
                st.metric(label="Efficiency Gain", value=f"{speedup}x Faster")
            else:
                st.metric(label="Efficiency Gain", value="N/A")
                
        st.markdown("---")

        # Map Visualization
        all_points = np.loadtxt(DATA_FILE)
        fig, ax = plt.subplots(figsize=(7, 7))
        
        # 1. Plot all background points (Gray)
        ax.scatter(all_points[:, 0], all_points[:, 1], c='lightgray', s=8, label='Inactive Nodes / Drivers')
        
        # 2. Plot all points found in the box (Red)
        if found_points:
            fx_arr, fy_arr = zip(*found_points)
            ax.scatter(fx_arr, fy_arr, c='red', s=25, label='Targets in Area')
            
        # 3. Plot the K-nearest points (Green, slightly larger)
        if nearest_points:
            nx_arr, ny_arr = zip(*nearest_points)
            ax.scatter(nx_arr, ny_arr, c='lime', s=70, edgecolor='black', label=f'Top {k_target} Nearest')
            
        # Draw the blue query box
        search_box = plt.Rectangle((qx - q_radius, qy - q_radius), q_radius * 2, q_radius * 2, 
                                     edgecolor='blue', facecolor='none', linewidth=2, label='Query Boundary')
        ax.add_patch(search_box)
        
        ax.set_xlim(0, 100)
        ax.set_ylim(0, 100)
        ax.legend(loc='upper right')
        
        st.pyplot(fig)
        
    except Exception as e:
        st.error(f"Pipeline Interruption: {e}")