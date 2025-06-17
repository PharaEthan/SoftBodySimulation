> This project is the final output of the Computer Graphics II module on physics simulation carried out at Keymiung University in Korea (2025).

# 💡 Soft Body Simulations HXPBD

This document explains how to interact with the simulations and what each `mainX.cpp` file does.

---

## 🕹️ User Interaction

- **Selection & Movement:**
  - Hold **LEFT_SHIFT** and **LEFT_CLICK** on an object to select it.
  - Once selected, move your mouse to drag the object in real-time.

- **Pressure & Wireframe:**
  - **Pressure** can only be modified on **closed objects** (e.g., cube, sphere...).
  - You must **select the object** before editing its pressure.
  - **Wireframe mode** allows you to see only the **vertices** instead of the full textured shape.

---

## 🎥 Camera Controls

- **Translate (left/right):**  
  - Hold **Left Click** and move your mouse horizontally.

- **Rotate the scene:**  
  - Hold **LEFT_CONTROL** + **Left Click**, then move the mouse.

- **Zoom In/Out:**  
  - Use the **mouse wheel**.

---

## 🧪 Simulation Files (`main*.cpp`)

| File        | Description |
|-------------|-------------|
| **main.cpp**   | **Empty simulation**. A clean template to create or test your own custom scenes. |
| **main1.cpp**  | **Sphere simulation**. <br/> You can: <br/> – Enable/disable **gravity** <br/> – Drag the sphere with the mouse <br/> – Change its **internal pressure** in real-time. |
| **main2.cpp**  | Interactive **cloth simulation**. <br/> You can grab points of the cloth and observe soft-body deformations. |
| **main3.cpp**  | Same as `main2.cpp`, but the cloth is **anchored** at its four corners using **FixedConstraints**, limiting edge motion. |

---

## 📌 Notes

- Objects must be properly **selected** before interacting with their properties.
- The simulation uses an **XPBD-based** constraint solver for realistic soft-body behavior.
- PBR materials are used to provide physically-based rendering with lighting and shadows.

---

## 🔧 Adding Your Own Scene

To create your own setup, duplicate `Main.cpp`, rename it (e.g., `main4.cpp`), and use the `SceneFactory` helpers:
```cpp
SceneFactory::CreateCarpet(...);
SceneFactory::CreateSphere(...);
```

## Compilation

In the `CMakeLists.txt`, you will find this:

```txt
# Change the main file to run different simulations
# Main.cpp  => Empty template for free sandbox using my engine.
# Main1.cpp => Simulation 1 using my engine.
# Main2.cpp => Simulation 2 using my engine.
# Main3.cpp => Simulation 3 using my engine.
Main1.cpp
```

To change the simulation, or just the hand of a project, simply change the line containing `Main1.cpp` to `Main2.cpp` or `YourMain.cpp`.

## Results

### Main1.cpp

<video width="320" height="240" controls>
  <source src="Docs/Simulation1A.mp4" type="video/mp4">
</video>
<video width="320" height="240" controls>
  <source src="Docs/Simulation1B.mp4" type="video/mp4">
</video>

### Main2.cpp

<video width="320" height="240" controls>
  <source src="Docs/Simulation2.mp4" type="video/mp4">
</video>

### Main3.cpp

<video width="320" height="240" controls>
  <source src="Docs/Simulation3.mp4" type="video/mp4">
</video>
