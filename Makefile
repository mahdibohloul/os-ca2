all: meanCalculator directoryProcess csvProcess courseProcess

meanCalculator: mean_calculator.cpp
	g++ -std=c++17 -o meanCalculator mean_calculator.cpp

directoryProcess: directory_process.cpp DirectoryProcess.o
	g++ -std=c++17 -o directoryProcess directory_process.cpp DirectoryProcess.o

DirectoryProcess.o: DirectoryProcess.cpp DirectoryProcess.h
	g++ -std=c++17 -c -o DirectoryProcess.o DirectoryProcess.cpp

courseProcess: course_process.cpp CourseProcess.o
	g++ -std=c++17 -o courseProcess course_process.cpp CourseProcess.o

CourseProcess.o: CourseProcess.cpp CourseProcess.h
	g++ -std=c++17 -c -o CourseProcess.o CourseProcess.cpp

csvProcess: csv_process.cpp CSVProcess.o
	g++ -std=c++17 -o csvProcess csv_process.cpp CSVProcess.o

CSVProcess.o: CSVProcess.cpp CSVProcess.h
	g++ -std=c++17 -c -o CSVProcess.o CSVProcess.cpp

clean:
	rm -f meanCalculator directoryProcess csvProcess courseProcess DirectoryProcess.o CourseProcess.o CSVProcess.o