# House Layout Designer

A Qt-based application for designing house layouts with walls and furniture.

## Features

- Draw horizontal and vertical walls
- Add furniture (sofa, chair, table) to your layout
- Move furniture via drag-and-drop
- Rotate furniture
- Select, copy, cut, and paste furniture
- Undo/redo support
- Save and load projects

## Building the Project

### Requirements

- Qt 6.8.1 or later
- CMake 3.16 or later
- C++ compiler with C++17 support

### Steps

1. Clone or download the project
2. Create a build directory:
   
```bash
   mkdir build
   cd build
```
   
3. Configure the project with CMake:
   
```bash
   cmake ..
```

4. Build the project:

```bash
   cmake --build .
```
   
5. Run the application:

```bash
   ./HouseLayoutDesigner
```

Alternatively, you can open the project in QtCreator by opening the `CMakeLists.txt` file and use the integrated build and run functionality.

## Usage Instructions

### Creating a New Project

You can create projects in three sizes:
- Small (300x300)
- Medium (600x600)
- Large (800x600)

Select File > New and then the desired size.

### Drawing Walls

1. Click the "Wall" tool in the toolbar
2. Click and drag to draw a wall
3. Hold the Shift key while drawing to snap to horizontal or vertical walls

### Adding Furniture

1. Click on the desired furniture tool (Sofa, Chair, or Table)
2. Click in the design area to place the furniture

### Selecting and Moving Furniture

1. Click the "Select" tool
2. Click on a furniture item to select it
3. Drag the selected item to move it
4. Hold Ctrl while clicking to select multiple items

### Rotating Furniture

1. Select a single furniture item
2. Press 'R' to rotate clockwise, or Shift+R to rotate counter-clockwise
3. Alternatively, use the "Rotate" tool

### Copying, Cutting, and Pasting

- Use standard keyboard shortcuts (Ctrl+C, Ctrl+X, Ctrl+V)
- You can also use the Edit menu or toolbar buttons

### Saving and Loading

- Save your project using File > Save or the toolbar button
- Open existing projects with File > Open

## Keyboard Shortcuts

- Ctrl+S: Save
- Ctrl+O: Open
- Ctrl+Z: Undo
- Ctrl+Y: Redo
- Ctrl+X: Cut
- Ctrl+C: Copy
- Ctrl+V: Paste
- Delete: Delete selected furniture
- Ctrl+A: Select all
- R: Rotate clockwise
- Shift+R: Rotate counter-clockwise
- Esc: Clear selection

## Project Structure

The application is built using the Model-View-Controller pattern with a command pattern for undo/redo functionality.

Key classes:
- `MainWindow`: The main application window
- `DesignArea`: The canvas where the layout is drawn
- `Wall`: Represents walls in the layout
- `Furniture`: Base class for furniture items
- `Command`: Base class for the command pattern
- `Project`: Handles saving and loading projects
