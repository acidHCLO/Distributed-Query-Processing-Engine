Distributed Query Processing Engine

Language: C++ (C++17)

Architecture: 
Driver distributes CSV files to multiple engines. Each engine processes a file and returns stats: batchYear, minScore, maxScore, avgScore, count.
Driver merges result from all engines and computes average score per batch year.

Developed on:
Windows 11
Microsoft Visual Studio C++

Dependencies:
Standard C++ libraries only (no external dependencies)

Compilation(Linux):
Build using the provided Make file, which will create the executables engine and driver.

Running the System:
Start engine instances in separate terminals, eg.,
	./engine 1
	./engine 2
	./engine 3

Run the driver:
	./driver 1 2 3

Output: batchYear, minScore, maxScore, avgScore and will be written to results.txt

NOTE: 
The submission contains only the source code. Executable files(.exe) are not included.
