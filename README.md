![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/API-OpenGL-5586A4?style=flat&logo=opengl&logoColor=white)
![Texture](https://img.shields.io/badge/Graphics-Texture_Mapping-FF6F00?style=flat)
![Transform](https://img.shields.io/badge/Math-Affine_Transformations-yellow?style=flat)
![GLSL](https://img.shields.io/badge/Shader-GLSL_Unified_Pipeline-blueviolet?style=flat)
![LodePNG](https://img.shields.io/badge/Library-lodepng-lightgrey?style=flat)
![Course](https://img.shields.io/badge/Course-BME--IIT_Graphics-green?style=flat)

# Smile Demo Application

This project is a C++ OpenGL application developed for the BME-IIT Computer Graphics course. It demonstrates the integration of **2D texture mapping** with geometric primitive rendering and real-time **affine transformations**.

The application renders a composite character ("Smiley") using procedural geometry over a textured background, managed by a unified GLSL shader pipeline.

---

## 📚 Features

* **Hybrid Rendering:** Combines textured quads (background) and solid-color geometric primitives (GL_TRIANGLE_FAN, GL_LINE_STRIP) in a single render loop.
* **Texture Mapping:** Implements UV coordinate mapping to render a full-screen background image (`bg.png`), loaded via the **LodePNG** library.
* **Interactive Transformations:** The character can be manipulated in real-time using Model matrices:
    * **Translation:** Movement along X/Y axes.
    * **Scaling:** Non-uniform scaling of the geometry.
    * **Rotation:** Z-axis rotation using the Rodrigues formula logic.
* **Unified Shader Architecture:** A single shader program handles both textured and non-textured objects using uniform switches (`useTexture`).

---

## 📦 Dependencies

- OpenGL 3.3+ (Core Profile)
- GLEW / GLAD (Extension loader)
- GLUT / GLFW (Window management)
- LodePNG (Included in source for PNG decoding)

---

## 🎮 Controls

The application uses keyboard inputs to modify the **Model-View-Projection (MVP)** matrix acting on the Smiley character.

| Key | Action | Description |
| :--- | :--- | :--- |
| **`T`** | **Translate** | Toggles between moving the character diagonally (+0.1, +0.1) and returning it back. |
| **`S`** | **Scale** | Toggles between scaling up (1.2x, 1.5x) and scaling down/resetting. |
| **`R`** | **Rotate** | Rotates the character by **45°** counter-clockwise around the Z-axis. |
| **`I`** | **Identity** | Resets all transformations (position, rotation, scale) to the default state. |
| **`Q`** | **Quit** | Exits the application. |

---

## 🛠️ Technical Implementation

### 1. Unified GLSL Pipeline
Instead of using separate shaders for the background and the character, this project uses a conditional fragment shader:

```glsl
uniform int useTexture; // 1 = Texture Mode, 0 = Color Mode

void main() {
    if (useTexture == 1) {
        fragmentColor = texture(samplerUnit, texCoord);
    } else {
        fragmentColor = vec4(color, 1);
    }
}
```

### 2. Texture Loading
The application integrates lodepng.cpp  to handle raw image decoding. The pixel data is then uploaded to the GPU memory and bound to a sampler2D uniform for the background quad.

### 3. Geometry Generation
The "Smiley" head is generated procedurally using the equation of a circle: $x = r \cdot \cos(\phi), \quad y = r \cdot \sin(\phi)$. It uses `GL_TRIANGLE_FAN` for the solid yellow fill and `GL_LINE_STRIP` for the black outline.

---

## 📦 Releases

| Version | Description | Download |
| :--- | :--- | :--- |
| **v1.0.0** | **Smile Demo v1.0.0** - Initial release of the OpenGL assignment. Contains the compiled application logic, shader resources, and the background texture (`bg.png`). | [Smile.Demo.Application.v1.0.0.zip](https://github.com/hajdu-patrik/Grafika-BME-Smile_CPP_OPENGL/releases/download/v1.0.0/Smile.Demo.Application.v1.0.0.zip) <br> [Source code (zip)](https://github.com/hajdu-patrik/Grafika-BME-Smile_CPP_OPENGL/archive/refs/tags/v1.0.0.zip) |
