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
```

### Run the tool:
```bash
pe_parser.exe
```

### When executed, the program will prompt you for the full path of the executable you want to analyze:
```bash
Please enter the full executable path: C:\Windows\System32\notepad.exe
```

### 🖥️ Example Output:
```bash
---------------FILE HEADER------------------
Size of Optional Header: 240 Bytes
Number of Sections: 5
Image architecture: x64

---------------OPTIONAL HEADER------------------
Size of Code: 16384
Major and Minor Linker version: 14 AND 29
Size of Initialized data section: 20480
Size of Uninitialized data section: 0
Address to Entry point in RVA: 8192 (in decimals)
Address of Entry point in VA: 0x00007FF712340000
Address of the code section in RVA: 4096
Address of the code section in VA: 0x00007FF712341000
Address of the ImageBase: 0x00007FF712340000
Major and Minor Operating system version: 6 AND 0

-------------------DATA DIRECTORIES--------------------------
EXPORT DIRECTORY
Export directory Address: 0x000001AABF020000
Export directory size: 112
Export directory RVA Address: 0x00004000
Import directory Address: 0x000001AABF021000
Import directory size: 240
Import directory RVA Address: 0x00005000

---------------------SECTIONS----------------------------------
[#] Section Name: .text
[#] Virtual Address: 0x000001AABF022000
[#] Section Name: .rdata
[#] Virtual Address: 0x000001AABF023000
...
```
