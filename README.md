# DebugFS Browser GUI 🐧

The DebugFS Browser GUI is a GTK+ application that provides a graphical user interface for browsing the contents of the Linux DebugFS file system. It allows users to explore various kernel debugging and tracing features conveniently.

## Features 🚀

- Browse and navigate the Linux DebugFS file system.
- Display information about files, directories, and attributes.
- Utilize icons to distinguish between different file types.
- Easily expand and collapse directory structures.
- Provides a user-friendly interface for kernel debugging and tracing tasks.

## Installation 🛠️

### Prerequisites

Before running the DebugFS Browser GUI, ensure you have the following dependencies installed:

- **GTK+ 3**
- **GLib**
- **GNU Compiler Collection (GCC)**

### Building from Source

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/yourusername/debugfs-browser-gui.git
   ```
2. Change into the project directory:

```bash
cd debugfs-browser-gui
```
3. Compile the application:

```bash
gcc -o debugfs-browser main.c treeview.c `pkg-config --cflags --libs gtk+-3.0`
```
4. Run the DebugFS Browser GUI:

```bash
./debugfs-browser
```
## Usage 😃
- Upon launching the application, set the mount point for DebugFS.
- The application will display the contents of DebugFS in a treeview.
- Click on directories to expand/collapse them.
- Files and directories are listed with their names and attributes.
- Icons are used to distinguish between directories and files.


## Contributing 🤝
Contributions are welcome! If you'd like to contribute to this project, please follow these steps:
- Fork the repository.
- Create a new branch for your feature or bug fix.
- Make your changes and commit them.
- Push your branch to your fork.
- Open a pull request to this repository.

## License 📜
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
This project was inspired by the need for a user-friendly interface to navigate the Linux DebugFS file system for kernel debugging and tracing purposes.

## Contact 📧
If you have any questions or suggestions, feel free to open a PR.

# Happy debugging!
