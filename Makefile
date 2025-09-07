create_test_samples:
	python create_test_samples.py 

run_python:
	time python solution_py.py
	python evaluate_test.py 
	rm test_sample_results_calculated.txt

run_go1:
	GOOS=darwin GOARCH=arm64 \
		go build -trimpath -ldflags="-s -w -buildid=" \
		-o solution_go_1 solution_go_1.go
	time ./solution_go_1
	python evaluate_test.py 
	rm test_sample_results_calculated.txt
	rm solution_go_1

run_rust1:
	rustc -C opt-level=3 -C lto -C codegen-units=1 -C target-cpu=native -C panic=abort -o solution_rs_1 solution_rs_1.rs 
	time ./solution_rs_1
	python evaluate_test.py 
	rm test_sample_results_calculated.txt
	rm solution_rs_1

run_rust2:
	rustc -C opt-level=3 -C lto -C codegen-units=1 -C target-cpu=native -C panic=abort -o solution_rs_2 solution_rs_2.rs 
	time ./solution_rs_2 
	python evaluate_test.py 
	rm test_sample_results_calculated.txt
	rm solution_rs_2

run_c1:
	clang -std=c23 -O3 -ffast-math -march=native -flto -DNDEBUG -fvisibility=hidden -o solution_c_1 solution_c_1.c
	time ./solution_c_1
	python evaluate_test.py 
	rm test_sample_results_calculated.txt
	rm solution_c_1

run_c2:
	clang -std=c23 -O3 -ffast-math -march=native -flto -DNDEBUG -fvisibility=hidden -o solution_c_2 solution_c_2.c
	time ./solution_c_2
	python evaluate_test.py 
	rm test_sample_results_calculated.txt
	rm solution_c_2

