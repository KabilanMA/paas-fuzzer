This is the Query Generator Application to generate dynamic MySQL queries, store them and run them after five seconds.

## Prerequisites

- MySQL Server
- MySQL Connector/C++ (mysqlcppconn library)

## Installation

1. **Clone the repository:**
   git clone https://github.com/KabilanMA/paas-fuzzer.git

2. **Move to the Relevant Folder:**
   cd "paas-fuzzer/src/QueryGeneration"

3. **Build the project:**
   g++ -o main DatabaseHandler.cpp QueryGenerator.cpp LogFileHandler.cpp QueryFileHandler.cpp main.cpp -I/path/to/include/folder/QueryGeneration -lmysqlcppconn

4. **Run the project:**
   ./main


