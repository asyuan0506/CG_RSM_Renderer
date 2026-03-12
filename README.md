# Real-time Global Illumination with Reflective Shadow Maps
A renderer developed in C++ and OpenGL 3.3, specializing in real-time indirect lighting and efficient multi-light management.

This renderer is designed specifically for .obj files and utilizes a Deferred Shading architecture to handle complex lighting environments.

## 🌟 Features 

* **Global Illumination via RSM:** Achieves real-time, indirect lighting for Directional and Spotlights by implementing Reflective Shadow Maps.
* **Deferred Shading Pipeline:** Decouples geometry rendering from lighting calculations, allowing efficient evaluation of multiple light sources.
* **Interactive UI:** Integrates GLUI to offer real-time control over model loading/deletion, model transformations, and light settings.
* **Complex Scene Support:** Capable of rendering multiple 3D models and evaluating point, directional, spot lights simultaneously.
* **Spherical Skybox:** UV-mapped 3D sphere environment background.
