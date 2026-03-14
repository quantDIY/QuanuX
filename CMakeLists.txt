cmake_minimum_required(VERSION 3.20)
project(QuanuX)

# Include the High-Frequency Execution Engine
add_subdirectory(QuanuX-Spreader/cpp)

# Include the Ingestion Engine
add_subdirectory(QuanuX-Annex)
