import streamlit as st
import numpy as np
import subprocess
import os
import matplotlib.pyplot as plt

# Auto-generate a dummy coordinate dataset if file does not exist
DATA_FILE = "drivers.txt"
if not os.path.exists(DATA_FILE):
    np.random.seed(42)
    # Generate 1500 spatial coordinates between 0 and 100
    points = np.random.uniform(0, 100, (1500, 2))
    with open(DATA_FILE, "w") as f:
        for p in points:
            f.write(f"{p[0]} {p[1]}\n")

st.set_page_config(layout="wide", page_title="Spatial Search Engine")
st.title("Geospatial Search Engine (QuadTree Core)")
st.markdown("Combines a memory-managed C++ spatial index with a visualization frontend to query location datasets.")

# Sidebar Configuration Layout
st.sidebar.header("Search & Execution Configuration")
qx = st.sidebar.slider("Query Target X Center", 0.0, 100.0, 50.0)
qy = st.sidebar.slider("Query Target Y Center", 0.0, 100.0, 50.0)
q_radius = st.sidebar.slider("Search Box Half-Extent", 1.0, 25.0, 10.0)

if st.sidebar.button("Build and Compile C++ Engine", type="primary"):
    try:
        res = subprocess.run(["g++", "-O3", "quadtree.cpp", "-o", "quadtree"], capture_output=True, text=True)
        if res.returncode == 0:
            st.sidebar.success("Backend built and optimized successfully!")
        else:
            st.sidebar.error(f"Build failed:\n{res.stderr}")
    except Exception as e:
        st.sidebar.error(f"Compiler utility not found: {e}")

# Check if compiled binary executable matches environment
binary_name = "./quadtree" if os.name != "nt" else "quadtree.exe"

if not os.path.exists(binary_name):
    st.warning("Please click 'Build and Compile C++ Engine' in the sidebar configuration to initialize the backend application.")
else:
    # Build arguments string list to pass via sub-process pipeline
    cmd = [binary_name, DATA_FILE, str(qx), str(qy), str(q_radius), str(q_radius)]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output_str = result.stdout.strip()
        
        # Parse result string array from stdout stream
        found_points = []
        if output_str:
            pairs = output_str.split(" ")
            for pair in pairs:
                if "," in pair:
                    fx, fy = map(float, pair.split(","))
                    found_points.append((fx, fy))
                    
        # Load absolute dataset matrix to paint baseline background elements
        all_points = np.loadtxt(DATA_FILE)
        
        # Configure Matplotlib spatial frame state
        fig, ax = plt.subplots(figsize=(7, 7))
        ax.scatter(all_points[:, 0], all_points[:, 1], c='lightgray', s=8, label='Inactive Nodes / Drivers')
        
        if found_points:
            fx_arr, fy_arr = zip(*found_points)
            ax.scatter(fx_arr, fy_arr, c='red', s=25, label='Target Matches Selected')
            
        # Draw physical square representing current quadrant range constraint limits
        search_box = plt.Rectangle((qx - q_radius, qy - q_radius), q_radius * 2, q_radius * 2, 
                                     edgecolor='blue', facecolor='none', linewidth=2, label='Query Radius Boundary')
        ax.add_patch(search_box)
        
        ax.set_xlim(0, 100)
        ax.set_ylim(0, 100)
        ax.legend(loc='upper right')
        ax.set_title(f"Spatial Results Captured: {len(found_points)} targets within search area")
        
        st.pyplot(fig)
        
    except Exception as e:
        st.error(f"Pipeline Interruption: {e}")