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

