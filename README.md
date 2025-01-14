# ProjectName

> Last README update on
> 14:01:2024::22:04:46 IST

- I am learning Windows Programming, Computer Graphics and Game Development. This repository is my progress in learning these topics and daily practice. I will be using this for my future reference and to track my progress. Although anyone is free to use this repository for their learning. I have written QUESTION, ANSWER, and NOTES in the code to make it easier to understand.
- I am using Visual Studio Code for this project, as I also want to learn about MSVC compiler, because Visaul Studio provides all the settings required automatically but I want to learn about how ot do it manually.
- There are also some handwritten notes that I will be uploading on my website in future. I will provide the link here when I do that.
- There is a `update.exe` file in root directory you can run it using `./update.exe timeinmiliseconds` and it will automatically check if any .cpp, .h file has changed and if it has changed it will run `make` command to compile the project. You can find the output in the `/bin` directory. You can also run `./update.exe` without any arguments and it will assume `timeinmiliseconds` to be `1` i.e. 1 millisecond.

## Directory Structure

This document explains the directory structure of the project.

```bash
ProjectName/
├── assets/             # Non-code resources (e.g., images, fonts, etc.)
│ ├── images/           # Image files used in the project
│ ├── fonts/            # Font files used in the project
│ └── sounds/           # Sound files used in the project
├── build/              # Build artifacts (e.g., binaries, object files)
│ ├── debug/            # Debug build artifacts
│ └── release/          # Release build artifacts
├── include/            # Public header files
│ └── ProjectName/      # Project-specific header files
├── src/                # Source files like .cpp, .c
│ ├── main.cpp          # Main entry point of the application
│ └── ...               # Other source files
└── lib/                # Library files (if you have any)
├── external/           # External libraries used in the project
└── internal/           # Internal libraries built by you that are used in the project
```

## Folder Descriptions

### assets/

Contains non-code resources such as images, fonts, and sounds.

- **images/** - Stores image files used in the project.
- **fonts/** - Stores font files used in the project.
- **sounds/** - Stores sound files used in the project.

### build/

Contains build artifacts such as binaries and object files.

- **debug/** - Stores debug build artifacts.
- **release/** - Stores release build artifacts.

### include/

Contains public header files.

- **ProjectName/** - Stores project-specific header files.

### src/

Contains source files like .cpp and .c.

- **main.cpp** - The main entry point of the application.
- **...** - Other source files.

### lib/

Contains library files.

- **external/** - Stores external libraries used in the project.
- **internal/** - Stores internal libraries built by you that are used in the project.
