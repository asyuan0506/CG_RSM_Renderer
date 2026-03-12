# Real-time Global Illumination with Reflective Shadow Maps
A renderer developed in C++ and OpenGL 3.3, specializing in real-time indirect lighting and efficient multi-light management.

This renderer is designed specifically for .obj files and utilizes a Deferred Shading architecture to handle complex lighting environments.

## 🌟 Features 

* **Global Illumination via RSM:** Achieves real-time, indirect lighting for Directional and Spotlights by implementing Reflective Shadow Maps.
* **Deferred Shading Pipeline:** Decouples geometry rendering from lighting calculations, allowing efficient evaluation of multiple light sources.
* **Interactive UI:** Integrates GLUI to offer real-time control over model loading/deletion, model transformations, and light settings.
* **Complex Scene Support:** Capable of rendering multiple 3D models and evaluating point, directional, spot lights simultaneously.
* **Spherical Skybox:** UV-mapped 3D sphere environment background.
<div align="center">
    <figure style="display: inline-block; width: 48%; margin: 0;">
        <img src="https://github.com/user-attachments/assets/4dd90408-5881-4a67-a949-5213cbefd9c2" style="width: 100%;" />
        <figcaption><b>With RSM (Global Illumination)</b></figcaption>
    </figure>
    <figure style="display: inline-block; width: 48%; margin: 0;">
        <img src="https://github.com/user-attachments/assets/e6b9ae90-4bf4-42f3-8b4c-dc1fe549905a" style="width: 100%;" />
        <figcaption><b>Without RSM (Direct Light Only)</b></figcaption>
    </figure>
</div>
