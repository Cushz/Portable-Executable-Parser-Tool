# Portable Executable (PE) Parser Tool

This project is a **Windows Portable Executable (PE) file parser** written in C.  
It demonstrates how to manually read and parse the structure of PE files using the **Windows API**, without relying on external libraries.  

The tool loads a given `.exe` file, validates its headers, and prints out important details such as file headers, optional headers, data directories, and section information.

---

## 📂 Project Structure

- **`pe_parser.c`** – Single C file containing:
  - A function `ReadPEFile()` to load an executable into memory.  
  - Header validation (`DOS Header`, `NT Header`, `Optional Header`).  
  - Extraction of **File Header** details (machine type, number of sections, optional header size).  
  - Parsing of **Optional Header** values (entry point, base addresses, OS versions).  
  - Listing of **Data Directories** (Export & Import tables).  
  - Iteration through all **Section Headers**.  

---

## ⚙️ Compilation & Running

You can compile this project with **Visual Studio**, **MinGW**, or any Windows-compatible C compiler.  

### Using MinGW (GCC for Windows):
```bash
gcc pe_parser.c -o pe_parser.exe
